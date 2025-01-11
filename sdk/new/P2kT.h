/**
 * @file P2kT.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_T_H
#define P2K_SDK_T_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name  General P2K macros
 * @brief These macros extend the syntax a little.
 * @{
 */
#if !defined(NULL)
	#define NULL               ((void *) (0)) /**< Null pointer definition.              */
#endif
#if !defined(TRUE)
	#define TRUE               ((BOOL) (1)) /**< Boolean true value.                   */
#endif
#if !defined(FALSE)
	#define FALSE              ((BOOL) (0)) /**< Boolean false value.                  */
#endif
#ifndef __cplusplus
	#if !defined(true)
		#define true           ((BOOL) (1)) /**< Boolean true value.                   */
	#endif
	#if !defined(false)
		#define false          ((BOOL) (0)) /**< Boolean false value.                  */
	#endif
#endif
#if !defined(RESULT_OK)
	#define RESULT_OK          (0)          /**< Result OK value.                      */
#endif
#if !defined(RESULT_FAIL)
	#define RESULT_FAIL        (1)          /**< Result fail value.                    */
#endif
/** @} */

/**
 * @name Basic P2K types
 * @brief These types are used to construct other.
 * @{
 */
typedef signed   char         INT8;         /**< Signed 8-bit integer type.            */
typedef unsigned char         UINT8;        /**< Unsigned 8-bit integer type.          */
typedef signed   short        INT16;        /**< Signed 16-bit integer type.           */
typedef unsigned short        UINT16;       /**< Unsigned 16-bit integer type.         */
typedef signed   long         INT32;        /**< Signed 32-bit integer type.           */
typedef unsigned long         UINT32;       /**< Unsigned 32-bit integer type.         */
typedef signed   long long    INT64;        /**< Signed 64-bit integer type.           */
typedef unsigned long long    UINT64;       /**< Unsigned 64-bit integer type.         */
/** @} */

/**
 * @name Primitive P2K types
 * @brief List of P2K platform primitive types.
 * @{
 */
typedef UINT8                  BOOL;        /**< Boolean type.                         */
typedef UINT8                  BYTE;        /**< Byte type.                            */
typedef UINT16                 WCHAR;       /**< Unicode UTF16-BE Wide character type. */
/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_T_H */

/** @} */ /* end of P2K_Types */
