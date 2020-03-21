#pragma once

#if defined(Q_DECL_EXPORT) && defined(Q_DECL_IMPORT)

#   define FIVE_DECL_EXPORT Q_DECL_EXPORT
#   define FIVE_DECL_IMPORT Q_DECL_IMPORT

#else // defined(Q_DECL_EXPORT) && defined(Q_DECL_IMPORT)

/*
 * Compiler & system detection for FIVE_DECL_EXPORT & FIVE_DECL_IMPORT.
 * Not using QtCore cause it shouldn't depend on Qt.
*/

#if defined(_MSC_VER)
#   define FIVE_DECL_EXPORT      __declspec(dllexport)
#   define FIVE_DECL_IMPORT      __declspec(dllimport)
#elif defined(__ARMCC__) || defined(__CC_ARM)
#   if defined(ANDROID) || defined(__linux__) || defined(__linux)
#       define FIVE_DECL_EXPORT  __attribute__((visibility("default")))
#       define FIVE_DECL_IMPORT  __attribute__((visibility("default")))
#   else
#       define FIVE_DECL_EXPORT  __declspec(dllexport)
#       define FIVE_DECL_IMPORT  __declspec(dllimport)
#   endif
#elif defined(__GNUC__)
#   if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || \
       defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#       define FIVE_DECL_EXPORT  __declspec(dllexport)
#       define FIVE_DECL_IMPORT  __declspec(dllimport)
#   else
#       define FIVE_DECL_EXPORT  __attribute__((visibility("default")))
#       define FIVE_DECL_IMPORT  __attribute__((visibility("default")))
#   endif
#else
#   define FIVE_DECL_EXPORT      __attribute__((visibility("default")))
#   define FIVE_DECL_IMPORT      __attribute__((visibility("default")))
#endif

#endif // defined(Q_DECL_EXPORT) && defined(Q_DECL_IMPORT)

/*
 * Define FIVESHARED_EXPORT for exporting function & class.
*/

#ifndef FIVESHARED_EXPORT
#if defined(__FIVE_LIBRARY__)
#  define FIVESHARED_EXPORT FIVE_DECL_EXPORT
#else
#  define FIVESHARED_EXPORT FIVE_DECL_IMPORT
#endif
#endif /*FIVESHARED_EXPORT*/
