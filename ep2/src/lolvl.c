
#include "lolvl.h"
#include "features.h"
#include <dl_keypad.h>


#if FTR_KEYPAD_TYPE == KP_THREE_POLE

UINT16 lolvl_poll_keypad()
{
	return HAPI_KEYPAD_get_keycode();
}


BOOL lolvl_check_key( UINT8 key )
{
	switch ( key )
	{
		case KEY_0:
			return ( lolvl_poll_keypad() == RTM_KEY_0 );
		
		default:
			//warning lolvl_check_key(): unsupported key-code
			return FALSE;
	}
}

#elif FTR_KEYPAD_TYPE == KP_TWO_POLE

void HAPI_GPIO_config( UINT8 port, UINT8 pin, BOOL dir_select, UINT8 io_select );
void HAPI_GPIO_data_write( UINT8 port, UINT8 pin, BOOL data );


// NOTE: port from dualboot sources for V3i
UINT16 lolvl_poll_keypad_LTE2( UINT8 port, UINT8 pin )
{
	UINT16		ret;
	
	// zero port
	HAPI_GPIO_data_write( port, pin, 0 );
	
	// configure port to output
	HAPI_GPIO_config( port, pin, 1, 0 );
	
	// read port
	ret = kpp.kpdr;
	
	// configure port to input (restore)
	HAPI_GPIO_config( port, pin, 0, 0 );
	
	return ret;
}


BOOL lolvl_check_key( UINT8 key )
{
	// NOTE: maybe don't work on not-L7e phones
	
	switch ( key )
	{
	
#if defined( FTR_L7E )
		// tim apple
		case KEY_0:
			return ( lolvl_poll_keypad_LTE2( 0x02, 0x0A ) & 0x08  == 0 );
		
#elif defined( FTR_L9 )
		// lavmen
		case KEY_CLEAR:
			return ( lolvl_poll_keypad_LTE2( 0x02, 0x0D ) & 0x02  == 0 );
#endif
		
		default:
			//warning lolvl_check_key(): unsupported key-code
			return FALSE;
	}
}

#else

BOOL lolvl_check_key( UINT8 key )
{
	return FALSE;
}

#endif





/* EOF */
