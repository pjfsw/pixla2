#ifndef SRC_RACK_H_
#define SRC_RACK_H_

#include "mixer.h"
#include "synth.h"
#include "sampler.h"
#include "audio_library.h"
#include "tracker_limits.h"

typedef struct {
    Instrument instruments[NUMBER_OF_INSTRUMENTS];
    Mixer *mixer;
    AudioLibrary *audio_library;
} Rack;

Rack *rack_create(MixerTriggerFunc trigger_func, void *trigger_func_user_data,
    SynthSettings *synth_settings, MixerSettings *mixer_settings);

void rack_all_off(Rack *rack);

void rack_destroy(Rack *rack);

#endif /* SRC_RACK_H_ */
