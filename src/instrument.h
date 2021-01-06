#ifndef SRC_INSTRUMENT_H_
#define SRC_INSTRUMENT_H_

#include "synth.h"
#include "sampler.h"

typedef enum {
    INSTR_SYNTH,
    INSTR_SAMPLER
} InstrumentType;

typedef struct {
    Synth *synth;
    Sampler *sampler;
    InstrumentType type;
    Uint8 volume;
} Instrument;

// Triggers note on, returns the voice id of the triggered note
int instrument_note_on(Instrument *instrument, int note, int velocity);

// Trigger voice off
void instrument_note_off(Instrument *instrument, int voice_id);

void instrument_off(Instrument *instrument);

void instrument_pitch_offset(Instrument *instrument, int voice_id, double pitch_offset);

double instrument_poll(Instrument *instrument, double delta_time);

double instrument_get_master_level(Instrument *instrument);

// Tracker controlled volume
void instrument_set_volume(Instrument *instrument, Uint8 volume);

#endif /* SRC_INSTRUMENT_H_ */
