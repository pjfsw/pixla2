#ifndef SRC_VCA_H_
#define SRC_VCA_H_

#include <stdbool.h>

typedef enum {
    VCA_OFF=0,
    VCA_ATTACK=1,
    VCA_DECAY=2,
    VCA_SUSTAIN=3,
    VCA_RELEASE=4
} VcaState;

typedef struct {
    double attack;
    double decay;
    double sustain;
    double release;
    int inverse;
} VcaSettings;

typedef struct {
    double attack;
    double attack_speed;
    double decay;
    double sustain;
    double release;
    int inverse;
    VcaState state;
    VcaSettings *settings;
    double t;
    double amp;
} Vca;

void vca_set_inverse(Vca *vca, bool inverse);

// Instance functions
void vca_trigger(void *user_data, double frequency);

void vca_off(void *user_data);

double vca_transform(void *user_data, double signal, double delta_time);

#endif /* SRC_VCA_H_ */
