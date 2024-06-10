#include <SoftwareSerial.h>

#define RXD2 14
#define TXD2 12

SoftwareSerial mySerial(RXD2, TXD2);

unsigned long previousMillis = 0; // Store the last time the command was executed
const long interval = 1000;       // Interval at which to execute the command (milliseconds)

void checkSerial() {
  if(mySerial.available()) {
    Serial.println(mySerial.readString());
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
  Serial.println(command);
  command.toCharArray(buf, command.length() + 1);
  mySerial.write(buf);
  free(buf);
  checkSerial();
}

void setup()
{
  Serial.begin(115200);
  mySerial.begin(115200);
  delay(1000);

  sendSerialData("AT+MODE=0"); //set tag
  delay(100);
  
  sendSerialData("AT+NETWORKID=TASTETHECODE"); //set network id
  delay(100);
  
  sendSerialData("AT+ADDRESS=TTC002"); //set address
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
  // unsigned long currentMillis = millis(); // Get the current time

  // // Check if one second has passed
  // if (currentMillis - previousMillis >= interval) {
  //   previousMillis = currentMillis;      // Update the time
  //   sendSerialData("AT+TAG_SEND=5,HELLO");
  // }
  checkSerial();
}
