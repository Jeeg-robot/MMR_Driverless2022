#ifndef INC_MMR_CAN_EVENTS_H_
#define INC_MMR_CAN_EVENTS_H_

#include "mmr_can.h"

typedef struct {
  CanId senderId;
  uint8_t *message;
} MmrCanEvent;

typedef void (*MmrCanEventHandler)(MmrCanEvent *event);

typedef struct {
  const MmrCanEventHandler *handlers;
  const size_t count;
} MmrCanEventList;


void MMR_CAN_InitRxHandlers(
  const MmrCanEventHandler *rxHandlers,
  size_t count
);


#endif /* INC_MMR_CAN_EVENTS_H_ */
