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

typedef enum SemaphoreState_e 
{
	SEMAPHORE_FREE,
	SEMAPHORE_ALLOCATED,
} 
SemaphoreState_t;

#define MAX_TASKS 20		// Maximum number of tasks initialized
#define TASK_SWITCHING_COUNT 50	// no of times TaskSwitch function needs to be called in loop
#define MAX_SEMAPHORES 10
#define MAX_QUEUE_SIZE 10

int32_t readyQueue[MAX_QUEUE_SIZE];  // Queue for tasks waiting to be READY
int32_t runningQueue[MAX_QUEUE_SIZE]; // Queue for tasks waiting to be RUNNING

int32_t readyQueueSize = 0;
int32_t runningQueueSize = 0;

typedef void (*FunctionPointer) (void *data);

int32_t bufferdata = 10;

int32_t TaskLoopValue = 0;
int32_t flag = -1;
int32_t stateflag = 0;

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

typedef struct Semaphore_s 
{
	SemaphoreState_t state; /* State of semaphore */
	int32_t count; /* Semaphore Count */
	int32_t taskID; /* Task ID of task */
	char *name; /* Name of semaphore */
}
Semaphore_t;

Semaphore_t Semaphores[MAX_SEMAPHORES];


int32_t TaskAdd (void (*f) (void *data), void *data, TaskStatus state);
int32_t TaskPending (int32_t id);
int32_t OneSemaphore (int32_t id);
int32_t TwoSemaphore (int32_t id);
int32_t TaskReady (int32_t id);
int32_t TaskRunning (int32_t id);
int32_t TaskKill (int32_t id);
int32_t TaskCurrent (void);
void TaskSwitcher (void);
int32_t FindTaskID (void *data);
int32_t TaskStateChange (void *data);
void SemaphoreInit(void);
uint32_t SemaphoreNew(uint32_t *semaphoreHandle, int32_t count, char *name);
uint32_t SemaphorePost(uint32_t semaphoreHandle);
uint32_t SemaphoreAcquire(uint32_t semaphoreHandle);
void InitSemaphores(void);
//void ReleaseSemaphoreAndCheckQueue(uint32_t semaphore, int32_t queue[], int32_t *queueSize);
void Enqueue(int32_t queue[], int32_t *queueSize, int32_t taskId);
int32_t Dequeue(int32_t queue[], int32_t *queueSize);
int32_t TaskAdd(void (*f)(void *data), void *data, TaskStatus state);
void Task1(void *data);
void Task2(void *data);
void Task3(void *data);
void Task4(void *data);
void Task5(void *data);
ParserReturnVal_t TaskSwitcherMain(int mode);
ParserReturnVal_t Task1Main(int mode);


const char* GetTaskStateName(TaskStatus state) {
    switch (state) {
        case TASK_PENDING:
            return "TASK_PENDING";
        case TASK_READY:
            return "TASK_READY";
        case TASK_RUNNING:
            return "TASK_RUNNING";
        case TASK_INACTIVE:
            return "TASK_INACTIVE";
        default:
            return "UNKNOWN_STATE";
    }
}



void SemaphoreInit(void)
{
	uint32_t i;
	/* Initialize all semaphores to free */
	for(i=0; i<MAX_SEMAPHORES; i++) 
	{
		Semaphores[i].state = SEMAPHORE_FREE;
		Semaphores[i].count = 0;
		Semaphores[i].taskID = -1;
		Semaphores[i].name = NULL;
	}
}


uint32_t SemaphoreNew(uint32_t *semaphoreHandle, int32_t count, char *name) {
    uint32_t i;
    if(semaphoreHandle == NULL) {
        printf("Error: NULL Semaphore handle passed to SemaphoreNew()\n");
        return 1;
    }
    // Find an unallocated semaphore
    for(i = 0; i<MAX_SEMAPHORES; i++) {
        if(Semaphores[i].state == SEMAPHORE_FREE) {
            Semaphores[i].state = SEMAPHORE_ALLOCATED;
            Semaphores[i].count = count;
            Semaphores[i].taskID = -1;
            Semaphores[i].name = name;
            *semaphoreHandle = i;
            return 0;
        }
    }
    // No semaphores left
    printf("Error: Out of semaphores to allocate in SemaphoreNew()\n");
    return 1;
}

