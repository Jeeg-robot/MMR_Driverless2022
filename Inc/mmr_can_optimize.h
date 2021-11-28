#ifndef INC_MMR_CAN_OPTIMIZE_H_
#define INC_MMR_CAN_OPTIMIZE_H_

#ifdef __GNUC__
#define always_inline inline __attribute__((always_inline))
#else
#define always_inline inline
#endif

#endif /* INC_MMR_CAN_OPTIMIZE_H_ */
