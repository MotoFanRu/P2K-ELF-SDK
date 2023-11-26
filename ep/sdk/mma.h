#ifndef SDK_MMA_H
#define SDK_MMA_H

#include <typedefs.h>
#include <resources.h>

typedef struct {
    RESOURCE_ID stat;	// Static icon
    RESOURCE_ID ani;	// animated icon under corsor
    RESOURCE_ID unk;	// NULL
    RESOURCE_ID text;
} MENU_ICON_T; 


typedef struct {
	UINT8	num;
	UINT8	cursor;
} MENU_ATTRIB_T;

#ifdef __cplusplus
extern "C" {
#endif

UIS_DIALOG_T
UIS_CreateIconicMenu( SU_PORT_T * port,
					ACTIONS_T * actions,
					MENU_ICON_T * icons,
					MENU_ATTRIB_T attr,
					RESOURCE_ID dlgres );
UIS_DIALOG_T
UIS_CreateIconicAppPickerTab( SU_PORT_T * port,
							ACTIONS_T * actions, 
							MENU_ICON_T * icons,
							MENU_ATTRIB_T attr,
							RESOURCE_ID dlgres );



#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