uint32_t SemaphorePost(uint32_t semaphoreHandle)
{
	if(semaphoreHandle >= MAX_SEMAPHORES) 
	{
		printf("Error: semaphoreHandle is out of range in SemaphorePost()\n");
		return 1;
	}
	if(Semaphores[semaphoreHandle].state == SEMAPHORE_FREE) 
	{
		printf("Error: trying to post to an unallocated semaphore in SemaphorePost()\n");
		return 1;
	}
	/* Post the semaphore */
	
	//printf("Task id in semaphore Post = %ld \n",Semaphores[semaphoreHandle].taskID);
	/* Can we unblock a task now */
	if((Semaphores[semaphoreHandle].count == 0) && (Semaphores[semaphoreHandle].taskID >= -1))
	{
		/* Yes, count is greater than 0,
		* and there is a task waiting */
		//OneSemaphore(Semaphores[semaphoreHandle].taskID);
		
		//printf("Reached place of -1\n");
		//Semaphores[semaphoreHandle].count--;
		Semaphores[semaphoreHandle].taskID = -1;
		Semaphores[semaphoreHandle].state = SEMAPHORE_FREE;
		Semaphores[semaphoreHandle].count++;
		if (Semaphores[semaphoreHandle].state == SEMAPHORE_FREE)
			printf("It's made FREE \n");
	}
	return 0;
}


uint32_t SemaphoreAcquire(uint32_t semaphoreHandle)
{
	if(semaphoreHandle >= MAX_SEMAPHORES) 
	{
		printf("Error: semaphoreHandle is out of range in SemaphoreAcquire()\n");
		return 1;
	}
	/*if(Semaphores[semaphoreHandle].state == SEMAPHORE_FREE) 
	{
		printf("Error: trying to acquire an unallocated semaphore in SemaphoreAcquire()\n");
		TaskPending(TaskCurrent());
		return 0;
	}*/
	/*if(Semaphores[semaphoreHandle].taskID != -1) 
	{
		printf("Error: There is a task already waiting on the semaphore in SemaphoreAcquire()\n");
		return 1;
	}*/
	/* Check if we need to block */
	if(Semaphores[semaphoreHandle].count > 0) 
	{
	/* Yes, there are no counts left */
		Semaphores[semaphoreHandle].taskID =currentTaskId;
		Semaphores[semaphoreHandle].state = SEMAPHORE_ALLOCATED;
		Semaphores[semaphoreHandle].count--;
		return 1;
	}
	//printf("Task id in semaphore Acquire = %ld \n",Semaphores[semaphoreHandle].taskID);
	TaskPending(TaskCurrent());
	return 0;
}

uint32_t semaphoreOne, semaphoreTwo;

void InitSemaphores(void) {
    SemaphoreNew(&semaphoreOne, 1, "Ready Semaphore"); //  one tasks to be READY
    SemaphoreNew(&semaphoreTwo, 1, "Running Semaphore"); // one task can be RUNNING
}


void Enqueue(int32_t queue[], int32_t *queueSize, int32_t taskId) {
    if (*queueSize < MAX_QUEUE_SIZE) {
        queue[*queueSize] = taskId;
        (*queueSize)++;
    }
}

int32_t Dequeue(int32_t queue[], int32_t *queueSize) {
    if (*queueSize > 0) {
        int32_t taskId = queue[0];
        for (int i = 0; i < (*queueSize) - 1; i++) {
            queue[i] = queue[i + 1];
        }
        (*queueSize)--;
        return taskId;
    }
    return -1;
}


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
      printf("Changing Task%ld state to TASK_PENDING\n",id+1);
      printf("Task %ld state: %s\n", id+1, GetTaskStateName(tasks[id].state));
      return 1;
}

