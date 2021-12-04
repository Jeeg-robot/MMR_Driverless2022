#ifndef INC_MMR_CAN_UTIL_H_
#define INC_MMR_CAN_UTIL_H_

#include <stdint.h>
#include <string.h>

#define arrayLength(array) \
  (sizeof(array) / sizeof(*(array)))

#define stringArrayLength(array) \
  stringBufferLength((array), arrayLength(array))

#define stringBufferLength(pbuffer, maxLen) \
  strnlen((const char*)(pbuffer), maxLen)

#endif /* INC_MMR_CAN_UTIL_H_ */
