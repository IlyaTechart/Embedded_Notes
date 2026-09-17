/*
 * rs485_pool.c
 *
 *  Created on: 16 сент. 2026 г.
 *      Author: q
 */

#include "../ARC_Control/rs485_pool.h"

#include <stdbool.h>
#include "main.h"



typedef struct {
    arc_data_t data;
} __attribute__((aligned(32))) arc_data_aligne_t; // Выравниевание по 32-битной сетке для DMA

typedef union{
	struct{
		arc_data_aligne_t uart_rx_buffer1[2];
		arc_data_aligne_t uart_rx_buffer2[2];
		arc_data_aligne_t uart_rx_buffer3[2];
		arc_data_aligne_t uart_rx_buffer4[2];
	}arc_raft_bufers_t;
	arc_data_aligne_t shared_uart_rx_buf[8];
}arc_data_dma_t;

__attribute__((section(".sram1_bss"))) arc_data_dma_t rx_dma_pool;



static UART_HandleTypeDef* rs485_ports[MAX_ARC_COUNT] = {NULL};


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // Проходим по массиву наших портов
    for (uint8_t i = 0; i < MAX_ARC_COUNT; i++)
    {
        if (huart == rs485_ports[i])
        {
            // Здесь мы понимаем, что данные пришли по одному из наших портов RS485.
            // i - это индекс порта (0, 1, 2 или 3).

            // Если нужно, здесь можно поставить точку останова (breakpoint),
            // чтобы в отладчике посмотреть, что лежит в буфере rx_dma_pool.

            // Инвалидируем кэш, чтобы процессор увидел новые данные от DMA
            // (ВНИМАНИЕ: используем размер arc_data_aligne_t, так как он выровнен по 32 байта)
            SCB_InvalidateDCache_by_Addr((uint32_t*)&rx_dma_pool, sizeof(arc_data_dma_t));

            // Если ты хочешь, чтобы прием шел непрерывно по кругу,
            // просто перезапусти DMA здесь же:
            // HAL_UART_Receive_DMA(rs485_ports[i], (uint8_t*)&rx_dma_pool.shared_uart_rx_buf[i], sizeof(arc_data_t));

            break; // Выходим из цикла, так как нужный порт найден
        }
    }
}

char RS485_Poll_Init(UART_HandleTypeDef *port1, UART_HandleTypeDef *port2,
        UART_HandleTypeDef *port3, UART_HandleTypeDef *port4){

	if(port1 == NULL || port2 == NULL || port3 == NULL || port4 == NULL)
	{
		return -1;
	}

	rs485_ports[0] = port1;
	rs485_ports[1] = port2;
	rs485_ports[2] = port3;
	rs485_ports[3] = port4;

	return 0;

}

void Recive_Frame_ARC(void)
{
	static uint8_t PieceRxBuf = 0;

	for(uint8_t i = 0; i < MAX_ARC_COUNT; i++)
	{
	   // Порт 0 -> буфер 0 или 1
	   // Порт 1 -> буфер 2 или 3
	   // Порт 2 -> буфер 4 или 5
	   // Порт 3 -> буфер 6 или 7
	    uint8_t buf_index = (i * 2) + PieceRxBuf;
		HAL_UART_Receive_DMA(rs485_ports[i], (uint8_t*)&rx_dma_pool.shared_uart_rx_buf[buf_index], sizeof(arc_data_t));
	}

	PieceRxBuf ^= 1;
}
