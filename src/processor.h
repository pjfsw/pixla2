#ifndef SRC_PROCESSOR_H_
#define SRC_PROCESSOR_H_

typedef double (*TransformFunc)(void *user_data, double signal, double delta_time);
typedef void (*TriggerFunc)(void *user_data, double frequency);
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

#endif /* SRC_PROCESSOR_H_ */
