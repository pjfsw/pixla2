#include <stdlib.h>
#include "processor.h"

void processor_set_stage(
    ProcessorStage *stage,
    void *user_data,
    TransformFunc transformFunc,
    TriggerFunc triggerFunc,
    OffFunc offFunc
) {
    if (stage == NULL) {
        return ;
    }
    stage->userData = user_data;
    stage->transformFunc = transformFunc;
    stage->triggerFunc = triggerFunc;
    stage->offFunc = offFunc;
}
