/**
 * @file P2kTypes.h
 * @brief SDK type definitions for various data types used in the P2K platform.
 * @defgroup P2K_Types P2K Type Definitions
 * @{
 */

#ifndef P2K_SDK_DEVICE_LAYER_SEEM_H
#define P2K_SDK_DEVICE_LAYER_SEEM_H

#include "P2kT.h"
#include "P2kSeemT.h"

#ifdef __cplusplus
extern "C" {
#endif


/*************************
  Симы
*************************/

struct SEEM_ELEMENT_DATA_CONTROL_T{
#if defined(EM1) || defined(EM2) || defined(EA1)
	UINT32 seem_offset;
	UINT32 seem_size;
	UINT16 seem_element_id;
	UINT16 seem_record_number;
#else
	UINT16 seem_element_id;
	UINT16 seem_record_number;
	UINT32 seem_offset;
	UINT32 seem_size;
#endif
};

//*data_ctrl_ptr - адрес структуры, содержащей информацию для запроса.
//*data_buf - адрес куда будет считана информация
// read_zero_byte_allowed - возможно ли читать симы размером меньше 255 байт (TRUE - да, FALSE - нет)

// чтение из сима
UINT16 SEEM_ELEMENT_DATA_read(
	SEEM_ELEMENT_DATA_CONTROL_T *data_ctrl_ptr,  // указатель на структуру данных, содержащую информацию о запросе
	UINT8 *data_buf,  // указатель на буфер, куда прочитаются данные
	BOOL read_zero_byte_allowed  // если true, то можно читать маленькие симы, длина которых меньше 255
);

//  запись в сим
UINT16 SEEM_ELEMENT_DATA_write(
	SEEM_ELEMENT_DATA_CONTROL_T *data_ctrl_ptr,
	UINT8 *seem_data_ptr  // указатель на буфер, где хранятся записываемые данные
);

// TODO: Check this!
//  возвращает размер сима
UINT32 SEEM_GET_ADDRESS_LENGTH_element_length(UINT16 seem_element_id);
// возвращает размер сима
UINT32 SEEM_ELEMENT_get_length(UINT16 seem_element_id);

// возвращает кол-во records (записей) сима
UINT16 SEEM_MAX_RECORD_get_max_record(UINT16 seem_element_id);

/* Читает в буфер seem_data count байт сима seem, записи record
    Перед чтением ОБЯЗАТЕЛЬНО выделить не менее count байт памяти! */
    // читает в буфер seem_data count байт сима seem, записи record. Перед чтением ОБЯЗАТЕЛЬНО выделить не менее count байт
// памяти!
UINT32 SEEM_FDI_OUTSIDE_SEEM_ACCESS_read(UINT32 seem, UINT32 record, void *seem_data, UINT32 count);

/* Записывает из буфера seem_data count байт в сим seem, запись record
    Не проверено */
#define SEEM_WRITE_METHOD_ADD    0
#define SEEM_WRITE_METHOD_UPDATE 1
// записывает из буфера seem_data count байт в сим seem, запись records
UINT32 SEEM_FDI_OUTSIDE_SEEM_ACCESS_write(UINT32 method, UINT32 seem, UINT32 record, void *seem_data, UINT32 count);

#ifdef __cplusplus
}
#endif

#endif  /* P2K_SDK_DEVICE_LAYER_SEEM_H */

/** @} */ /* end of P2K_Types */