int32_t OneSemaphore (int32_t id)
{
    if (SemaphoreAcquire(semaphoreOne) == 1) {
        if(TaskReady(id) == 1)
        {
		printf("Changing Task%ld state to TASK_READY\n",id+1);
		printf("Action: Ready Semaphore Accured \n");
		printf("Task %ld state: %s\n", id+1, GetTaskStateName(tasks[id].state));
	}
        return 1;
    } 
    else {
        Enqueue(readyQueue, &readyQueueSize, id);
        printf("Keeping Task%ld state to TASK_PENDING\n",id+1);
        printf("Task %ld state: %s\n", id+1, GetTaskStateName(tasks[id].state));
        return 0;  // Indicates the task is enqueued and not ready
    }

}

int32_t TaskReady (int32_t id)
{
  tasks[id].state = TASK_READY;
  return 1;
}

int32_t TwoSemaphore (int32_t id)
{
   if (SemaphoreAcquire(semaphoreTwo) == 1) {
        //tasks[id].state = TASK_RUNNING;
        //running_flag = 1;

        // Release a semaphoreOne when a task moves to RUNNING
        //ReleaseSemaphoreAndCheckQueue(semaphoreOne, readyQueue, &readyQueueSize);
        //currentTaskId = id;
        if(TaskRunning(id) == 1)
        {
		SemaphorePost(semaphoreOne);
	  	printf("Changing Task%ld state to TASK_RUNNING\n",id+1);
	  	printf("Action: Running Semaphore Accured \n");
	  	printf("Task %ld state: %s\n", id+1, GetTaskStateName(tasks[id].state));
	}
        return 1;
    } 
    else {
        Enqueue(runningQueue, &runningQueueSize, id);
        printf("Keeping Task%ld state to TASK_READY\n",id+1);
        printf("Task %ld state: %s\n", id+1, GetTaskStateName(tasks[id].state));
        return 0;  // Task is enqueued and not running
    }

}

int32_t TaskRunning (int32_t id)
{
  tasks[id].state = TASK_RUNNING;
  return 1;
}

