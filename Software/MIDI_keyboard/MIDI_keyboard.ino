#include <USB-MIDI.h>
#include <pitchToNote.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();

using namespace MIDI_NAMESPACE;

/* Useful defines */
#define COL_START        0
#define COL_END          5
#define ROW_START        6
#define ROW_END          11
#define COLS_PER_ROW     6
#define LEFT_START_ROW   6
#define RIGHT_START_ROW  8
#define NUM_KEYS         24
#define VELOCITY         64
#define CHANNEL          1
#define SCAN_RATE_MS     5
#define NUM_POTS         4

// Global variables
uint8_t keyPressed[NUM_KEYS];
uint8_t potVals[NUM_POTS];
unsigned long t0 = millis(); // Need this for delay counting
uint8_t pinMap[12]; // I did not read the data sheet carefully enough so now I need a pin map

///
/// Scan a row of the key matrix, there are 4 rows, each is half an octave.
/// @param row tthe row to scan
/// @param baseNote MIDI note to count from
/// @param distance incremented with how many keys were scanned, used for calculation of note based on base note
///
int scanRow(int row, uint8_t baseNote, uint8_t* distance){
  for(int iCol = COL_START; iCol <= COL_END; iCol++){
    digitalWrite(pinMap[iCol], 0);
    uint8_t checkPressed = !digitalRead(pinMap[row]);
    
    uint8_t idx = ((row-ROW_START)*COLS_PER_ROW)+(iCol-COL_START);
    if(checkPressed != keyPressed[idx]){
      // Only send a MIDI message if the key has changed from previous state
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

///
/// Scan the 4 potentiometers and send MIDI change control messages if updated
///
void scanPots(){
  for(int iPot = 0; iPot < NUM_POTS; iPot++){
    uint8_t potVal = 127 - (uint8_t)(analogRead(A0+iPot) >> 3);
    
    if(potVal !=  potVals[iPot]){
      potVals[iPot] = potVal;
      MIDI.sendControlChange(iPot, potVal, CHANNEL);
    }
  }

}


///
/// Setup
///
void setup() {
  MIDI.begin(CHANNEL);  
  
  //set up pin map, this let's me easily loop through the pins for scanning.
  for(int iPin = 0; iPin < sizeof(pinMap)/sizeof(uint8_t); iPin++){
    pinMap[iPin] = iPin+2; 
  }
  pinMap[9] = 16;
  pinMap[10] = 14;
  pinMap[11] = 15;

  //set up pins
  for(int iRow = ROW_START; iRow <= ROW_END; iRow++){
    //pinMode(pinMap[iRow], INPUT_PULLUP);
  }
  for(int iCol = COL_START; iCol <= COL_END; iCol++){
    pinMode(pinMap[iCol], OUTPUT);
    digitalWrite(pinMap[iCol], 1);
  }

}

///
/// Main loop
///
void loop() {
  
  if(millis()-t0 > SCAN_RATE_MS){
    // For a steady scan rate.
    t0 = millis();
    uint8_t leftBaseNote = pitchC1; // These should be updated with the value of the rotary switches
    uint8_t rightBaseNote = pitchC2;
    uint8_t leftCounter = 0; // Counters to keep track of what  key I am scanning and it's distance from the base key
    uint8_t rightCounter = 0;

    scanPots();

    // Scan the 2 rows of the left octave.
    for(int iRow = LEFT_START_ROW; iRow < LEFT_START_ROW+2; iRow++){
      scanRow(iRow, leftBaseNote, &leftCounter);
    }
    // Scan the 2 rows of the right octave
    for(int iRow = RIGHT_START_ROW; iRow < RIGHT_START_ROW+2; iRow++){
      scanRow(iRow, rightBaseNote, &rightCounter);
    }
  }
}
