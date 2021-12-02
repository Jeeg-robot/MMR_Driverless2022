#ifndef INC_MMR_CAN_EVENTS_H_
#define INC_MMR_CAN_EVENTS_H_

#include "mmr_can.h"

<<<<<<< HEAD
typedef struct {
  CanId senderId;
  uint8_t *message;
} MmrCanEvent;

typedef void (*MmrCanEventHandler)(MmrCanEvent *event);
=======
typedef void (*MmrCanEventHandler)(const MmrCanMessage *event);
>>>>>>> 458fec2ee75d141c6291b2b819f6a4448af688fb

typedef struct {
  const MmrCanEventHandler *handlers;
  const size_t count;
} MmrCanEventList;


void MMR_CAN_InitRxHandlers(const MmrCanEventList *rxEvents);


#endif /* INC_MMR_CAN_EVENTS_H_ */
