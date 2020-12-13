#include <stdio.h>
#include "echo.h"
#include "lookup_tables.h"

double echo_transform(void *user_data, double value, double delta_time) {
    //return value;
    Echo *echo = (Echo*)user_data;
    double wrapAround = lookup_echo_time(echo->echo_time);
    if (wrapAround < 1) {
        wrapAround = 1;
    }
    if (wrapAround > ECHO_BUFFER-1) {
        wrapAround = ECHO_BUFFER - 1;
    }
    if (wrapAround <= 0 || wrapAround >= ECHO_BUFFER) {
        printf("Echo buffer index error %f\n", wrapAround);
    }

    double fb =  lookup_echo_feedback(echo->feedback);
    double wet = lookup_echo_wetness(echo->wetness);
    echo->buffer[echo->pos] = 1.0 * value + fb * echo->buffer[echo->pos];
    int playPos = (echo->pos + 1) % (int)wrapAround;
    echo->pos = (echo->pos + 1) % (int)wrapAround;
    double dry = 1.0 - wet;
    return dry*value + wet * echo->buffer[playPos];
}
