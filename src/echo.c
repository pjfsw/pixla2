#include <stdio.h>
#include "echo.h"

double echo_transform(void *user_data, double value, double delta_time) {
    //return value;
    Echo *echo = (Echo*)user_data;
    double wrapAround = echo->echo_time * ECHO_BUFFER;
    if (wrapAround < 1) {
        wrapAround = 1;
    }
    if (wrapAround > ECHO_BUFFER-1) {
        wrapAround = ECHO_BUFFER - 1;
    }
    if (wrapAround <= 0 || wrapAround >= ECHO_BUFFER) {
        printf("Echo buffer index error %f\n", wrapAround);
    }

    echo->buffer[echo->pos] = 0.5 * value + 0.5 * echo->buffer[echo->pos];
    int playPos = (echo->pos + 1) % (int)wrapAround;
    echo->pos = (echo->pos + 1) % (int)wrapAround;
    return value + 0.5 * echo->buffer[playPos];
}
