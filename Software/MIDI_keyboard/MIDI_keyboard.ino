#include <USB-MIDI.h>
#include <frequencyToNote.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();

unsigned long t0 = millis();
unsigned long tClock = millis();

using namespace MIDI_NAMESPACE;


#define COL_START 2
#define COL_END 7
#define ROW_START 8
#define ROW_END 15
#define COLS_PER_ROW 6

#define NUM_KEYS 24


uint8_t keyPressed[NUM_KEYS];






int scanKey(int row, uint8_t baseNote){
  for(int iCol = COL_START; iCol <= COL_END; iCol++){
    digitalWrite(iCol, 0);
    uint8_t checkPressed = ~digitalRead(row);
    if(checkPressed != keyPressed[((row-ROW_START)*COLS_PER_ROW)+iCol]){
      if(checkPressed){
        //note on baseNote + distance from base
      }
      else{
        //note off baseNote + distance from 
      }
      keyPressed[((row-ROW_START)*COLS_PER_ROW)+iCol] ^= 1;

    }
    digitalWrite(iCol, 1);
  }
}
  

void setup() {
  // put your setup code here, to run once:
  for(int iRow = ROW_START; iRow <= ROW_END; iRow++){
    pinMode(iRow, INPUT_PULLUP);
  }
  for(int iCol = COL_START; iCol <= COL_END; iCol++){
    pinMode(iCol, OUTPUT);
    digitalWrite(iCol, 1);
  }
}

void loop() {
  uint8_t leftHandBaseNote;
  uint8_t rightHandBaseNote;
}
