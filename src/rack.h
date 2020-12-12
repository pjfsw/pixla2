#ifndef SRC_RACK_H_
#define SRC_RACK_H_

#include "synth.h"
#include "mixer.h"

#define NUMBER_OF_INSTRUMENTS 8

typedef struct {
    Instrument instruments[NUMBER_OF_INSTRUMENTS];
    Mixer *mixer;
} Rack;

Rack *rack_create();

void rack_destroy(Rack *rack);

#endif /* SRC_RACK_H_ */
