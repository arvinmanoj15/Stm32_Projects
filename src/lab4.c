#include <stdio.h>

#include <stdint.h>

#include <ctype.h>

#include <string.h>

#include "common.h"

#include "main.h"

typedef enum TaskStatus
{
  TASK_PENDING,
  TASK_READY,
  TASK_RUNNING,
  TASK_INACTIVE
}
TaskStatus;

#define MAX_TASKS 20		// Maximum number of tasks initialized
#define TASK_SWITCHING_COUNT 50	// no of times TaskSwitch function needs to be called in loop

typedef void (*FunctionPointer) (void *data);

int32_t bufferdata = 10;

int32_t TaskLoopValue = 0;
int32_t flag = -1;

int32_t running_flag = -1;

int32_t switcherFlag = 0;

// to hold the index of the current task running
int32_t currentTaskId = 0;

// Type definition for Tasks structure
typedef struct Task_struct
{
  FunctionPointer f;
  void *data;
  TaskStatus state;
}
Task_typedef;

Task_typedef tasks[MAX_TASKS];

int32_t TaskAdd (void (*f) (void *data), void *data, TaskStatus state);
int32_t TaskPending (int32_t id);
int32_t TaskReady (int32_t id);
int32_t TaskRunning (int32_t id);
int32_t TaskKill (int32_t id);
int32_t TaskCurrent (void);
void TaskSwitcher (void);
int32_t FindTaskID (void *data);
int32_t TaskStateChange (void *data);

void Task1 (void *data);
void Task2 (void *data);
void Task3 (void *data);
void Task4 (void *data);
void Task5 (void *data);

int32_t TaskAdd (void (*f) (void *data), void *data, TaskStatus state)
{
  /* Try to find an empty slot */
  for (int32_t i = 0; i < MAX_TASKS; i++)
    {
      if (tasks[i].f == NULL)
	{
	  tasks[i].f = f;
	  tasks[i].data = data;
	  tasks[i].state = state;
	  return i;
	}
    }
  /* No slots available, return -1 */
  return -1;
}

int32_t TaskPending (int32_t id)
{
      tasks[id].state = TASK_PENDING;
      return 1;
}

int32_t TaskReady (int32_t id)
{
      tasks[id].state = TASK_READY;
      return 1;

}

int32_t TaskRunning (int32_t id)
{
      if(running_flag == 0)
      {
      		tasks[id].state = TASK_RUNNING;
      		running_flag = 1;
      		return 1;
      }
      return 0;

}

