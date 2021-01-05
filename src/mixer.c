#include <SDL2/SDL.h>
#include <math.h>
#include "mixer.h"
#include "synth.h"
#include "vca.h"
#include "midi_notes.h"
#include "lookup_tables.h"

void _mixer_process_buffer(Mixer *mixer, float *buffer, int number_of_floats, bool debug) {
    int tapCount = 0;
    double delta_time = 1/mixer->sample_rate;
    double max_raw = 0;
    for (int t = 0; t < number_of_floats; t+=2) {
        if (debug) {
            printf("%d: Iteration start\n", t);
        }
        if (mixer->player_delay == 0) {
            int bpm  = mixer->mixer_trigger_func(mixer->mixer_trigger_func_user_data);
            if (bpm <= 0) {
                bpm = 120;
            }
            mixer->player_delay = 0.5 * mixer->sample_rate / bpm;
        } else {
            mixer->player_delay--;
        }
        if (debug) {
            printf("%d: Trigger complete\n", t);
        }

        double sample = 0.0;
        for (int n = 0; n < mixer->number_of_instruments; n++) {
            double amp = lookup_volume(mixer->settings.instr_volume[n]) * instrument_poll(&mixer->instruments[n], delta_time);
            if (fabs(amp) > max_raw) {
                max_raw = amp;
            }
            sample += amp;
        }
        if (debug) {
            printf("%d: Instruments complete\n", t);
        }

        float adjusted_sample = (float)(lookup_volume(mixer->settings.master_volume) * sample);
        float squared_sample = adjusted_sample * adjusted_sample;
        mixer->loudness_sum += squared_sample;
        mixer->loudness_buffer[mixer->loudness_pos] = squared_sample;
        mixer->loudness_pos = (mixer->loudness_pos + 1) % LOUDNESS_BUFFER;
        mixer->loudness_sum -= mixer->loudness_buffer[mixer->loudness_pos];
        float loudness = fmax(0, fmin(1,sqrt(mixer->loudness_sum / LOUDNESS_BUFFER)));
        mixer->loudness = loudness;
        /*if (fabs(adjusted_sample) > MIXER_CLIPPING) {
            printf("Overflow %0.2f\n", adjusted_sample);
        }*/
        if (adjusted_sample > MIXER_CLIPPING) {
            adjusted_sample = MIXER_CLIPPING;
        } else if( adjusted_sample < -MIXER_CLIPPING) {
            adjusted_sample = -MIXER_CLIPPING;
        }
        if (debug) {
            printf("%d: Mixing complete\n", t);
        }
        mixer->left_tap[tapCount] = adjusted_sample;
        mixer->right_tap[tapCount] = adjusted_sample;

        buffer[t] = adjusted_sample;
        buffer[t+1] = adjusted_sample;
        if (debug) {
            printf("%d: Iteration complete\n", t);
        }

        //mixer->lr_delay[mixer->delay_pos] = adjusted_sample;
        //mixer->delay_pos = (mixer->delay_pos + 1) % LR_DELAY;
        //buffer[t+1] = mixer->lr_delay[mixer->delay_pos];


        tapCount++;
    }
//    printf("Fabs(v) %f %f\n", max_raw, max_vol);
}
void mixer_process_buffer(Mixer *mixer, float *buffer, int number_of_floats) {
    _mixer_process_buffer(mixer, buffer, number_of_floats, false);
}

void _mixer_callback(void *user_data, Uint8 *stream, int len) {
    // valueDBFS = 20*log10(abs(value))
    Mixer *mixer = (Mixer*)user_data;
    float *buffer = (float*)stream;
    _mixer_process_buffer(mixer, buffer, len/sizeof(float), false);
}

void mixer_start(Mixer *mixer) {
    SDL_PauseAudioDevice(mixer->device, 0);
}

void mixer_stop(Mixer *mixer) {
    SDL_PauseAudioDevice(mixer->device, 1);
}

Mixer *mixer_create(Instrument *instruments, int number_of_instruments,
    MixerTriggerFunc mixer_trigger_func,  void *mixer_trigger_func_user_data,
    bool attach_audio_device) {
    Mixer *mixer = calloc(1, sizeof(Mixer));
    mixer->mixer_trigger_func = mixer_trigger_func;
    mixer->mixer_trigger_func_user_data = mixer_trigger_func_user_data;
    mixer->instruments = instruments;
    mixer->number_of_instruments = number_of_instruments;
    mixer->settings.master_volume = lookup_volume_minus_6dbfs();
    mixer->tap_size = MIXER_DEFAULT_BUFFER_SIZE;
    mixer->left_tap = calloc(mixer->tap_size, sizeof(float));
    mixer->right_tap = calloc(mixer->tap_size, sizeof(float));

    if (attach_audio_device) {
        SDL_InitSubSystem(SDL_INIT_AUDIO);

        SDL_AudioSpec want;
        SDL_AudioSpec have;

        SDL_memset(&want, 0, sizeof(want));
        want.freq = MIXER_DEFAULT_SAMPLE_RATE;
        want.format = AUDIO_F32;
        want.channels = 2;
        want.samples = MIXER_DEFAULT_BUFFER_SIZE;
        want.callback = _mixer_callback;
        want.userdata = mixer;

        mixer->device = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
        if (!mixer->device) {
            SDL_Log("Failed to open audio: %s", SDL_GetError());
            mixer_destroy(mixer);
            return NULL;
        }
        mixer->sample_rate = have.freq;
        printf("Sample rate %f Hz\n", mixer->sample_rate);
    } else {
        mixer->device = 0;
        mixer->sample_rate = MIXER_DEFAULT_SAMPLE_RATE;
    }

    for (int i = 0; i < number_of_instruments; i++) {
        mixer->settings.instr_volume[i] = lookup_volume_minus_6dbfs();
    }
    return mixer;
}

void mixer_destroy(Mixer *mixer) {
    if (mixer != NULL) {
        if (mixer->device != 0) {
            printf("Close audio\n");
            SDL_CloseAudioDevice(mixer->device);
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
        }
        free(mixer->left_tap);
        free(mixer->right_tap);
        free(mixer);
    }
}
