#ifndef INC_MMR_CAN_EVENTS_H_
#define INC_MMR_CAN_EVENTS_H_

#include "mmr_can.h"

typedef void (*MmrCanEventHandler)(const MmrCanMessage *event);

typedef struct {
  const MmrCanEventHandler *handlers;
  const size_t count;
} MmrCanEventList;


void MMR_CAN_InitRxHandlers(const MmrCanEventList *rxEvents);


#endif /* INC_MMR_CAN_EVENTS_H_ */
