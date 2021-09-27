#include <USB-MIDI.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();

unsigned long t0 = millis();
unsigned long tClock = millis();

using namespace MIDI_NAMESPACE;


byte note;
byte velocity;
byte channel;
byte already_on;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  velocity = 55;
  channel = 1;
  already_on = 0;
  MIDI.begin(1);  
}

void loop() {
  // put your main code here, to run repeatedly:

  // send a note every second
  // (dont cáll delay(1000) as it will stall the pipeline)
 

  if ((millis() - t0) > 500 && !already_on)
  {
    t0 = millis();
    note = random(32,96);
    MIDI.sendNoteOn(note, velocity, channel);
    MIDI.sendNoteOn(note, velocity, channel);

    already_on = 1;
    Serial.print("note on!!!!!\n");
  }
  if ((millis() - t0) > 500 && already_on)
  {
    t0 = millis();
    MIDI.sendNoteOff(note, velocity, channel);
    already_on = 0;
    Serial.print("note off!!!!!\n");

  }

  if ((millis() - tClock) > 20)
  {
    tClock = millis();
    MIDI.sendRealTime(MidiType::Clock);
  }
}
