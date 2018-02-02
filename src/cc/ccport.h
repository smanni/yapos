/** \file ccport.h
 ** \brief C-Compiler portable
 ** \author S.Rossi
 **
 **  Version Control Informations:
 **     $Revision: 1548 $
 **     $LastChangedDate: 2016-02-05 12:47:03 +0100 (ven, 05 feb 2016) $
 **     $LastChangedBy: Luca.Vergani $
 **/

#ifndef __CCPORT_H
#define __CCPORT_H

#ifdef __ICCARM__
    #include <intrinsics.h>
#endif

#define __CC_STR(s)         #s
#define __CC_XSTR(s)        __CC_STR(s)

/*
 * IRQ implementation
 *
 * void __CC_IRQ ISR_Routine(void)
 * {
 *     return;
 * }
 *
 */
#ifdef __GNUC__
    #define __CC_IRQ                // __attribute__((__interrupt__)) // ignored by gcc Cortex-M compiler
#elif defined (__ICCARM__)
    #define __CC_IRQ
#endif


/*
 * INLINE a function
 *
 * inline void __CC_INLINE fInline(void)
 * {
 *     return;
 * }
 *
 */
#ifdef __GNUC__
    #define __CC_INLINE            __attribute__( ( always_inline ) )
#elif defined (__ICCARM__)
    #define __CC_INLINE
#endif


/*
 * WEAK function prototype
 *
 * __CC_WEAK_PROTO( void fweak(void) );
 *
 */
#ifdef __GNUC__
    #define __CC_WEAK_PROTO(proto)  proto
#elif defined (__ICCARM__)
    #define __CC_WEAK_PROTO(proto)  __weak proto
#endif


/*
 * WEAK function implementation
 *
 * __CC_WEAK_FUNC( void fweak(void) )
 * {
 *     return;
 * }
 *
 */
#ifdef __GNUC__
    #define __CC_WEAK_FUNC(func)   __attribute__((weak)) func
#elif defined (__ICCARM__)
    #define __CC_WEAK_FUNC(func)    __weak func
#endif


/*
 * PLACE variable to section
 *
 * __CC_PLACE(const int foo, ".sect") = 0;
 *
 */
#ifdef __GNUC__
    #define __CC_PLACE(var, sec)    __attribute__((__section__ (sec), __used__)) var
#elif defined (__ICCARM__)
    #define __CC_PLACE(var, sec)    __root var @ sec
#endif


/*
 * KEEP variable
 *
 * __CC_KEEP(bool flag) = false;
 *
 */
#ifdef __GNUC__
    #define __CC_KEEP(var)          var __attribute__((used))
#elif defined (__ICCARM__)
    #define __CC_KEEP(var)          __root var
#endif


/* No-Operation */
#ifdef __GNUC__
    #define __CC_NOP()              __asm volatile ("nop")
#elif defined (__ICCARM__)
    #define __CC_NOP()              __no_operation()
#endif


#if !(defined(__GNUC__) || defined (__ICCARM__))
    #error "unsupported C-Compiler"
#endif

/* Unsed variable */
#define __CC_UNUSED(param)  (void)(param)


#endif /* CCPORT_H_ */
