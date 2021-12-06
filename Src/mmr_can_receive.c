#include <stdbool.h>
#include "mmr_can.h"

static HalStatus receiveOne(CanHandle *hcan, CanRxHeader *header, uint8_t *result);
static HalStatus receiveAll(CanHandle *hcan, CanRxHeader *header, uint8_t *result);
static bool headerIsMultiFrame(CanRxHeader *header, CanId targetId);


HalStatus MMR_CAN_Receive(CanHandle *hcan, MmrCanMessage *result) {
  CanRxHeader header = {};
  uint8_t *dest = result->store;
  HalStatus status = receiveOne(hcan, &header, dest);

  result->senderId = header.StdId;
  if (MMR_CAN_IsMultiFrame(header)) {
    status |= receiveAll(hcan, &header, dest);
  }

  return status;
}


static HalStatus receiveOne(
  CanHandle *hcan,
  CanRxHeader *header,
  uint8_t *result
) {
  return HAL_CAN_GetRxMessage(hcan, MMR_CAN_RX_FIFO, header, result);
}

static HalStatus receiveAll(
  CanHandle *hcan,
  CanRxHeader *header,
  uint8_t *result
) {
  CanId targetId = header->StdId;
  HalStatus status = HAL_OK;
  do {
    result += MMR_CAN_MAX_DATA_LENGTH;
    status |= receiveOne(hcan, header, result);
  } while (
     status == HAL_OK && headerIsMultiFrame(header, targetId)
  );

  return status;
}


static bool headerIsMultiFrame(CanRxHeader *header, CanId targetId) {
  return
    header->DLC >= MMR_CAN_MAX_DATA_LENGTH &&
    header->StdId & MMR_CAN_MESSAGE_TYPE_MULTI_FRAME &&
    header->StdId == targetId;
}
