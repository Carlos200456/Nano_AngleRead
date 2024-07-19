#include <Arduino.h>
#include <EEPROM.h>
#include <U8g2lib.h>

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

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
int eeAddress; //EEPROM address to start reading from

int CangleX, LangleX, TangleX, Tang, pasos = 0;
float Cang, Lang, CangF, LangF;


void ReadAnalogVal(void){
  int test;
  test = analogRead(CArco);
  CangleX = analogRead(CArco);
  test = analogRead(CRota);
  LangleX = analogRead(CRota);
  test = analogRead(Techo);
  TangleX = analogRead(Techo);
}



void setup() {
  // initialize serial:
  Serial.begin(9600);                 // Start the serial terminal
  
  u8x8.begin();  // initialize with the I2C
  u8x8.setPowerSave(0);
  // init done
  u8x8.setFont(u8x8_font_pxplustandynewtv_r);  // u8x8_font_torussansbold8_r, u8x8_font_chroma48medium8_r, u8x8_font_victoriamedium8_r, u8x8_font_pxplustandynewtv_r
  u8x8.draw1x2String(0,0,"Cartech");
  u8x8.setCursor(0,2);             // Column, Row
  u8x8.print("GEO Angle Read");

  // reserve 20 bytes for the inputString:
  inputString.reserve(20);
  pinMode(CArco, INPUT);
  pinMode(CRota, INPUT);
  pinMode(SDD, INPUT);
  pinMode(Techo, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  ReadAnalogVal();

  Cang = map(CangleX, 0, 1023, -90, 90); // Map the analog value to degrees
  Lang = map(LangleX, 0, 1023, -90, 90); // Map the analog value to degrees
  Tang = map(TangleX, 0, 1023, 0, 180); // Map the analog value to degrees

  // Low-pass filter
  CangF = 0.95 * CangF + 0.05 * Cang;
  LangF = 0.95 * LangF + 0.05 * Lang;




  if (pasos == 40){
    u8x8.setCursor(0,3);             // Column, Row
    u8x8.print("    C     L     ");
    u8x8.setCursor(0,4);             // Column, Row
    u8x8.print("                ");
    if (CangF >= 0) {
      u8x8.setCursor(3,4);             // Column, Row
      u8x8.print("+");
      u8x8.setCursor(4,4);             // Column, Row
      u8x8.print(CangF, 0);
    } else {
      u8x8.setCursor(3,4);             // Column, Row
      u8x8.print(CangF, 0);
    }
    if (LangF >= 0) {
      u8x8.setCursor(9,4);             // Column, Row
      u8x8.print("+");
      u8x8.setCursor(10,4);             // Column, Row
      u8x8.print(LangF, 0);
    } else {
      u8x8.setCursor(9,4);             // Column, Row
      u8x8.print(LangF, 0);
    }
    if (Tang >= 0) {
      u8x8.setCursor(6,5);             // Column, Row
      u8x8.print("+");
      u8x8.setCursor(7,5);             // Column, Row
      u8x8.print(Tang);
    } else {
      u8x8.setCursor(6,5);             // Column, Row
      u8x8.print(Tang);
    }

    if (CangF >= 0) Serial.print("  "); else Serial.print(" "); Serial.print(CangF, 0); if (abs(CangF) < 10) Serial.print(" "); Serial.print(",");
    if (LangF >= 0) Serial.print("  "); else Serial.print(" "); Serial.print(LangF, 0); if (abs(LangF) < 10) Serial.print(" "); 
    Serial.print(", 0000, 0000, XXX, XXX,"); 
    Serial.println();
    pasos = 0;
  } else pasos += 1;
  delay(10);
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

