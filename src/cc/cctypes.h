/** \file cctypes.h
 ** \brief C-Compiler type definition
 ** \author S.Rossi
 **
 **  Version Control Informations:
 **     $Revision: 1996 $
 **     $LastChangedDate: 2016-05-13 08:56:07 +0200 (ven, 13 mag 2016) $
 **     $LastChangedBy: Luca.Vergani $
 **/

#ifndef __CCTYPES_H
#define __CCTYPES_H

#include <stdint.h>
#include <stdio.h>
#include "ccport.h"

#ifndef CHAR
    #define CHAR char
#endif

#ifndef INT8
    #define INT8 int8_t
#endif

#ifndef UINT8
    #define UINT8 uint8_t
#endif

#ifndef BYTE
    #define BYTE uint8_t
#endif

#define U8_LIM 0xFF


#ifndef INT16
    #define INT16 int16_t
#endif

#ifndef UINT16
    #define UINT16 uint16_t
#endif

#ifndef WORD
    #define WORD uint16_t
#endif

#define U16_LIM 0xFFFF

#ifndef INT32
    #define INT32 int32_t
#endif

#ifndef UINT32
    #define UINT32 uint32_t
#endif

#ifndef DWORD
    #define DWORD uint32_t
#endif

#define U32_LIM 0xFFFFFFFF

#ifndef INT64
    #define INT64 int64_t
#endif

#ifndef UINT64
    #define UINT64 uint64_t
#endif

#ifndef QWORD
    #define QWORD uint64_t
#endif

#define U64_LIM ((uint64_t)0xFFFFFFFFFFFFFFFF)

typedef UINT32 SIZE_t;

#ifndef REAL32
    #define REAL32 float
#endif

#ifndef REAL64
    #define REAL64 double
#endif


#ifndef BOOL
    #define BOOL UINT8
#endif

#ifndef FALSE
    #define FALSE 0x00
#endif

#ifndef TRUE
    #define TRUE  0x01
#endif

typedef BYTE* ADDR_t;

typedef void (*CallBack_t)(void*);

#ifndef NULL
    #define NULL ((void*)0)
#endif

/* Endianness swap utils */
#define bswap16(x) \
( \
	((UINT16)( \
			(((UINT16)(x) & (UINT16)0x00ffU) << 8) | \
			(((UINT16)(x) & (UINT16)0xff00U) >> 8) )) \
)

#define bswap32(x) \
( \
	((UINT32)( \
			(((UINT32)(x) & (UINT32)0x000000ffUL) << 24) | \
			(((UINT32)(x) & (UINT32)0x0000ff00UL) <<  8) | \
			(((UINT32)(x) & (UINT32)0x00ff0000UL) >>  8) | \
			(((UINT32)(x) & (UINT32)0xff000000UL) >> 24) )) \
)

#if 0

/* Endianness swap utils */
#define bswap16(x) \
({ \
	UINT16 __x = (x); \
	((UINT16)( \
			(((UINT16)(__x) & (UINT16)0x00ffU) << 8) | \
			(((UINT16)(__x) & (UINT16)0xff00U) >> 8) )); \
})

#define bswap32(x) \
({ \
	UINT32 __x = (x); \
	((UINT32)( \
			(((UINT32)(__x) & (UINT32)0x000000ffUL) << 24) | \
			(((UINT32)(__x) & (UINT32)0x0000ff00UL) <<  8) | \
			(((UINT32)(__x) & (UINT32)0x00ff0000UL) >>  8) | \
			(((UINT32)(__x) & (UINT32)0xff000000UL) >> 24) )); \
})

#endif


/** \brief Error codes */
typedef enum RETCODES
{
    RC_ERR = 0,         /* FALSE */
    RC_ERR_NONE,        /* TRUE */
    RC_ERR_REV,
    RC_ERR_INIT,
    RC_ERR_CRC,
	RC_CHECKSUM,
    RC_ERR_DEVICE,
    RC_INVALID_PAR,
	RC_UNAVAILABLE,
    RC_NULL_PTR,
    RC_OUT_OF_RANGE,
	RC_OUT_OF_ORDER,
    RC_TIMEOUT,
    RC_BUF_EMPTY,
    RC_BUF_FULL,
    RC_BUF_OVF,
    RC_BUF_OVERRUN,
	RC_AUTH_FAIL,
    RC_BUSY,
    RC_RDY,
    RC_WAIT,
	RC_UNINITIALIZED,
	RC_WRITE_ERR,
	RC_DFT_VALUE,
	RC_REJECTED
}RETCODES_t;

#endif
