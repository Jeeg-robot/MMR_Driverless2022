#ifndef INC_MMR_CAN_H_
#define INC_MMR_CAN_H_

#include <stdint.h>
#include <stdbool.h>
#include "mmr_can_includes.h"
#include "mmr_can_types.h"
#include "mmr_can_optimize.h"
#include "mmr_can_binary_literals.h"

#ifndef MMR_CAN_RX_FIFO
#define MMR_CAN_RX_FIFO CAN_RX_FIFO0
#endif

#if MMR_CAN_RX_FIFO == CAN_RX_FIFO0
#define MMR_CAN_FILTER_FIFO CAN_FILTER_FIFO0
#else
#define MMR_CAN_FILTER_FIFO CAN_FILTER_FIFO1
#endif

#if MMR_CAN_RX_FIFO == CAN_RX_FIFO0
#define MMR_CAN_RX_INTERRUPT CAN_IT_RX_FIFO0_MSG_PENDING
#else
#define MMR_CAN_RX_INTERRUPT CAN_IT_RX_FIFO1_MSG_PENDING
#endif

#ifndef MMR_CAN_MAX_DATA_LENGTH
#define MMR_CAN_MAX_DATA_LENGTH 8
#endif

typedef uint8_t CanRxBuffer[MMR_CAN_MAX_DATA_LENGTH];


typedef struct {
  bool enabled;

  /**
   * The fifo that the filter should operate
   * on
   */
  CanFilterFifo fifo;

  /**
   * The filter bank to use
   *
   * In systems with more than one CAN interface,
   * this must go from 0 to slaveBankStart for the master CAN
   * and from slaveBankStart to 27 for the slave CAN
   * 
   * Master CAN is usually CAN1, while slave CAN is CAN2
   */
  CanFilterBank bank;

  /**
   * The start of the slave filter banks.
   * This value is ignored for single-CAN boards.
   */
  CanFilterBank slaveBankStart;

  /**
   * The mask to use with this filter.
   */
  CanFilterMask idMask;
} MmrCanFilterSettings;


typedef struct {
  CanId remoteId;
  CanMailbox *mailbox;
  uint8_t *data;
  uint8_t length;
} MmrCanPacket;


/**
 * @brief
 * These values can be appended to the extended-id
 * portion of the CAN bus message (that is, the lower 5 bits
 * of the standard id)
 * 
 * They are used to check if a message is either standalone
 * or split into multiple frames
 * 
 * Multi-frame messages have a higher priority over normal ones
 */
typedef enum {
  MMR_CAN_MESSAGE_TYPE_MULTI_FRAME = B_(0010),
  MMR_CAN_MESSAGE_TYPE_MULTI_FRAME_END = B_(0011),
  MMR_CAN_MESSAGE_TYPE_NORMAL = B_(1000),
} MmrCanMessageType;


/**
 * @brief
 * Represents a CAN message
 */
typedef struct {
  CanId senderId;
  void *store;
} MmrCanMessage;


#define MMR_CAN_FilterConfigDefault(phcan) \
  MMR_CAN_FilterConfig(phcan, MMR_CAN_GetDefaultFilterSettings())


HalStatus MMR_CAN_BasicSetupAndStart(CanHandle *hcan);
HalStatus MMR_CAN_FilterConfig(CanHandle *hcan, MmrCanFilterSettings settings);
CanFilterMask MMR_CAN_AlignStandardMask(CanFilterMask baseMask);
MmrCanFilterSettings MMR_CAN_GetDefaultFilterSettings();

HalStatus MMR_CAN_Send(CanHandle *hcan, MmrCanPacket packet);
HalStatus MMR_CAN_Receive(CanHandle *hcan, MmrCanMessage *result);


bool MMR_CAN_IsMultiFrame(CanRxHeader *header);
bool MMR_CAN_IsMultiFrameEnd(CanRxHeader *header);

#endif /* INC_MMR_CAN_H_ */
