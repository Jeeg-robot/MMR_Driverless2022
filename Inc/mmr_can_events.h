#ifndef INC_MMR_CAN_EVENTS_H_
#define INC_MMR_CAN_EVENTS_H_

#include "mmr_can.h"

typedef struct {
  CanId senderId;
  CanRxBuffer message;
} MmrCanEvent;

typedef void (*MmrCanEventHandler)(MmrCanEvent *event);

typedef struct {
  MmrCanEventHandler *events;
  size_t count;
} MmrCanEventList;


void MMR_CAN_InitRxHandlers(MmrCanEventList *rxHandlers);


#endif /* INC_MMR_CAN_EVENTS_H_ */
