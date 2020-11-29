#ifndef SRC_ECHO_H_
#define SRC_ECHO_H_

#define ECHO_BUFFER 96000

typedef struct {
    double echo_time;
    double buffer[ECHO_BUFFER];
    int pos;
} Echo;

double echo_transform(void *user_data, double value, double delta_time);

#endif /* SRC_ECHO_H_ */
