#ifndef SRC_ECHO_H_
#define SRC_ECHO_H_

#include <SDL2/SDL.h>

#define ECHO_BUFFER 96000

typedef struct {
    Uint8 wetness;
    Uint8 feedback;
    Uint8 echo_time;
    double buffer[ECHO_BUFFER];
    int pos;
} Echo;

double echo_transform(void *user_data, double value, double delta_time);

#endif /* SRC_ECHO_H_ */
