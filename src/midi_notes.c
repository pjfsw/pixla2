#include "midi_notes.h"

#define MIDI_NOTE_69_FREQ 440.0 // MIDI
#define MIDI_NOTES 128

double _midi_notes[128];

double _midi_note_to_frequency(int note) {
    return MIDI_NOTE_69_FREQ * pow(2, (note-69)/(double)12);
}

void midi_notes_init() {
    for (int i = 0; i < MIDI_NOTES; i++) {
        _midi_notes[i] = _midi_note_to_frequency(i);
    }
}

double midi_get_frequency(int note) {
    if (note < 0 || note >= MIDI_NOTES) {
        return 440;
    }
    return _midi_notes[note];
}
