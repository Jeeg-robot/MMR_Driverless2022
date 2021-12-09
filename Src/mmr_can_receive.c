#include <stdbool.h>
#include "mmr_can.h"

static HalStatus receiveOne(CanHandle *hcan, CanRxHeader *header, uint8_t *result);
static HalStatus receiveAll(CanHandle *hcan, CanRxHeader *header, uint8_t *result);
static bool headerIsMultiFrame(CanRxHeader *header, CanId targetId);


HalStatus MMR_CAN_Receive(CanHandle *hcan, MmrCanMessage *result) {
  CanRxHeader header = {};
  uint8_t *dest = result->store;
  HalStatus status = receiveOne(hcan, &header, dest);

  result->senderId = header.ExtId;
  if (MMR_CAN_IsMultiFrame(&header)) {
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
  CanId targetId = header->ExtId;
  HalStatus status = HAL_OK;
  do {
    result += MMR_CAN_MAX_DATA_LENGTH;
    status |= receiveOne(hcan, header, result);
  } while (
    headerIsMultiFrame(header, targetId) && status == HAL_OK
  );

  return status;
}


static bool headerIsMultiFrame(CanRxHeader *header, CanId targetId) {
  return
    MMR_CAN_IsMultiFrame(header) &&
    !MMR_CAN_IsMultiFrameEnd(header) &&
    header->DLC >= MMR_CAN_MAX_DATA_LENGTH;
}
