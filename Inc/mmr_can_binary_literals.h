#ifndef INC_MMR_CAN_BINARY_LITERALS_H_
#define INC_MMR_CAN_BINARY_LITERALS_H_

/**
 * @brief
 * These set of macros (B_, B8_, B16_, B32_) allow one
 * to convert bit literals to their hexadecimal representation
 *
 * They are useful when it is needed to convey exactly which bits
 * are being set for a particular register, message etc...
 *  
 * @example
 * B_(1010) = 0xA
 * B8_(1111, 1111) = 0xFF
 * 
 * -----------------------------------------------------------
 * Macros taken from https://stackoverflow.com/a/7577517 and
 * sligthly readapted
 */

#define __B_0000 0
#define __B_0001 1
#define __B_0010 2
#define __B_0011 3
#define __B_0100 4
#define __B_0101 5
#define __B_0110 6
#define __B_0111 7
#define __B_1000 8
#define __B_1001 9
#define __B_1010 a
#define __B_1011 b
#define __B_1100 c
#define __B_1101 d
#define __B_1110 e
#define __B_1111 f

#define __B2H(bits) __B_ ## bits
#define _B2H(bits) __B2H(bits)
#define __HEX(n) 0x ## n
#define _HEX(n) __HEX(n)
#define __CCAT(a,b) a ## b
#define _CCAT(a,b) __CCAT(a,b)

#define B_(a) _HEX(_B2H(a))
#define B8_(a, b) _HEX(_CCAT(_B2H(a), _B2H(b)))
#define B16_(a, b, c, d) \
  _HEX(_CCAT( \
    _CCAT(_B2H(a), _B2H(b)), \
    _CCAT(_B2H(c), _B2H(d))  \
  ))

#define B32_(a, b, c, d, e, f, g, h) \
  _HEX(_CCAT( \
    _CCAT( \
      _CCAT(_B2H(a), _B2H(b)), \
      _CCAT(_B2H(c),_B2H(d))), \
    _CCAT( \
      _CCAT(_B2H(e), _B2H(f)), \
      _CCAT(_B2H(g), _B2H(h))  \
    ) \
  ))

#endif /* INC_MMR_CAN_BINARY_LITERALS_H_ */
