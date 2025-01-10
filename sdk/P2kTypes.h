/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 */

#ifndef P2K_SDK_TYPES_H
#define P2K_SDK_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Base P2K types.
 */
typedef signed   char         INT8;         /**< Signed 8-bit integer type.            */
typedef unsigned char         UINT8;        /**< Unsigned 8-bit integer type.          */
typedef signed   short        INT16;        /**< Signed 16-bit integer type.           */
typedef unsigned short        UINT16;       /**< Unsigned 16-bit integer type.         */
typedef signed   long         INT32;        /**< Signed 32-bit integer type.           */
typedef unsigned long         UINT32;       /**< Unsigned 32-bit integer type.         */
typedef signed   long long    INT64;        /**< Signed 64-bit integer type.           */
typedef unsigned long long    UINT64;       /**< Unsigned 64-bit integer type.         */

/**
 * @brief Constructed P2K types.
 */
typedef UINT8                  BOOL;        /**< Boolean type.                         */
typedef UINT8                  BYTE;        /**< Byte type.                            */
typedef UINT16                 WCHAR;       /**< Unicode UTF16-BE Wide character type. */
typedef UINT16                 WORD;        /**< Word type, 16-bit.                    */
typedef UINT16                 *PWORD;      /**< Word type pointer, 16-bit.            */
typedef UINT32                 DWORD;       /**< Double word type, 32-bit.             */
typedef UINT32                 *PDWORD;     /**< Double word type pointer, 32-bit.     */
typedef void                   VOID;        /**< Void type.                            */

/**
 * @brief General P2K macros.
 */
#if !defined(NULL)
	#define NULL               (0)          /**< Null pointer definition.              */
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

//typedef UINT8 BOOLEAN; ???
//typedef UINT16 W_CHAR;
//typedef UINT16 UIS_STRING_T;
//typedef char UIS_ASCII_CHAR;
//typedef INT8 SYN_BOOL;
//#define SYN_TRUE    (SYN_BOOL)(1)
//#define SYN_FALSE   (SYN_BOOL)(0)
//#define SYN_SUCCESS (0)
//#define SYN_FAIL    (1)
//typedef INT32 SYN_RETURN_STATUS_T;
//typedef INT32 RETURN_STATUS_T;
//#define SYN_NULL 0

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_TYPES_H */
