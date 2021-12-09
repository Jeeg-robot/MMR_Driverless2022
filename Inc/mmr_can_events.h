#ifndef INC_MMR_CAN_EVENTS_H_
#define INC_MMR_CAN_EVENTS_H_

#include "mmr_can.h"
#include "mmr_can_util.h"

typedef void (*MmrCanEventHandler)(const MmrCanMessage *event);

typedef struct {
  const MmrCanEventHandler *handlers;
  const size_t count;
} MmrCanEventList;

#define MMR_CAN_CreateEventList(handlers) \
  (const MmrCanEventList) { handlers, sizeofarray(handlers) }

HalStatus MMR_CAN_InitRxHandlers(CanHandle *hcan, const MmrCanEventList *rxEvents);


#endif /* INC_MMR_CAN_EVENTS_H_ */
