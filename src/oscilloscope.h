#ifndef SRC_OSCILLOSCOPE_H_
#define SRC_OSCILLOSCOPE_H_

#define OSCILLOSCOPE_SIZE 256

typedef struct {
    double buffer[OSCILLOSCOPE_SIZE];
    double trigger_time;
} Oscilloscope;

void oscilloscope_sync(Oscilloscope *oscilloscope, double t);

#endif /* SRC_OSCILLOSCOPE_H_ */
