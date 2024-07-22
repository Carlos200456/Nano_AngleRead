#include <Arduino.h>
#include <EEPROM.h>
#include <U8g2lib.h>
#include "Functions.h"

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

int CangleX, LangleX, TangleX, Tang, C0, C1023, L0, L1023, T0, T1023, pasos = 0;
float Cang, Lang, CangF, LangF;


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
  C0 = ReadEEPROM(0);
  C1023 = ReadEEPROM(1);
  L0 = ReadEEPROM(2);
  L1023 = ReadEEPROM(3);
  T0 = ReadEEPROM(4);
  T1023 = ReadEEPROM(5);
}

void loop() {
  ReadAnalogVal();
  Analyze_Data_In();

  Cang = map(CangleX, C0, C1023, -90, 90); // Map the analog value to degrees
  Lang = map(LangleX, L0, L1023, -90, 90); // Map the analog value to degrees
  Tang = map(TangleX, T0, T1023, 0, 180);  // Map the analog value to degrees

  // Low-pass filter
  CangF = 0.95 * CangF + 0.05 * Cang;
  LangF = 0.95 * LangF + 0.05 * Lang;

  if (pasos == 40){
    u8x8.setCursor(0,3);             // Column, Row
    u8x8.print(" C     L     T  ");
    u8x8.setCursor(0,4);             // Column, Row
    u8x8.print("                ");
    if (CangF >= 0) {
      u8x8.setCursor(0,4);             // Column, Row
      u8x8.print("+");
      u8x8.setCursor(1,4);             // Column, Row
      u8x8.print(CangF, 0);
    } else {
      u8x8.setCursor(0,4);             // Column, Row
      u8x8.print(CangF, 0);
    }
    if (LangF >= 0) {
      u8x8.setCursor(6,4);             // Column, Row
      u8x8.print("+");
      u8x8.setCursor(7,4);             // Column, Row
      u8x8.print(LangF, 0);
    } else {
      u8x8.setCursor(6,4);             // Column, Row
      u8x8.print(LangF, 0);
    }
    if (Tang >= 0) {
      u8x8.setCursor(12,4);             // Column, Row
      u8x8.print("+");
      u8x8.setCursor(13,4);             // Column, Row
      u8x8.print(Tang);
    } else {
      u8x8.setCursor(12,4);             // Column, Row
      u8x8.print(Tang);
    }

    if (Tang < 50) {                    // Invert the display for L position
      // L position Lateral
      if (CangF >= 0) Serial.print("  "); else Serial.print(" "); Serial.print(CangF, 0); if (abs(CangF) < 10) Serial.print(" "); Serial.print(",");
      if (LangF >= 0) Serial.print("  "); else Serial.print(" "); Serial.print(LangF, 0); if (abs(LangF) < 10) Serial.print(" "); 
    } else {
      // L position Head
      if (LangF >= 0) Serial.print("  "); else Serial.print(" "); Serial.print(LangF, 0); if (abs(LangF) < 10) Serial.print(" "); Serial.print(",");
      if (CangF >= 0) Serial.print("  "); else Serial.print(" "); Serial.print(CangF, 0); if (abs(CangF) < 10) Serial.print(" ");
    }

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

void ReadAnalogVal(void){
  CangleX = analogRead(CArco);
  delay(1);
  CangleX = analogRead(CArco);
  LangleX = analogRead(CRota);
  delay(1);
  LangleX = analogRead(CRota);
  TangleX = analogRead(Techo);
  delay(1);
  TangleX = analogRead(Techo);
}

void Analyze_Data_In(void){
  if (DataReady) {  // ---------------- Data Analisys Start --------------------
    String Tipo = "";
    String Magnitud = "";
    Tipo = inputString.substring(0,2);
    Magnitud = inputString.substring(2);

    if (Tipo == "IN"){            // Comando de Reset de EEPROM
        WriteEEPROM(0, 0);        // C0
        WriteEEPROM(1, 1023);     // C1023
        WriteEEPROM(2, 0);        // L0
        WriteEEPROM(3, 1023);     // L1023
        WriteEEPROM(4, 0);        // T0
        WriteEEPROM(5, 1023);     // T1023
      goto jmp;
    }

    if (Tipo == "C+"){            // Comando de calibracion de "C" en +90°
      if (Magnitud.toInt() == 90){
        C0 = CangleX;
        WriteEEPROM(0, C0);
      } else error = true;
      goto jmp;
    }

    if (Tipo == "C-"){          // Comando de calibracion de "C" en -90°
      if (Magnitud.toInt() == 90){
        C1023 = CangleX;
        WriteEEPROM(1, C1023);
      } else error = true;
      goto jmp;
    }

    if (Tipo == "L+"){          // Comando de calibracion de "L" en +90°
      if (Magnitud.toInt() == 90){
        L0 = LangleX;
        WriteEEPROM(2, L0);
      } else error = true;
      goto jmp;
    }

    if (Tipo == "L-"){            // Comando de calibracion de "L" en -90°
      if (Magnitud.toInt() == 90){
        L1023 = LangleX;
        WriteEEPROM(3, L1023);
      } else error = true;
      goto jmp;
    } 

    if (Tipo == "T="){            // Comando de calibracion de "T" en 0°
      if (Magnitud.toInt() == 0){
        T0 = TangleX;
        WriteEEPROM(4, T0);
      } else error = true;
      goto jmp;
    }

    if (Tipo == "T="){            // Comando de calibracion de "T" en 180°
      if (Magnitud.toInt() == 180){
        T1023 = TangleX;
        WriteEEPROM(5, T1023);
      } else error = true;
      goto jmp;
    }

    // End of Data Analisys -------------------

    error = true;      // Si el comando de entrada es desconocido setear error
    jmp:

    // error = CheckGenPW();

    if (error) {
      Serial.println(NACK);
    } else {
      Serial.println(ACK);
    }
    error = false;
    // clear the string:
    inputString = "";
    DataReady = false;

  } // -------------------------------- Data Analisys End --------------------
}
