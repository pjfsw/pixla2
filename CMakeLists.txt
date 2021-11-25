cmake_minimum_required(VERSION 3.10)

project(pixla3)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -pedantic -Werror)

include(FindPkgConfig)

pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

add_executable(pixla2   
    src/audio_library.c
    src/buffered_file.c
    src/combiner.c
    src/directory_tree.c
    src/echo.c
    src/filter.c
    src/font.c
    src/instrument.c
    src/lfo.c
    src/lookup_tables.c
    src/main.c
    src/midi_notes.c
    src/mixer.c
    src/modulation.c
    src/oscillator.c
    src/parameter_functions.c
    src/player.c
    src/processor.c
    src/rack.c
    src/sampler.c
    src/selection_functions.c
    src/song_exporter.c
    src/song_storage.c
    src/synth.c
    src/ui_colors.c
    src/ui_component_manager.c
    src/ui_instrument.c
    src/ui_mixer.c
    src/ui_pattern.c
    src/ui_rack.c
    src/ui_sampler.c
    src/ui_synth.c
    src/ui_track.c
    src/ui_trackpos.c
    src/ui_volume_meter.c
    src/vca.c
    src/wave_tables.c
    src/wav_saver.c
)
configure_file(src/nesfont.fon ./nesfont.fon COPYONLY)

target_link_libraries(pixla2 ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} m)

