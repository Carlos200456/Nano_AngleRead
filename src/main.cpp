#include <Arduino.h>
#include <EEPROM.h>

#define IDIup 2
#define IDIdw 3
#define CArco A0
#define CRota A1
#define SDD A2
#define Techo A3
String inputString = "";               // a string to hold incoming data
String Tipo ="";
String Signo ="";
String Magnitud ="";
String ACK ="ACK";
String NACK ="NACK";
bool DataReady = false;             // whether the string is complete
bool error = false;                 // whether the string have errors
unsigned long lastDebounceTimeSC = 0;  // the last time the output SC was toggled
unsigned long debounceDelaySC = 20;    // the debounce time; increase if the output flickers
unsigned long lastDebounceTimeCI = 0;  // the last time the output CI was toggled
unsigned long debounceDelayCI = 20;    // the debounce time; increase if the output flickers
int buttonStateSC;                     // the current reading from the input pin
int buttonStateCI;                     // the current reading from the input pin
int lastButtonStateSC = 0;           // the previous reading from the input pin SC
int lastButtonStateCI = 0;           // the previous reading from the input pin CI
int eeAddress; //EEPROM address to start reading from
void setup() {
  // initialize serial:
  Serial.begin(57600);
  // reserve 20 bytes for the inputString:
  inputString.reserve(20);
  pinMode(IDIup, INPUT_PULLUP);
  pinMode(IDIdw, INPUT_PULLUP);
  pinMode(CArco, INPUT);
  pinMode(CRota, INPUT);
  pinMode(SDD, INPUT);
  pinMode(Techo, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:



}

/*
 SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      DataReady = true;
    }
  }
}
int ReadEEPROM(int i){
  int Data;
  eeAddress = sizeof(int) * i; 
  //Get the int data from the EEPROM at position 'eeAddress'
  EEPROM.get(eeAddress, Data);
  return Data;
}

void WriteEEPROM(int i, int Data){
  eeAddress = sizeof(int) * i; 
  EEPROM.put(eeAddress, Data);
}

