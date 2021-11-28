#ifndef INC_MMR_CAN_UTIL_H_
#define INC_MMR_CAN_UTIL_H_

#include <stdint.h>
#include <string.h>

#define stringArrayLength(array) \
  stringBufferLength((array), sizeof(array))

size_t stringBufferLength(uint8_t *buffer, size_t maxLen) {
  return strnlen((const char*)buffer, maxLen);
}

#endif /* INC_UTIL_H_ */
