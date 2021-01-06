#include <stdio.h>
#include <stdbool.h>
#include "song_exporter.h"
#include "wav_saver.h"

#include "player.h"

void song_exporter_export(char *filename, Song *song, Mixer *source_mixer, AudioLibrary *audio_library) {
    Player player;
    memset(&player, 0, sizeof(Player));
    player.song = song;

    Instrument instruments[NUMBER_OF_INSTRUMENTS];
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        instruments[i].type = source_mixer->instruments[i].type;
        instruments[i].sampler = sampler_create(audio_library);
        instruments[i].synth = synth_create(&song->synth_settings[i]);
    }
    player.instruments = instruments;

    Mixer *mixer = mixer_create(source_mixer->settings,
        instruments, NUMBER_OF_INSTRUMENTS,
        player_trigger, &player, false);

    player_start(&player);
    WavSaver *wav_saver = wav_saver_init("output.wav", MIXER_DEFAULT_SAMPLE_RATE, 2);
    if (wav_saver == NULL) {
        printf("Failed to export song\n");
    }

    int buffer_size = 32768;
    float render_buffer[2];
    Sint16 *wav_buffer = calloc(1, sizeof(Sint16)*buffer_size);
    int count = 0;
    count = 0;
    int sample = 0;

    player_start(&player);

    wav_buffer[0] = 0;
    while (!player.song_ended) {
        mixer_process_buffer(mixer, render_buffer, 2);
        if (fabs(render_buffer[0]) > 1.0 || fabs(render_buffer[1]) > 1.0) {
            fprintf(stderr, "Audio sample %d clipping (L: %.1f, R:%.1f)!\n", sample, render_buffer[0], render_buffer[1]);
        }
        sample++;
        Sint16 left = render_buffer[0] * 32767.0;
        Sint16 right = render_buffer[1] * 32767.0;

        wav_buffer[count] = left;
        wav_buffer[count+1] = right;
        count+=2;
        if (count >= buffer_size) {
            wav_saver_consume(wav_saver, wav_buffer, count);
            count = 0;
        }
    }
    if (count > 0) {
        wav_saver_consume(wav_saver, wav_buffer, count);
    }

    free(wav_buffer);
    wav_saver_close(wav_saver);
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        synth_destroy(instruments[i].synth);
        sampler_destroy(instruments[i].sampler);
    }
    mixer_destroy(mixer);
    printf("Export to %s complete!\n", filename);
}
