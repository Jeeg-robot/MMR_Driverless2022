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

#define min(a, b) ((a) < (b) ? a : b);


#endif /* INC_MMR_CAN_UTIL_H_ */
