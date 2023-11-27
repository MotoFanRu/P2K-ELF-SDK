
#ifndef RAMTRANS__H
#define RAMTRANS__H

typedef struct
{
	u32			src;
	u32			dst;
	u32			size;

} REGION_TABLE_ENTRY_T;


#ifdef __cplusplus
extern "C" {
#endif


BOOL RamTransInit();
void RamTransTerm();
void RamTrans(u32 *target);


#ifdef __cplusplus
}
#endif

#endif