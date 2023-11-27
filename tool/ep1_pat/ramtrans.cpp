
#include "common.h"

BOOL					sRamTransInited = FALSE;

REGION_TABLE_ENTRY_T	*sRegionTable = NULL;
u32						sRegionsCount;

BOOL					gRamTransEnabled = FALSE;


BOOL RamTransInit()
{
	u32			*array;
	u32			count;
	u32			_region_table;
	u32			i, index;
	BOOL		savedRamTransEnabled = gRamTransEnabled;
	
	gRamTransEnabled = FALSE;

	array = libpatFindPattern("_region_table D E255501028A408C18AFFFFFCE1B05E85+0x1C", &count);

	if(count != 1)
		return FALSE;

	_region_table = array[0] - gCG1Offset;

	array = (u32*)&gBuffer[_region_table];
	
	sRegionsCount = (E32(array[1]) - E32(array[0])) / sizeof(REGION_TABLE_ENTRY_T);

	// START_RAM_MEM_USED
	array = (u32*)&gBuffer[_region_table + E32(array[0])];

	sRegionTable = (REGION_TABLE_ENTRY_T*)malloc(sRegionsCount * sizeof(REGION_TABLE_ENTRY_T));

	index = 0;
	for(i=0; i<sRegionsCount; i++)
	{
		sRegionTable[index].src  = E32(array[i*3 + 0]);
		sRegionTable[index].dst  = E32(array[i*3 + 1]);
		sRegionTable[index].size = E32(array[i*3 + 2]);

		//if(sRegionTable[index].src != sRegionTable[index].dst)
		if(sRegionTable[index].size != 0)
			index++;
	}

	sRegionsCount = index;

	sRamTransInited = TRUE;
	gRamTransEnabled = savedRamTransEnabled;

	return TRUE;
}

void RamTransTerm()
{
	if(sRegionTable != NULL)
		free(sRegionTable);
}


void RamTrans(u32 *target)
{
	u32		value = *target;
	u32		i;

	if(sRamTransInited == FALSE)
	{
		if(RamTransInit() == FALSE)
			return;
	}

	for(i=0; i<sRegionsCount; i++)
	{
		if( (value >= sRegionTable[i].src) && 
			(value < (sRegionTable[i].src + sRegionTable[i].size)) )
		{
			*target = value - sRegionTable[i].src + sRegionTable[i].dst;
			break;
		}
	}
}