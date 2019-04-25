
/* This file was automatically generated by nrfutil on 2018-04-29 (YY-MM-DD) at 19:13:56 */

#include "sdk_config.h"
#include "stdint.h"
#include "compiler_abstraction.h"

#if NRF_CRYPTO_BACKEND_OBERON_ENABLED
/* Oberon backend is changing endianness thus public key must be kept in RAM. */
#define _PK_CONST
#else
#define _PK_CONST const
#endif


/** @brief Public key used to verify DFU images */
__ALIGN(4) _PK_CONST uint8_t pk[64] =
{
    0xe6, 0x77, 0x9d, 0x30, 0xf2, 0xe0, 0x8d, 0x5f, 0x21, 0x4c, 0x21, 0x51, 0xef, 0xca, 0xda, 0x72, 0x83, 0x9a, 0x29, 0x7a, 0x97, 0x11, 0x8e, 0xca, 0x5b, 0x65, 0x1b, 0xee, 0x58, 0xf2, 0x88, 0xd9, 
    0x34, 0x34, 0xaf, 0x66, 0x62, 0x40, 0x5a, 0x5a, 0xbf, 0xfc, 0xb2, 0x67, 0x1b, 0xaa, 0xd3, 0x85, 0x07, 0x8f, 0x6e, 0x99, 0xab, 0x7a, 0x58, 0x6c, 0x29, 0x2e, 0x07, 0x41, 0x30, 0x21, 0x12, 0x5c
};
