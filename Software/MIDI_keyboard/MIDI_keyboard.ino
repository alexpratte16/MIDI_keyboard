#include <USB-MIDI.h>
#include <pitchToNote.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();



using namespace MIDI_NAMESPACE;


#define COL_START       0
#define COL_END         5 
#define ROW_START       6
#define ROW_END         11
#define COLS_PER_ROW    6 
#define LEFT_START_ROW  6
#define RIGHT_START_ROW 8
#define NUM_KEYS        24
#define VELOCITY        64
#define CHANNEL         1
#define SCAN_RATE_MS    5
#define NUM_POTS        4

uint8_t keyPressed[NUM_KEYS];
uint8_t potVals[NUM_POTS];
unsigned long t0 = millis();
uint8_t pinMap[12]; //I did not read the data sheet carefully enough so now I need a pin map





void scanRow(int row, uint8_t baseNote, uint8_t* distance){
  
  for(int iCol = COL_START; iCol <= COL_END; iCol++){
    digitalWrite(pinMap[iCol], 0);
    uint8_t checkPressed = !digitalRead(pinMap[row]);
    uint8_t idx = ((row-ROW_START)*COLS_PER_ROW)+(iCol-COL_START);
    if(checkPressed != keyPressed[idx]){
      if(checkPressed){
        //note on baseNote + distance from base
        MIDI.sendNoteOn(baseNote+(*distance), VELOCITY, CHANNEL);
      }
      else{
        MIDI.sendNoteOff(baseNote+(*distance), VELOCITY, CHANNEL);
      }
      keyPressed[idx] = !keyPressed[idx];

    }
    digitalWrite(pinMap[iCol], 1);
    (*distance)++;
  }
}

void scanPots(){
  for(int iPot = 0; iPot < NUM_POTS; iPot++){
    uint8_t potVal = 127 - (uint8_t)(analogRead(A0+iPot) >> 3);
    
    if(potVal !=  potVals[iPot]){
      potVals[iPot] = potVal;
      MIDI.sendControlChange(iPot, potVal, CHANNEL);
    }
  }

}

void setup() {
  //Serial IO
  Serial.begin(115200);
  //while (!Serial);
  MIDI.begin(CHANNEL);  
  
  //set up pin map
  for(int iPin = 0; iPin < sizeof(pinMap)/sizeof(uint8_t); iPin++){
    pinMap[iPin] = iPin+2;
  }
  pinMap[9] = 16;
  pinMap[10] = 14;
  pinMap[11] = 15;

  //set up pins
  for(int iRow = ROW_START; iRow <= ROW_END; iRow++){
    pinMode(pinMap[iRow], INPUT_PULLUP);
  }
  for(int iCol = COL_START; iCol <= COL_END; iCol++){
    pinMode(pinMap[iCol], OUTPUT);
    digitalWrite(pinMap[iCol], 1);
  }

  for(int iPot = 0; iPot < NUM_POTS; iPot++){
    potVals[iPot] = 0;
  }

}

void loop() {
  if(millis()-t0 > SCAN_RATE_MS){
    t0 = millis();
    uint8_t leftBaseNote = pitchC1;
    uint8_t rightBaseNote = pitchC2;
    uint8_t leftCounter = 0;
    uint8_t rightCounter = 0;


    scanPots();
    
    for(int iRow = LEFT_START_ROW; iRow < LEFT_START_ROW+2; iRow++){
      scanRow(iRow, leftBaseNote, &leftCounter);
    }
    for(int iRow = RIGHT_START_ROW; iRow < RIGHT_START_ROW+2; iRow++){
      scanRow(iRow, rightBaseNote, &rightCounter);
    }
  }
}
