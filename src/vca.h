#ifndef SRC_VCA_H_
#define SRC_VCA_H_

#include <stdbool.h>

typedef enum {
    VCA_OFF=0,
    VCA_ACTIVE=1,
    VCA_RELEASE=2
} VcaState;

typedef struct {
    VcaState state;
    double t;
} Vca;

void vca_reset(Vca *vca);

void vca_trigger(void *user_data);

void vca_off(void *user_data);

double vca_transform(void *user_data, double signal, double delta_time);

#endif /* SRC_VCA_H_ */
