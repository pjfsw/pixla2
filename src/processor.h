#ifndef SRC_PROCESSOR_H_
#define SRC_PROCESSOR_H_

#include <SDL2/SDL.h>

typedef double (*TransformFunc)(void *user_data, double signal, double delta_time);
typedef void (*TriggerFunc)(void *user_data, double frequency, Uint8 velocity);
typedef void (*OffFunc)(void *user_data);

typedef struct {
    TransformFunc transformFunc;
    TriggerFunc triggerFunc;
    OffFunc offFunc;
    void *userData;
} ProcessorStage;

typedef struct {
    ProcessorStage *stages;
    int number_of_stages;
} Processor;

void processor_set_stage(
    ProcessorStage *stage,
    void *user_data,
    TransformFunc transformFunc,
    TriggerFunc triggerFunc,
    OffFunc offFunc
);

#endif /* SRC_PROCESSOR_H_ */
