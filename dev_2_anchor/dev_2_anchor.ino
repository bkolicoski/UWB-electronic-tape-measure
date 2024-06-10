#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RXD2 14
#define TXD2 12

// Define the OLED display parameters
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

SoftwareSerial mySerial(RXD2, TXD2);

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long previousMillis = 0; // Store the last time the command was executed
const long interval = 100;       // Interval at which to execute the command (milliseconds)

void checkSerial() {
  if(mySerial.available()) {
    String received = mySerial.readString();
    if(received.indexOf("+ANCHOR_RCV")) {
      String distance = parseDistance(received);
      //Serial.print("Distance: ");
      Serial.println(distance);
      displayDistance(distance);
    } else {
      Serial.print("Received: ");
      Serial.println(received);
    }
  }
}

void sendCommand(String command) {
  // Converts command string to byte array and sends it over UART
  for (unsigned int i = 0; i < command.length(); i += 2) {
    byte byteToSend = (hexToByte(command.charAt(i)) << 4) | hexToByte(command.charAt(i + 1));
    mySerial.write(byteToSend);
  }
}
byte hexToByte(char hexChar) {
  // Convert a single hex character to a 4-bit value
  if (hexChar >= '0' && hexChar <= '9') {
    return hexChar - '0';
  } else if (hexChar >= 'A' && hexChar <= 'F') {
    return hexChar - 'A' + 10;
  }
  return 0; // Error case (not handled explicitly in this example)
}

void sendSerialData(String command) {
  command = command + "\r\n";
  char* buf = (char*) malloc(sizeof(char) * command.length() + 1);
  //Serial.println(command);
  command.toCharArray(buf, command.length() + 1);
  mySerial.write(buf);
  free(buf);
  checkSerial();
}

String parseDistance(String input) {
  String distance = "";
  int startPos = input.indexOf(',');
  int endPos = input.lastIndexOf(" cm");

  if (startPos != -1 && endPos != -1) {
    startPos = input.lastIndexOf(',', endPos) + 1;
    // Extract the substring containing the distance
    distance = input.substring(startPos, endPos);
    distance.trim();  // Remove any leading/trailing whitespace
  }
  
  return distance;
}

void displayDistance(String distance) {
  // Clear the buffer
  display.clearDisplay();
  
  // Set text color and size
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(3);
  display.setCursor(0, 0);
  
  float distanceVal = distance.toFloat();

  // Check if the distance is over 100 cm
  if (distanceVal > 100.0) {
    float distanceInMeters = distanceVal / 100.0;
    display.print(distanceInMeters, 1); // Display one decimal place
    display.print(" m");
  } else {
    display.print(distanceVal, 0);
    display.print(" cm");
  }
  
  // Update the display with the buffer's contents
  display.display();
}

void setup()
{
  Serial.begin(115200);
  mySerial.begin(115200);
  delay(1000);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  sendSerialData("AT+MODE=1"); //set anchor
  delay(100);
  
  sendSerialData("AT+NETWORKID=TASTETHECODE"); //set network id
  delay(100);
  
  sendSerialData("AT+ADDRESS=TTC001"); //set address
  delay(100);
  
  sendSerialData("AT+CPIN=FABC0002EEDCAA90FABC0002EEDCAA90"); //set password
  delay(100);
}

void loop() {
  if (Serial.available()){
    String content = Serial.readString();
    content.trim();
    sendSerialData(content);
  }
  // Check if one second has passed
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();      // Update the time
    sendSerialData("AT+ANCHOR_SEND=TTC002,4,TEST");
  }
  
  checkSerial();
}
