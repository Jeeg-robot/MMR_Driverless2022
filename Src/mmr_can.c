#include "mmr_can.h"

HalStatus MMR_CAN_BasicSetupAndStart(CanHandle *hcan) {
  return
    MMR_CAN_FilterConfigDefault(hcan) |
    HAL_CAN_Start(hcan)
    ;
}

HalStatus MMR_CAN_FilterConfig(CanHandle *hcan, MmrCanFilterSettings settings) {
  CAN_FilterTypeDef filter = {
    .FilterActivation = settings.enabled
      ? CAN_FILTER_ENABLE
      : CAN_FILTER_DISABLE,

    .FilterIdHigh = settings.idMask,
    .FilterMaskIdHigh = settings.idMask,
    .FilterFIFOAssignment = settings.fifo,
    .FilterBank = settings.bank,
    .SlaveStartFilterBank = settings.slaveBankStart,
    .FilterMode = CAN_FILTERMODE_IDMASK,
    .FilterScale = CAN_FILTERSCALE_32BIT,
  };

  return HAL_CAN_ConfigFilter(hcan, &filter);
}

CanFilterMask MMR_CAN_AlignStandardMask(CanFilterMask baseMask) {
  static const uint8_t extendedMaskSurplusBytes = 5;
  return baseMask << extendedMaskSurplusBytes;
}

MmrCanFilterSettings MMR_CAN_GetDefaultFilterSettings() {
  return (MmrCanFilterSettings) {
    .enabled = true,
    .fifo = MMR_CAN_FILTER_FIFO,
    .idMask = 0,
    .bank = 0,
    .slaveBankStart = 14,
  };
}
