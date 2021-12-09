#include <stdbool.h>
#include "mmr_can.h"
#include "mmr_can_util.h"


static HalStatus sendNormal(CanHandle *hcan, CanTxHeader *header, MmrCanPacket *packet);
static HalStatus sendMulti(CanHandle *hcan, CanTxHeader *header, MmrCanPacket *packet);
static HalStatus sendSingleMultiFrame(CanHandle *hcan, CanTxHeader *header, MmrCanPacket *packet, uint8_t *offset);
static uint8_t computeFramesToSend(MmrCanPacket *packet);
static uint8_t computeNextMessageLength(MmrCanPacket *packet, uint8_t offset);


HalStatus MMR_CAN_Send(CanHandle *hcan, MmrCanPacket packet) {
  CanTxHeader header = {
    .IDE = CAN_ID_EXT,
    .RTR = CAN_RTR_DATA,
    .DLC = packet.length,
    .ExtId = packet.remoteId << 5,
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
  header->ExtId |= MMR_CAN_MESSAGE_TYPE_NORMAL;
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

  header->ExtId |= MMR_CAN_MESSAGE_TYPE_MULTI_FRAME;
  do {
    bool isLastFrame = framesToSend <= 1;
    if (isLastFrame) {
      header->ExtId |= MMR_CAN_MESSAGE_TYPE_MULTI_FRAME_END;
    }

    status |=
      sendSingleMultiFrame(hcan, header, packet, &offset);
  }
  while (framesToSend-- > 1 && status == HAL_OK);

  return status;
}


static HalStatus sendSingleMultiFrame(
  CanHandle *hcan,
  CanTxHeader *header,
  MmrCanPacket *packet,
  uint8_t *offset
) {
  uint8_t *dataStart = packet->data + (*offset);
  uint8_t length = computeNextMessageLength(packet, *offset);

  header->DLC = length;
  *offset += length;

  return HAL_CAN_AddTxMessage(hcan, header, dataStart, packet->mailbox);
}


/**
 * @brief 
 * Computes the frames that will need to be sent for a packet.
 * 
 * For example, if a packet has 15 bytes, the function will return
 * 2, as one packet is needed for the first 8 bytes and another is
 * needed for the remaining 7.
 * 
 * @example
 * The computation is pretty simple
 * Given
 *               length = 17 (bytes)
 * We'll have
 *         framesToSend = 17 / 8 = 2 (frames)
 *            remainder = 17 % 8 = 1 (bytes)
 * maybeOneForRemainder = 1 > 0  = 1 (frame)
 * -------------------------------------------
 *               result = 2 + 1  = 3 (frames) 
 */
static uint8_t computeFramesToSend(MmrCanPacket *packet) {
  uint8_t length = packet->length;
  uint8_t framesToSend = length / MMR_CAN_MAX_DATA_LENGTH;
  uint8_t remainder = length % MMR_CAN_MAX_DATA_LENGTH;
  uint8_t maybeOneForRemainder = remainder > 0;

  return framesToSend + maybeOneForRemainder;
}


/**
 * @brief 
 * Returns the length for the next message, either
 * 8 bytes or a lower value, keeping the offset into count.
 * 
 * @example
 * Given
 *  - a 17 bytes packet
 *  - an offset starting at 0 and supposedly incrementing
 *    of 8 after each call
 * 
 * Three subsequent calls to this function will return
 *  First call  -> min(17 - 0, 8)  = min(17, 8) = 8 (bytes)
 *  Second call -> min(17 - 8, 8)  = min(9, 8)  = 8 (bytes)
 *  Third call  -> min(17 - 16, 8) = min(1, 8)  = 1 (byte)
 */
static uint8_t computeNextMessageLength(MmrCanPacket *packet, uint8_t offset) {
  uint8_t remainingBytes = packet->length - offset;
  return min(
    remainingBytes, MMR_CAN_MAX_DATA_LENGTH
  );
}
