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
} Instrument;

void instrument_note_on(Instrument *instrument, int note, int velocity);

void instrument_note_off(Instrument *instrument, int note);

void instrument_off(Instrument *instrument);

double instrument_poll(Instrument *instrument, double delta_time);

double instrument_get_master_level(Instrument *instrument);

#endif /* SRC_INSTRUMENT_H_ */
