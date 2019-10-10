/************************************************
Changeable Control Change Controller
by Tim Bartlett & Enthusiastic Electrons, 2019
midi controller pedal with knobs, buttons, and switches
will send CC data via USB midi
programmed for a Teesny 3.0 - 3.2 pinout
************************************************/

#include <Bounce.h>

#define debounceTime 5 // adjust debounce time for buttons
#define TOTAL_PINS 34 // there are 34 pins on a Teensy 3.0

#define NUM_KNOBS 11
#define NUM_BUTTONS 3
#define NUM_TOGGLES 3
#define NUM_JACKS 2

#define KNOB_01 A0
#define KNOB_02 A1
#define KNOB_03 A2
#define KNOB_04 A3
#define KNOB_05 A4
#define KNOB_06 A5
#define KNOB_07 A6
#define KNOB_08 A7
#define KNOB_09 A8
#define KNOB_10 A9
#define KNOB_11 A10
#define BUTTON_1 0
#define BUTTON_2 1
#define BUTTON_3 2
#define TOGGLE_1 3
#define TOGGLE_2 11
#define TOGGLE_3 12
#define JACKIN_1 6
#define JACKIN_2 7

Bounce BUTTON_1_BOUNCE = Bounce(BUTTON_1, debounceTime);
Bounce BUTTON_2_BOUNCE = Bounce(BUTTON_2, debounceTime);
Bounce BUTTON_3_BOUNCE = Bounce(BUTTON_3, debounceTime);
Bounce TOGGLE_1_BOUNCE = Bounce(TOGGLE_1, debounceTime);
Bounce TOGGLE_2_BOUNCE = Bounce(TOGGLE_2, debounceTime);
Bounce TOGGLE_3_BOUNCE = Bounce(TOGGLE_3, debounceTime);
Bounce JACKIN_1_BOUNCE = Bounce(JACKIN_1, debounceTime);
Bounce JACKIN_2_BOUNCE = Bounce(JACKIN_2, debounceTime);

const uint8_t channel = 1; // set the midi channel

const uint8_t theKnobs[NUM_KNOBS] = {KNOB_01, KNOB_02, KNOB_03, KNOB_04, KNOB_05, KNOB_06, KNOB_07, KNOB_08, KNOB_09, KNOB_10, KNOB_11};
const uint8_t theButtons[NUM_BUTTONS] = {BUTTON_1, BUTTON_2, BUTTON_3};
Bounce theButtonsBounce[NUM_BUTTONS] = {BUTTON_1_BOUNCE, BUTTON_2_BOUNCE, BUTTON_3_BOUNCE};
const uint8_t theToggles[NUM_TOGGLES] = {TOGGLE_1, TOGGLE_2, TOGGLE_3};
Bounce theTogglesBounce[NUM_TOGGLES] = {TOGGLE_1_BOUNCE, TOGGLE_2_BOUNCE, TOGGLE_3_BOUNCE};
const uint8_t theJacks[NUM_JACKS] = {JACKIN_1, JACKIN_2};
Bounce theJacksBounce[NUM_JACKS] = {JACKIN_1_BOUNCE, JACKIN_2_BOUNCE};
const uint8_t knobCC[NUM_KNOBS] = {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};
const uint8_t buttonCC[NUM_BUTTONS] = {23, 24, 25};
const uint8_t toggleCC[NUM_TOGGLES] = {26, 27, 28};
const uint8_t jackCC[NUM_JACKS] = {29, 30};

uint8_t knobState[NUM_KNOBS];
bool buttonState[3] = {LOW, LOW, LOW};

void setup() {
  for(int p = 0; p < TOTAL_PINS; p++){
    pinMode(p, INPUT);
  }
  for(int p = 0; p < NUM_BUTTONS; p++){
    pinMode(theButtons[p], INPUT_PULLUP);
  }
  for(int p = 0; p < NUM_TOGGLES; p++){
    pinMode(theToggles[p], INPUT_PULLUP);
  }
  for(int p = 0; p < NUM_JACKS; p++){
    pinMode(theJacks[p], INPUT_PULLUP);
  }
  for(int i = 0; i < NUM_KNOBS; i++){
    knobState[i] = (analogRead(theKnobs[i])/8);
  }
}

void loop() {
//check the knobs  
  for(int i = 0; i < NUM_KNOBS; i++){
      uint32_t knobMidi = (analogRead(theKnobs[i])/8);
    if(knobMidi != knobState[i]){
      usbMIDI.sendControlChange(knobCC[i], knobMidi, channel);
      knobState[i] = knobMidi;
    }
  }
//check the buttons
  for(int i = 0; i < NUM_BUTTONS; i++){
    theButtonsBounce[i].update();
    if(theButtonsBounce[i].fallingEdge()){
      if(buttonState[i] == LOW){
        usbMIDI.sendControlChange(buttonCC[i], 127, channel);
        buttonState[i] = HIGH;
      }
      else if(buttonState[i] == HIGH){
        usbMIDI.sendControlChange(buttonCC[i], 0, channel);
        buttonState[i] = LOW;
      }
    }
  }
//check the switches  
  for(int i = 0; i < NUM_TOGGLES; i++){
    theTogglesBounce[i].update();
    if(theTogglesBounce[i].risingEdge()){
      usbMIDI.sendControlChange(toggleCC[i], 127, channel);
    }
    if(theTogglesBounce[i].fallingEdge()){
      usbMIDI.sendControlChange(toggleCC[i], 0, channel);
    }
  }
//check the jacks
  for(int i = 0; i < NUM_JACKS; i++){
    theJacksBounce[i].update();
    if(theJacksBounce[i].fallingEdge()){
      usbMIDI.sendControlChange(jackCC[i], 127, channel);
    }
    if(theJacksBounce[i].risingEdge()){
      usbMIDI.sendControlChange(jackCC[i], 0, channel);
    }
  }
  delay(10);
}
