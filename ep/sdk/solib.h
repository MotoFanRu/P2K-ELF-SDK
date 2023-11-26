#ifndef __SDK_SO_LIB_H__
#define __SDK_SO_LIB_H__


#if (defined(EP2) && defined(SHARED_LIB) && defined(__GNUC__))
	#define SO_PUBLIC		__attribute__((visibility("default")))
	#define SO_LOCAL		__attribute__((visibility("hidden")))
#else
	#define SO_PUBLIC
	#define SO_LOCAL
#endif


#endif