int32_t TaskKill (int32_t id)
{
      if(tasks[id].state == TASK_RUNNING)
      {
      		tasks[id].state = TASK_INACTIVE;
      		return 1;
      }
      return 0;

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



int32_t TaskStateChange (void *data)
{
  int32_t taskId = FindTaskID (data);

  if (taskId != -1)
    {
      TaskStatus currentState = tasks[taskId].state;
      TaskStatus nextState = (TaskStatus) (currentState + 1);

      if (nextState <= TASK_INACTIVE)
	{
	  if (nextState == TASK_RUNNING && running_flag == 1)
	  {
	    nextState = TASK_PENDING; // Change to TASK_PENDING instead of TASK_RUNNING
	  }
	  
	  //tasks[taskId].state = nextState;

	  char taskName[15];
	  switch (currentState)
	    {
	    case TASK_READY:
	      strcpy (taskName, "TASK_READY");
	      break;
	    case TASK_RUNNING:
	      strcpy (taskName, "TASK_RUNNING");
	      // Reset running_flag when a task in RUNNING state is changed
	      running_flag = 0;
	      break;
	    case TASK_PENDING:
	      strcpy (taskName, "TASK_PENDING");
	      break;
	    case TASK_INACTIVE:
	      strcpy (taskName, "TASK_INACTIVE");
	      break;
	    default:
	      strcpy (taskName, "UNKNOWN_STATE");
	      break;
	    }

	  char nextStateName[15];
	  switch (nextState)
	    {
	    case TASK_READY:
	      strcpy (nextStateName, "TASK_READY");
	      if(TaskReady(taskId)==1)
	      {
	      	printf ("Error Changing State!!!\n");
	      }
	      break;
	    case TASK_RUNNING:
	      strcpy (nextStateName, "TASK_RUNNING");
	      // Set running_flag when a task is changed to RUNNING state
	      if(TaskRunning(taskId)==1)
	      {
	      	printf ("Error Changing State!!!\n");
	      }
	      running_flag = 1;
	      break;
	    case TASK_PENDING:
	      strcpy (nextStateName, "TASK_PENDING");
	      if(TaskPending(taskId)==1)
	      {
	      	printf ("Error Changing State!!!\n");
	      }
	      break;
	    case TASK_INACTIVE:
	      strcpy (nextStateName, "TASK_INACTIVE");
	      if(TaskKill(taskId)==1)
	      {
	      	printf ("Error Changing State!!!\n");
	      }
	      break;
	    default:
	      strcpy (nextStateName, "UNKNOWN_STATE");
	      break;
	    }
	  printf ("Changing %s state of Task%ld to %s\n", taskName, ++taskId,
		  nextStateName);
	}
	return 1;
    }
    return 0;
}


void Task1 (void *data)
{
  if (FindTaskID (Task1) == -1)
    {
      if (TaskAdd (Task1, &bufferdata, 1) == -1)
	{
	  printf ("No more Slots Available \n");
	}
      else
	printf ("Task 1 Created \n");
    }
  else
    printf ("Task 1 Exist \n");
}

void Task2 (void *data)
{
  if (FindTaskID (Task2) == -1)
    {
      if (TaskAdd (Task2, &bufferdata, 0) == -1)
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
    printf ("Task 2 Exist \n");
}

void Task3 (void *data)
{
  if (FindTaskID (Task3) == -1)
    {
      if (TaskAdd (Task3, &bufferdata, 0) == -1)
	{
	  printf ("No more Slots Available \n");
	  currentTaskId++;
	}
      else
	{
	  printf ("Task 3 Created \n");
	}
    }
  else
    printf ("Task 3 Exist \n");
}

void Task4 (void *data)
{
  if (FindTaskID (Task4) == -1)
    {
      if (TaskAdd (Task4, &bufferdata, 0) == -1)
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
    printf ("Task 4 Exist \n");
}

void Task5 (void *data)
{
  if (FindTaskID (Task5) == -1)
    {
      if (TaskAdd (Task5, &bufferdata, 0) == -1)
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
    printf ("Task 5 Exist\n");
}



void TaskSwitcher (void)
{
  int32_t nextTaskIndex;
  
  if (TaskLoopValue > 0)
    {
      
      
      //Task State switcher should be called here - 1
      nextTaskIndex = (currentTaskId + 1) % MAX_TASKS;
        
        // If the next task is not valid, reset to the beginning
      if (tasks[nextTaskIndex].f == NULL) 
      {
          currentTaskId = 0; // Reset to the first task
      } 
      else 
      {
          currentTaskId = nextTaskIndex; // Move to the next task
      }
      while (tasks[currentTaskId].state == TASK_INACTIVE || (tasks[currentTaskId].state == TASK_PENDING && running_flag == 1))
      {
          currentTaskId++;
      }


        // Now switch the state of the current task
      if (!TaskStateChange(tasks[currentTaskId].f)) 
      {
          printf("Failed to switch task state for task %ld\n", currentTaskId + 1);
      }
      
      
      switch (flag)
	{
	case 0:
	  Task2 (NULL);
	  break;
	case 1:
	  Task3 (NULL);
	  break;
	case 2:
	  Task4 (NULL);
	  break;
	case 3:
	  Task5 (NULL);
	  break;
	}
      //printf("Flag %ld \n",flag);
      flag = (flag + 1) % 4;
      //printf("The Taskcount is %ld\n",TaskSwitcher);
      
      nextTaskIndex = (currentTaskId + 1) % MAX_TASKS;
        
        // If the next task is not valid, reset to the beginning
      if (tasks[nextTaskIndex].f == NULL) 
      {
          currentTaskId = 0; // Reset to the first task
      }
      else 
      {
          currentTaskId = nextTaskIndex; // Move to the next task
      }
      while (tasks[currentTaskId].state == TASK_INACTIVE || (tasks[currentTaskId].state == TASK_PENDING && running_flag == 1))
      {
          currentTaskId++;
      }

        // Now switch the state of the current task
      if (!TaskStateChange(tasks[currentTaskId].f)) 
      {
          printf("Failed to switch task state for task %ld\n", currentTaskId + 1);
      }
      
      printf("\n---------------------------------------------> Switch task -> %ld \n",TaskLoopValue);
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
      if (flag == -1)
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

