/*
 * rs485_pool.h
 *
 *  Created on: 16 сент. 2026 г.
 *      Author: q
 */

#ifndef ARC_CONTROL_RS485_POOL_H_
#define ARC_CONTROL_RS485_POOL_H_

#include "main.h"


#define MAX_ARC_COUNT       4    // Максимальное количество ММЦ (по числу UART портов)
#define MAX_ION_PER_ARC     120  // Максимальное количество ММА на один ММЦ


#define ID_ARC_TRANSLATE_TO_NUMBER(X)           ((uint8_t) (X & 0x0F))



#pragma pack(push, 1)

typedef struct{
	uint8_t ID_ARC;         // Индификатор ММЦ
	uint8_t count_ion;      // Колличество опрошеных ММА
}Head_arc_data_t;

typedef struct {
    uint16_t identifier;         // Индификатор ячейки
    uint32_t time;               // Время когда данные
    uint16_t voltage_mv;        // Напряжение на ячейке в милливольтах (например, 3200 = 3.2В)
    int16_t  temperature_c;     // Температура ячейки (можно в 0.1 °C, например 255 = 25.5 °C)
    uint16_t internal_res_mohm; // Внутреннее сопротивление в миллиомах (если измеряется)

    // Битовое поле статусов для экономии места
    union {
        uint16_t all_flags;
        struct {
            uint16_t is_balancing      : 1; // 1 - балансировка включена
            uint16_t comm_error        : 1; // 1 - нет связи с этим ММА (таймаут)
            uint16_t sensor_fault      : 1; // 1 - обрыв датчика температуры/напряжения
            uint16_t voltage_high_alm  : 1; // 1 - авария: перезаряд
            uint16_t voltage_low_alm   : 1; // 1 - авария: глубокий разряд
            uint16_t temp_high_alm     : 1; // 1 - авария: перегрев
            uint16_t reserved          : 10;
        } bits;
    } status;
} ion_data_t;

typedef struct {

	Head_arc_data_t Head_arc_data;   // Header фрейма
    // Данные нижнего уровня
    ion_data_t mma[MAX_ION_PER_ARC]; // Массив всех возможных ММА в этой цепи

    // Общие данные состояния шкафа
    uint16_t humidity;                // Влажность
    int16_t  ambient_temp_c;          // Температура внутри шкафа ШАБ (0.1 °C)
    uint16_t atmospheric_pressure;    // Атмосферно давление

    // Состояние цепи
    int16_t circuit_current;       // Ток цепи для ММЦ

    // Статусы ММЦ
    union {
        uint8_t all_flags;  // Потом уточнить что именно нужнро, щас здесь пример
        struct {
            uint8_t uart_comm_error   : 1;
            uint8_t contactor_state   : 1;
            uint8_t string_overload   : 1;
            uint8_t hardware_fault    : 1;
            uint8_t reserved          : 4;
        } bits;
    } status;

    uint32_t crc;  // Контрольная сумма всего фрейма
}arc_data_t;

#pragma pack(pop)




char RS485_Poll_Init(UART_HandleTypeDef *port1, UART_HandleTypeDef *port2,
        UART_HandleTypeDef *port3, UART_HandleTypeDef *port4);
void Recive_Frame_ARC(void);
void ARC_Control_Task(void *argument);



#endif /* ARC_CONTROL_RS485_POOL_H_ */
