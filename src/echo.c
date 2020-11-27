#include "echo.h"

double echo_transform(void *user_data, double value, double delta_time) {
    //return value;
    Echo *echo = (Echo*)user_data;
    double oneSecondInSamples = 1/delta_time;
    double wrapAround = ECHO_TIME * oneSecondInSamples;

    echo->buffer[echo->pos] = 0.5 * value + 0.5 * echo->buffer[echo->pos];
    int playPos = (echo->pos + 1) % (int)wrapAround;
    echo->pos = (echo->pos + 1) % (int)wrapAround;
    return 0.6 * value + 0.4 * echo->buffer[playPos];
}
