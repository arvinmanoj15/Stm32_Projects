#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "common.h"

#include "main.h"

#define MAX_TASKS 20		// Maximum number of tasks initialized
#define TASK_SWITCHING_COUNT 50	// no of times TaskSwitch function needs to be called in loop

typedef void (*FunctionPointer) (void *data);

int32_t bufferdata = 10;

int32_t TaskLoopValue = 0;
int32_t flag = -1;

// to hold the index of the current task running
int32_t currentTaskId = 0;

// Type definition for Tasks structure
typedef struct Task_struct
{
  FunctionPointer f;
  void *data;
  int32_t priority;
} Task_typedef;

Task_typedef tasks[MAX_TASKS];

int32_t TaskAdd (void (*f) (void *data), void *data, int32_t priority);
int32_t TaskKill (int32_t id);
int32_t TaskCurrent (void);
void TaskSwitcher (void);
int32_t FindTaskID (void *data);

void Task1 (void *data);
void Task2 (void *data);
void Task3 (void *data);
void Task4 (void *data);
void Task5 (void *data);

int32_t TaskAdd (void (*f) (void *data), void *data, int32_t priority)
{
  /* Try to find an empty slot */
  for (int32_t i = 0; i < MAX_TASKS; i++)
    {
      if (tasks[i].f == NULL)
	{
	  tasks[i].f = f;
	  tasks[i].data = data;
	  tasks[i].priority = priority;
	  return i;
	}
    }
  /* No slots available, return -1 */
  return -1;
}

int32_t TaskKill (int32_t id)
{
  if (tasks[id].f != NULL)
    {
      tasks[id].f = NULL;
      tasks[id].data = NULL;
    }
  else
    return -1;
  return 1;
}

int32_t TaskCurrent (void)
{
  return currentTaskId;
}

int32_t FindTaskID (void *data)
{
  for (int32_t i = 0; i < MAX_TASKS; i++)
    {
      if (tasks[i].f == data)
	{
	  return i;
	}
    }
  return -1;
}

void Task1 (void *data)
{
  if (TaskAdd (Task1, &bufferdata, 0) == -1)
    {
      printf ("No more Slots Available \n");
    }
  else
    printf ("Task 1 Created \n");
}

void Task2 (void *data)
{
  if(FindTaskID(Task2) == -1)
  {
	  if (TaskAdd (Task2, &bufferdata,0) == -1)
	    {
	      printf ("No more Slots Available \n");
	      currentTaskId++;
	    }
	  else
	  {
	    printf ("Task 2 Created \n");
	  }
  }
  else
  	printf("Task 2 Alredy Running \n");
}

void Task3 (void *data)
{
  /*if (TaskAdd (Task3, &bufferdata,1) == -1)
    {
      printf ("No more Slots Available \n");
      currentTaskId++;
    }
  else
    printf ("Task 3 Created by Task 2 \n"); */
  if(FindTaskID(Task3) == -1)
  {
	  if (TaskAdd (Task3, &bufferdata,1) == -1)
	    {
	      printf ("No more Slots Available \n");
	      currentTaskId++;
	    }
	  else
	  {
	    printf ("Task 3 Created By Task 2\n");
	  }
  }
  else
  	printf("Task 3 Alredy Running \n");
}

void Task4 (void *data)
{
/*
  if (TaskAdd (Task4, &bufferdata,1) == -1)
    {
      printf ("No more Slots Available \n");
      currentTaskId++;
    }
  else
    printf ("Task 4 Created \n");
*/
  if(FindTaskID(Task4) == -1)
  {
	  if (TaskAdd (Task4, &bufferdata,1) == -1)
	    {
	      printf ("No more Slots Available \n");
	      currentTaskId++;
	    }
	  else
	  {
	    printf ("Task 4 Created \n");
	  }
  }
  else
  	printf("Task 4 Alredy Running \n");
}

void Task5 (void *data)
{
/*
  if (TaskAdd (Task5, &bufferdata,1) == -1)
    {
      printf ("No more Slots Available \n");
      currentTaskId++;
    }
  printf ("Task 5 Created \n");
*/
  if(FindTaskID(Task5) == -1)
  {
	  if (TaskAdd (Task5, &bufferdata,1) == -1)
	    {
	      printf ("No more Slots Available \n");
	      currentTaskId++;
	    }
	  else
	  {
	    printf ("Task 5 Created \n");
	  }
  }
  else
  	printf("Task 5 Alredy Running \n");
  if (tasks[FindTaskID(Task2)].priority >= tasks[FindTaskID(Task5)].priority)
  {
	  if (TaskKill (FindTaskID (Task2)))
	    printf ("Task 2 Killed By Task 5\n");
	  else
	    printf ("Kill Unsuccessful \n");
  }
  else
  	printf("Kill Not Possible High Priority \n");
  if (tasks[FindTaskID(Task3)].priority >= tasks[FindTaskID(Task5)].priority)
  {
	  if (TaskKill (FindTaskID (Task3)))
	    printf ("Task 3 Killed By Task 5\n");
	  else
	    printf ("Kill Unsuccessful \n");
  }
  else
  	printf("Kill Not Possible High Priority \n");
  if (tasks[FindTaskID(Task4)].priority >= tasks[FindTaskID(Task5)].priority)
  {
	  if (TaskKill (FindTaskID (Task4)))
	    printf ("Task 4 Killed By Task 5\n");
	  else
	    printf ("Kill Unsuccessful \n");
  }
  else
  	printf("Kill Not Possible High Priority \n");
  if (tasks[FindTaskID(Task5)].priority >= tasks[FindTaskID(Task5)].priority)
  {
	  if (TaskKill (FindTaskID (Task5)))
	  {
	    printf ("Task 5 Killed By Task 5\n");
	    currentTaskId = 0;
	  }
	  else
	    printf ("Kill Unsuccessful \n");
  }
  else
  	printf("Kill Not Possible High Priority \n");
}

void TaskSwitcher (void)
{
  if (TaskLoopValue > 0)
  {
      switch (flag)
      {
	case 0:
	  Task2 (NULL);
	  Task3 (NULL);
	  flag++;
	  break;
	case 1:
	  Task4 (NULL);
	  flag++;
	  break;
	case 2:
	  Task5 (NULL);
	  flag++;
	  break;
      }
      //printf("Flag %ld \n",flag);
      if (flag >= 3)
	flag = 0;
      //printf("The Taskcount is %ld\n",TaskSwitcher);
      TaskLoopValue--;
  }
}

ParserReturnVal_t TaskSwitcherMain (int mode)
{
  if (mode != CMD_INTERACTIVE)
    return CmdReturnOk;
  uint32_t count;
  fetch_uint32_arg (&count);
  if (tasks[0].f != NULL)
    {
      TaskLoopValue = count;
      if(flag == -1)
      	flag = 0;
    }
  else
    {
      printf ("Task 1 not Added\n");
    }
  return CmdReturnOk;
}

ADD_CMD ("TaskSwitcher", TaskSwitcherMain, "Any Number");

ParserReturnVal_t Task1Main (int mode)
{
  if (mode != CMD_INTERACTIVE)
    return CmdReturnOk;
  Task1 (NULL);
  return CmdReturnOk;
}

ADD_CMD ("AddTask1", Task1Main, "Run this to Start the Tasks!");

