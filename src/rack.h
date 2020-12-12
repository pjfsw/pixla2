#ifndef SRC_RACK_H_
#define SRC_RACK_H_

#include "synth.h"
#include "sampler.h"
#include "mixer.h"
#include "audio_library.h"
#define NUMBER_OF_INSTRUMENTS 8

typedef struct {
    Instrument instruments[NUMBER_OF_INSTRUMENTS];
    Mixer *mixer;
    AudioLibrary *audio_library;
} Rack;

Rack *rack_create();

void rack_all_off(Rack *rack);

void rack_destroy(Rack *rack);

#endif /* SRC_RACK_H_ */