int32_t TaskKill (int32_t id)
{
      if (tasks[id].state == TASK_RUNNING) {
        tasks[id].state = TASK_INACTIVE;

        // Release semaphoreTwo when a task is killed (stops running)
        //ReleaseSemaphoreAndCheckQueue(semaphoreTwo, runningQueue, &runningQueueSize);
        //currentTaskId = id;
        SemaphorePost(semaphoreTwo);
	printf("Changing Task%ld state to TASK_INACTIVE\n",id+1);
	printf("Task %ld state: %s\n", id+1, GetTaskStateName(tasks[id].state));
        running_flag = 0;  // Reset the running flag
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

int ProcessQueueTasksAndChangeState() {
    int32_t taskId;
    if (Semaphores[semaphoreTwo].count > 0 && (taskId = Dequeue(runningQueue, &runningQueueSize)) != -1) {
        TwoSemaphore(taskId);
        return 1;
    } else if (Semaphores[semaphoreOne].count > 0 && (taskId = Dequeue(readyQueue, &readyQueueSize)) != -1) {
        OneSemaphore(taskId);
        return 1;
    }
    return 0;
}

void ChangeStateOfCurrentTask() {
    if (tasks[currentTaskId].f != NULL) {
        TaskStateChange(tasks[currentTaskId].f);
    }
}


int32_t TaskStateChange(void *data) {
    
    int32_t taskId = FindTaskID(data);

    if (taskId != -1) {
        //TaskStatus currentState = tasks[taskId].state;
        //printf("Task %ld state: %s\n",taskId+1, GetTaskStateName(tasks[taskId].state));
	
        // Handle state transitions based on current state
        if (tasks[taskId].state == TASK_PENDING) {
            // Attempt to transition from PENDING to READY
            OneSemaphore(taskId);
        }
        else if (tasks[taskId].state == TASK_READY) {
            // Attempt to transition from READY to RUNNING
            TwoSemaphore(taskId);
        }
        else if (tasks[taskId].state == TASK_RUNNING) {
            // Transition from RUNNING to INACTIVE
            TaskKill(taskId);
        }
        else if (tasks[taskId].state == TASK_INACTIVE) {
            // Transition from INACTIVE to PENDING (reactivation)
            TaskPending(taskId);
        }
        else {
            printf("Unknown current state for Task%ld\n", taskId + 1);
            return 0;
        }

        return 1;
    }

    return 0;
}




void Task1 (void *data)
{
  if (FindTaskID (Task1) == -1)
    {
      if (TaskAdd (Task1, &bufferdata, 0) == -1)
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


void TaskSwitcher(void) {
    if (TaskLoopValue > 0) {
        // Part 1: Attempt state transitions before switch case logic
        if (tasks[currentTaskId].f == NULL || currentTaskId >= 5) {
            currentTaskId = 0; // Reset to the first task
        }

        if (tasks[currentTaskId].state == TASK_RUNNING) {
            TaskKill(currentTaskId); // Change running task to inactive
        } else {
            // Check running queue first if semaphore is free
            if (Semaphores[semaphoreTwo].count > 0 && runningQueueSize > 0) {
                int32_t runningTaskId = Dequeue(runningQueue, &runningQueueSize);
                if (runningTaskId != -1) {
                    printf("Queue RUN \n");
                    TwoSemaphore(runningTaskId);
                }
            }
            // Then check ready queue if semaphore is free
            else if (Semaphores[semaphoreOne].count > 0 && readyQueueSize > 0) {
                int32_t readyTaskId = Dequeue(readyQueue, &readyQueueSize);
                if (readyTaskId != -1) {
                    printf("Queue READY \n");
                    OneSemaphore(readyTaskId);
                }
            }
            // If no tasks in queues, try changing the state of the current task
            else {
                TaskStateChange(tasks[currentTaskId].f);
            }
        }

        currentTaskId = (currentTaskId + 1) % 5; // Increment for next iteration

        // Part 2: Switch case logic for task creation
        switch (flag) {
            case 0:
                Task2(NULL);
                break;
            case 1:
                Task3(NULL);
                break;
            case 2:
                Task4(NULL);
                break;
            case 3:
                Task5(NULL);
                break;
            default:
                break;
        }
        flag = (flag + 1) % 4; // Cycle through flags

        // Part 3: Repeat the state transition logic (same as Part 1)
        if (tasks[currentTaskId].f == NULL || currentTaskId >= 5) {
            currentTaskId = 0; // Reset to the first task
        }

        if (tasks[currentTaskId].state == TASK_RUNNING) {
            TaskKill(currentTaskId); // Change running task to inactive
        } else {
            // Check running queue first if semaphore is free
            if (Semaphores[semaphoreTwo].count > 0 && runningQueueSize > 0) {
                int32_t runningTaskId = Dequeue(runningQueue, &runningQueueSize);
                if (runningTaskId != -1) {
                    printf("Queue RUN \n");
                    TwoSemaphore(runningTaskId);
                }
            }
            // Then check ready queue if semaphore is free
            else if (Semaphores[semaphoreOne].count > 0 && readyQueueSize > 0) {
                int32_t readyTaskId = Dequeue(readyQueue, &readyQueueSize);
                if (readyTaskId != -1) {
                    printf("Queue READY \n");
                    OneSemaphore(readyTaskId);
                }
            }
            // If no tasks in queues, try changing the state of the current task
            else {
                TaskStateChange(tasks[currentTaskId].f);
            }
        }

        currentTaskId = (currentTaskId + 1) % 5; // Increment for next iteration

        printf("\n---------------------------------------------> Switch task -> %ld \n", TaskLoopValue);
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
  SemaphoreInit();
  InitSemaphores();
  Task1 (NULL);
  return CmdReturnOk;
}

ADD_CMD ("AddTask1", Task1Main, "Run this to Start the Tasks!");

