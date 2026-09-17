/*
 * arc_control_task.c
 *
 *  Created on: 16 сент. 2026 г.
 *      Author: q
 */

#include "cmsis_os2.h"
#include "../ARC_Control/rs485_pool.h"
#include "Logger.h"


void ARC_Control_Task(void *argument)
{
	for(;;)
	{
		Recive_Frame_ARC();
		osDelay(1000);

	}
}
