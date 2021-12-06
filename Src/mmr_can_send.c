#include "mmr_can.h"
#include "mmr_can_util.h"


static HalStatus sendNormal(CanHandle *hcan, CanTxHeader *header, MmrCanPacket *packet);
static HalStatus sendMulti(CanHandle *hcan, CanTxHeader *header, MmrCanPacket *packet);
static HalStatus sendSingleMultiFrame(CanHandle *hcan, CanTxHeader *header, MmrCanPacket *packet);
static uint8_t computeFramesToSend(MmrCanPacket *packet);
static uint8_t computeNextMessageLength(MmrCanPacket *packet, uint8_t offset);


HalStatus MMR_CAN_Send(CanHandle *hcan, MmrCanPacket packet) {
  CanTxHeader header = {
    .IDE = CAN_ID_STD,
    .RTR = CAN_RTR_DATA,
    .DLC = packet.length,
    .StdId = packet.remoteId,
    .TransmitGlobalTime = DISABLE,
  };

  return packet.length <= MMR_CAN_MAX_DATA_LENGTH
    ? sendNormal(hcan, &header, &packet)
    : sendMulti(hcan, &header, &packet);
}


static HalStatus sendNormal(
  CanHandle *hcan,
  CanTxHeader *header,
  MmrCanPacket *packet
) {
  header->StdId |= MMR_CAN_MESSAGE_TYPE_NORMAL;
  return
    HAL_CAN_AddTxMessage(hcan, header, packet->data, packet->mailbox);
}


static HalStatus sendMulti(
  CanHandle *hcan,
  CanTxHeader *header,
  MmrCanPacket *packet
) {
  HalStatus status = HAL_OK;
  uint8_t offset = 0;
  uint8_t framesToSend = computeFramesToSend(packet);

  header->StdId |= MMR_CAN_MESSAGE_TYPE_MULTI_FRAME;
  do {
    status |=
      sendSingleMultiFrame(hcan, header, packet, &offset);
  }
  while (
    framesToSend --> 1 && status == HAL_OK
  );

  return status;
}


static HalStatus sendSingleMultiFrame(
  CanHandle *hcan,
  CanTxHeader *header,
  MmrCanPacket *packet,
  uint8_t *offset
) {
  uint8_t *dataStart = packet->data + offset;
  uint8_t length = computeNextMessageLength(packet, *offset);

  header->DLC = length;
  *offset += length;

  return HAL_CAN_AddTxMessage(hcan, header, dataStart, packet->mailbox);
}


static uint8_t computeFramesToSend(MmrCanPacket *packet) {
  uint8_t length = packet->length;
  uint8_t framesToSend = length / MMR_CAN_MAX_DATA_LENGTH;
  uint8_t maybeOneForRemainder = length % MMR_CAN_MAX_DATA_LENGTH > 0;

  return framesToSend + maybeOneForRemainder;
}

static uint8_t computeNextMessageLength(MmrCanPacket *packet, uint8_t offset) {
  return min(
    packet->length - offset, MMR_CAN_MAX_DATA_LENGTH
  );
}
