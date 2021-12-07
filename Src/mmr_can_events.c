#include "mmr_can.h"
#include "mmr_can_events.h"


static const MmrCanEventList *_rxEvents = NULL;


static void __handleCanRxInterrupt(CanHandle *hcan);
static void __invokeAll(const MmrCanEventList *events, const MmrCanMessage *event);
static void __maybeInvoke(const MmrCanEventHandler handler, const MmrCanMessage *event);


HalStatus MMR_CAN_InitRxHandlers(CanHandle *hcan, const MmrCanEventList *rxEvents) {
  _rxEvents = rxEvents;
  return HAL_CAN_ActivateNotification(hcan, MMR_CAN_RX_INTERRUPT);
}


static void __handleCanRxInterrupt(CanHandle *hcan) {
  static CanRxBuffer buffer = {};
  static MmrCanMessage event = {
    .store = buffer,
  };

  MMR_CAN_Receive(hcan, &event);
  __invokeAll(_rxEvents, &event);
}

static void __invokeAll(
  const MmrCanEventList *events,
  const MmrCanMessage *event
) {
  if (!events) {
    return;
  }

  short i;
  for (i = 0; i < events->count; i++) {
    __maybeInvoke(events->handlers[i], event);
  }
}

static always_inline void __maybeInvoke(
  const MmrCanEventHandler handler,
  const MmrCanMessage *event
) {
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
