#include "mmr_can.h"
#include "mmr_can_events.h"


static MmrCanEventList *_rxHandlers = NULL;
static MmrCanEventList *_txHandlers = NULL;


static void __handleCanRxInterrupt(CanHandle *hcan);
static void __invokeAll(MmrCanEventList *list, MmrCanEvent *event);
static void __maybeInvoke(MmrCanEventHandler handler, MmrCanEvent *event);


void MMR_CAN_InitRxHandlers(MmrCanEventList *rxHandlers) {
  _rxHandlers = rxHandlers;
}


static void __handleCanRxInterrupt(CanHandle *hcan) {
  static CanRxHeader rxHeader = {};
  static CanRxBuffer rxData = {};

  HAL_CAN_GetRxMessage(hcan, MMR_CAN_RX_FIFO, &rxHeader, rxData);

  MmrCanEvent event = {
    .senderId = 0,
    .message = message,
  };

  __invokeAll(_rxHandlers, &event);
}

static void __invokeAll(MmrCanEventList *list, MmrCanEvent *event) {
  if (!list) {
    return;
  }

  short i;
  for (i = 0; i < list->count; i++) {
    __maybeInvoke(list->events[i], event);
  }
}

static always_inline void __maybeInvoke(MmrCanEventHandler handler, MmrCanEvent *event) {
  if (handler) {
    handler(event);
  }
}


#if MMR_CAN_RX_FIFO == CAN_RX_FIFO0
void HAL_CAN_RxFifo0MsgPendingCallback(CanHandle *hcan) {
  __handleCanRxInterrupt(hcan);
}
#elif MMR_CAN_RX_FIFO == CAN_RX_FIFO1
void HAL_CAN_RxFifo1MsgPendingCallback(CanHandle *hcan) {
  __handleCanRxInterrupt(hcan);
}
#endif
