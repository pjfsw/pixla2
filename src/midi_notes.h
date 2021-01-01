#ifndef SRC_MIDI_NOTES_H_
#define SRC_MIDI_NOTES_H_

void midi_notes_init();

double midi_get_frequency(int note);

double midi_get_note(double frequency);

#endif /* SRC_MIDI_NOTES_H_ */
