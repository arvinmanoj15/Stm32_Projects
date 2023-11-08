/* simpletask.c - Simple Task for Monitor */

#include <stdio.h>
#include <stdint.h>
#include "common.h"

#include "main.h"

/* Static global counter for printing messages */
static uint32_t Counter = 0;

/* Function to print a message with the current Counter value and blink the LED */

void
TaskCounter (void)
{
  if (Counter > 0)
    {
      printf ("TaskCounter: Current Counter value: %lu\n", Counter);
      HAL_GPIO_WritePin (LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
      HAL_Delay (250);
      HAL_GPIO_WritePin (LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
      HAL_Delay (250);
      Counter--;
    }
}

/* Monitor Command to set the counter variable */
ParserReturnVal_t
CmdCount (int mode)
{
  uint32_t count;
  if (mode != CMD_INTERACTIVE)
    return CmdReturnOk;
  count = 0;
  fetch_uint32_arg (&count);
  Counter = count;
  return CmdReturnOk;
}

ADD_CMD ("count", CmdCount, "<n> Add <n> to the message print count");

