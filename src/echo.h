#ifndef SRC_ECHO_H_
#define SRC_ECHO_H_

#define ECHO_BUFFER 48000
#define ECHO_TIME 0.4

typedef struct {
    double buffer[ECHO_BUFFER];
    int pos;
} Echo;

double echo_transform(void *user_data, double value, double delta_time);

#endif /* SRC_ECHO_H_ */
