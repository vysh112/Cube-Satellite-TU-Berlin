#include "Melodies.h"
#include <Arduino.h>


void play_melody(const int *melody, int array_length, int buzzer_channel){
  // calculate whole note duration
  int tempo = melody[0];
  int wholenote = (60000 * 4) / tempo;

  // iterate over the melody
  for(unsigned int note = 1; note < array_length / sizeof(int); note += 2) {
    // calculate the duration of each note
    int noteDuration = 0;
    int divider = melody[note + 1];
    if(divider > 0) {
      // regular note, just proceed
      noteDuration = wholenote / divider;
    } else if(divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = wholenote / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }
    // we only play the note for 90% of the duration, leaving 10% as a pause
    ledcWriteTone(buzzer_channel, melody[note]);
    delay(noteDuration*0.9);
    ledcWrite(buzzer_channel, 0);
    delay(noteDuration*0.1);
  }
}