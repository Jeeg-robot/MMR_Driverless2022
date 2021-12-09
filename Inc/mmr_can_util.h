#ifndef INC_MMR_CAN_UTIL_H_
#define INC_MMR_CAN_UTIL_H_

#include <stdint.h>
#include <string.h>

#define sizeofarray(array) \
  (sizeof(array) / sizeof(*(array)))

#define stringArrayLength(array) \
  stringBufferLength((array), sizeofarray(array))

#define stringBufferLength(pbuffer, maxLen) \
  strnlen((const char*)(pbuffer), maxLen)

#define min(a, b) ((a) < (b) ? a : b);
#define mask(value, bits) (value & bits)


#endif /* INC_MMR_CAN_UTIL_H_ */
