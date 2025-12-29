//---------------------------------------------------------------
//
// M2 keyboard implementatie using Arduino Nano
//                 version 04
// Change log:
//  6/4/25  Added key processing for arrows and backspace
//          Made SendChar a seperate function
//  7/4/25  Test with Arduino Pro Mini -> OK
//          life blink is timer based (8 sec)
// 10/4/25  Implemented code for F1~F12.
//          Have PuTTY keyboard behaviour set to VT100+
//---------------------------------------------------------------
//
// Decisions:
//  Busy check not implemented. Seems not needed for normal typing.
//  Signal is now used to detect M2 powered off/on.
//
//---------------------------------------------------------------
#include <SoftwareSerial.h> ;

const int LF = 10;  //cntrl-J
const int CR = 13;  //cntrl-M         Black (Ground)
const int Busy      = 2;  //  pin D2  White
const int ClockLine = 3;  //  pin D3  Grey
const int DataLine  = 4;  //  pin D4  Purple
long BlinkInterval = 8000;  //8000 msec, 8 seconds
const long BlinkOn       =   10;  //  10 msec blink on
long CurrentMillis  = 0;
long PreviousMillis = 0;

unsigned char CharBuffer[1];  // for incoming characters
int M2Status = 0;             // detect power on M2 on Busy line (0 is off, 1 is on)
long QuarterPulse = 250;      // gives 1ms quarterpulse
int Code;                     // used for the character received from the PC terminal

void setup() {
  pinMode(LED_BUILTIN,  OUTPUT);
  pinMode(Busy,         INPUT);
  pinMode(ClockLine,    OUTPUT);
  pinMode(DataLine,     OUTPUT);
  DataBit(HIGH);  // non-active state
  ClockBit(LOW);  // non-active state
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(9600);      // Any baud rate should work
  Serial.setTimeout(100);  //set the Timeout to 100ms
  Welcome();
  while (Serial.available()) {
    Serial.readBytes(CharBuffer, 1);  //Clear buffer during setup
    Serial.print(Code);
  }
}

void loop() {
  Alive();
  // Check serial input from PC
  if (Serial.available()) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(10);
    digitalWrite(LED_BUILTIN, LOW );
    // read the incoming character and process when needed
    Serial.readBytes(CharBuffer, 1);  //this should read 1 character
    Code = CharBuffer[0];             //but it waits for CR
    switch (Code) {
      case CR:                        // CR
        Serial.print(char(LF));       // send LF to terminal
        break;
      case 20:                        //ctrl-T (Welcome to terminal)
        Welcome();
        break;
      case 127:            // backspace on PC (7F) becomes 08
        Code = 8;
        break;
      case 27:                                 // start of esc sequence, followed by [A for arrow up
        if (Serial.available() == 0) {break;}  // This is the case when only ESC key is pressed.
        Serial.readBytes(CharBuffer, 1);       // get the [ character
        Serial.readBytes(CharBuffer, 1);       // get the real key
        Code = CharBuffer[0];
        switch (Code) {
          case 65:  Code = 30;  break;     // [A becomes up arrow     (1E)
          case 66:  Code = 31;  break;     // [B becomes down arrow   (1F)
          case 67:  Code = 29;  break;     // [C becomes right arrow  (1D)
          case 68:  Code = 28;  break;     // [D becomes left arrow   (1C)
          
          case 80:  Code = 01;  break;     // F1  [P becomes F1    (01)       Have PuTTY keyboard emulation as VT100+
          case 81:  Code = 02;  break;     // F2  [Q becomes F2    (02)
          case 82:  Code = 04;  break;     // F3  [R becomes F3    (04)
          case 83:  Code = 12;  break;     // F4  [S becomes F4    (0C)
          case 84:  Code = 21;  break;     // F5  [T becomes F5    (15)
          case 85:  Code = 16;  break;     // F6  [U becomes F6    (10)
          case 86:  Code = 14;  break;     // F7  [V becomes F7    (08)
          case 87:  Code = 19;  break;     // F8  [W becomes F8    (13)

          case 88:  Code = 03;  break;     // F9  [X becomes BREAK (03)
          case 89:  Code = 00;  break;     // F10 [Y becomes HOLD  (00)
     //   case 90:  Code = XX;  break;     // F11 [X becomes ~ NOT ASSIGNED ~
     //   case 91:  Code = XX;  break;     // F12 [Y becomes ~ NOT ASSIGNED ~

          default:
            break;
        }
        break;
      default:
        break;
    }
    if (Code == 0) {
      Serial.print ("[HOLD]");
    } else{
      Serial.print(char(Code));       // ECHO
    }
    SendChar( Code );
    //delay(30);
  }                                 // end if serial available
}                                   // end loop

void SendChar (int Code) {
  // Send (translated) character to M2
  // 8 databits
  for (int b = 0; b < 8; b++) {
    byte bit = bitRead(Code, b);
    //Serial.print(bit);
    if (bit) {         // part 1
      DataBit(HIGH);
    } else {
      DataBit(LOW);
    }
    ClockBit(LOW);

    if (bit) {         // part 2
      DataBit(HIGH);
    } else {
      DataBit(LOW);
    }
    ClockBit(HIGH);

    DataBit(HIGH);     // part 3
    ClockBit(HIGH);
    DataBit(HIGH);     // part 4
    ClockBit(LOW);
  }
  // stop bit
  DataBit(LOW);
  ClockBit(LOW);  // non-active state
  DataBit(HIGH);  // non-active state
}

void DataBit(int db) {
  digitalWrite(DataLine, db);
}

void ClockBit(int db) {
  digitalWrite(ClockLine, db);
  //delay(QuarterPulse);
  delayMicroseconds(QuarterPulse);  //use delaymicroseconds to create the pulse sequence.
}

void Alive() {
  CurrentMillis = millis();                                 // alive blink
  M2Status = digitalRead(Busy);
  if (!M2Status) {        // M2 problem, keep blinking !!
    BlinkInterval = 400;
  } else {   
    BlinkInterval = 8000;
  }
 if (CurrentMillis - PreviousMillis >= BlinkInterval) {
    PreviousMillis = CurrentMillis;
    digitalWrite(LED_BUILTIN, LOW);
  }
  if (CurrentMillis - PreviousMillis  == BlinkInterval - BlinkOn ) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}


void Welcome() {
  Serial.print("\n\r");
  Serial.println("M2 keyboard interface v1.00 active");
  Serial.println("Use PuTTY keyboard emulation as VT100+");
  Serial.println("F9 = BREAK, F10 = HOLD");
  
  M2Status = digitalRead(Busy);
  if (M2Status) {        // the line shows Busy* (inverted) !!
    Serial.println("M2 is powered on");
  } else {
    Serial.println("M2 is powered off or not connected");
  }
  Serial.println("(C) 2025, Ruud Broers");
  Serial.println("(Cntrl-T for this message)");
}
