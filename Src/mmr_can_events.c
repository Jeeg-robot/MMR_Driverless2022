#include "mmr_can.h"
#include "mmr_can_events.h"


static const MmrCanEventList _rxEvents = {};


static void __handleCanRxInterrupt(CanHandle *hcan);
static void __invokeAll(const MmrCanEventList *events, MmrCanEvent *event);
static void __maybeInvoke(const MmrCanEventHandler handler, MmrCanEvent *event);


void MMR_CAN_InitRxHandlers(const MmrCanEventHandler *rxHandlers, size_t count) {
  _rxEvents.handlers = rxHandlers;
  _rxEvents.count = count;
}


static void __handleCanRxInterrupt(CanHandle *hcan) {
  static CanRxHeader rxHeader = {};
  static CanRxBuffer rxData = {};

  HAL_CAN_GetRxMessage(hcan, MMR_CAN_RX_FIFO, &rxHeader, rxData);

  MmrCanEvent event = {
    .senderId = rxHeader->StdId,
    .message = rxData,
  };

  __invokeAll(&_rxEvents, &event);
}

static void __invokeAll(const MmrCanEventList *events, MmrCanEvent *event) {
  if (!events) {
    return;
  }

  short i;
  for (i = 0; i < events->count; i++) {
    __maybeInvoke(events->handlers[i], event);
  }
}

static always_inline void __maybeInvoke(const MmrCanEventHandler handler, MmrCanEvent *event) {
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
