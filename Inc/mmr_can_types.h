#ifndef INC_MMR_CAN_TYPES_H_
#define INC_MMR_CAN_TYPES_H_

#include <stdint.h>
#include "mmr_can_includes.h"

typedef uint32_t CanId;
typedef uint32_t CanMailbox;

/**
 * A filter mask for the CANbus.
 * It acts like a subnet mask, filtering the ids that
 * do not match it.
 * 
 * The value 0 can be used to let any message pass through.
 * 
 * You should use the MMR_CAN_AlignStandardMask function
 * to align the mask when using standard masks.
 */
typedef uint32_t CanFilterMask;

/**
 * Stores a value from CAN_filter_FIFO
 * That is, CAN_FILTER_FIFOx
 */
typedef uint8_t CanFilterFifo;

/** 
 * Represents a filter bank.
 * The values must be in the range [0, 27]
 */
typedef uint8_t CanFilterBank;

typedef HAL_StatusTypeDef HalStatus;
typedef CAN_HandleTypeDef CanHandle;
typedef CAN_TxHeaderTypeDef CanTxHeader;
typedef CAN_RxHeaderTypeDef CanRxHeader;

#endif /* INC_MMR_CAN_TYPES_H_ */
