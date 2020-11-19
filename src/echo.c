#include "echo.h"

double echo_transform(void *user_data, double value, double delta_time) {
    Echo *echo = (Echo*)user_data;
    echo->buffer[echo->pos] = 0.5 * value + 0.5 * echo->buffer[echo->pos];
    int offset = ECHO_TIME/delta_time;
    int playPos = echo->pos - offset;
    if (playPos < 0) {
        playPos += ECHO_BUFFER;
    }
    echo->pos = (echo->pos + 1) % ECHO_BUFFER;
    return 0.7 * value + 0.3 * echo->buffer[playPos];
}
