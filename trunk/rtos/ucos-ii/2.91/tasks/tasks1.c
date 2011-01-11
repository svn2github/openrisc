/*
  Example uC/OS-II tasks.
  
  Time-related tasks.
*/

#include "includes.h"


#define          TASK_STK_SIZE     512                /* Size of each task's stacks (# of WORDs)       */

#define          TASK_START_ID       0                /* Application tasks IDs                         */
#define          TASK_CLK_ID         1
#define          TASK_1_ID           2
#define          TASK_2_ID           3
#define          TASK_3_ID           4

#define          TASK_START_PRIO    10                /* Application tasks priorities                  */
#define          TASK_CLK_PRIO      11
#define          TASK_1_PRIO        12
#define          TASK_2_PRIO        13
#define          TASK_3_PRIO        14

OS_STK        TaskClkStk[TASK_STK_SIZE];              /* Clock      task stack                         */
OS_STK        Task1Stk[TASK_STK_SIZE];                /* Task #1    task stack                         */
OS_STK        Task2Stk[TASK_STK_SIZE];                /* Task #2    task stack                         */
OS_STK        Task3Stk[TASK_STK_SIZE];                /* Task #3    task stack                         */

void  TaskStartCreateTasks (void);
void  TaskClk(void *data);
void  Task1(void *data);
void  Task2(void *data);
void  Task3(void *data);


/* This function is called from TaskStart() in the same file as main() */
void  TaskStartCreateTasks (void)
{

  OSTaskCreateExt(TaskClk,
		  (void *)0,
		  &TaskClkStk[TASK_STK_SIZE - 1],
		  TASK_CLK_PRIO,
		  TASK_CLK_ID,
		  &TaskClkStk[0],
		  TASK_STK_SIZE,
		  (void *)0,
		  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

  OSTaskCreateExt(Task1,
		  (void *)0,
		  &Task1Stk[TASK_STK_SIZE - 1],
		  TASK_1_PRIO,
		  TASK_1_ID,
		  &Task1Stk[0],
		  TASK_STK_SIZE,
		  (void *)0,
		  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

  OSTaskCreateExt(Task2,
		  (void *)0,
		  &Task2Stk[TASK_STK_SIZE - 1],
		  TASK_2_PRIO,
		  TASK_2_ID,
		  &Task2Stk[0],
		  TASK_STK_SIZE,
		  (void *)0,
		  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

  OSTaskCreateExt(Task3,
		  (void *)0,
		  &Task3Stk[TASK_STK_SIZE - 1],
		  TASK_3_PRIO,
		  TASK_3_ID,
		  &Task3Stk[0],
		  TASK_STK_SIZE,
		  (void *)0,

		  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  
  console_clear();
  console_puts(28,2,"************************");
  console_puts(28,3,"* uC/OS-II on OpenRISC *");
  console_puts(28,4,"************************");
  console_puts(28,6,"Tasks 1: four timer tasks");
  console_puts(18,20,"Press 'q' to exit simulator, or reset board.");

    
}

/*********************************************************************************************************
 *                                               TASK #1
 *
 * Description: This task executes every 100 mS and measures the time it task to perform stack checking
 *              for each of the 5 application tasks.  Also, this task displays the statistics related to
 *              each task's stack usage.
 *********************************************************************************************************
 */

void  Task1 (void *pdata)
{
  INT8U       err;
  OS_STK_DATA data;                       /* Storage for task stack data                             */
  INT16U      time;                       /* Execution time (in uS)                                  */
  INT8U       i;
  char        s[80];


  pdata = pdata;

  console_puts(28, 10, "Memory stats per task");

  console_puts(19, 11, "Total       Free        Used        Time Taken");

  for (;;) {
    for (i = 0; i < 5; i++) {
      //TODO: Some sort of timer PC_ElapsedStart();
      err  = OSTaskStkChk(TASK_START_PRIO + i, &data);
      //TODO: time = PC_ElapsedStop();
      if (err == OS_ERR_NONE) {
	sprintf(s, "%4d        %4d        %4d        %6d",
	       data.OSFree + data.OSUsed,
	       data.OSFree,
	       data.OSUsed,
	       time);
	console_puts(19, 12 + i, s/*, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY*/);
      }
    }
    OSTimeDlyHMSM(0, 0, 0, 100);                       /* Delay for 100 mS                         */
  }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK #2
*
* Description: This task displays a clockwise rotating wheel on the screen.
*********************************************************************************************************
*/
#define SPIN1X 30
#define SPIN1Y 18
#define SPIN1DLY 20
void  Task2 (void *data)
{
  data = data;
  for (;;) {
    console_putc(SPIN1X, SPIN1Y, '|');
    OSTimeDly(SPIN1DLY);
    console_putc(SPIN1X, SPIN1Y, '/');
    OSTimeDly(SPIN1DLY);
    console_putc(SPIN1X, SPIN1Y, '-');
    OSTimeDly(SPIN1DLY);
    console_putc(SPIN1X, SPIN1Y, '\\');
    OSTimeDly(SPIN1DLY);
  }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK #3
*
* Description: This task displays a counter-clockwise rotating wheel on the screen.
*
* Note(s)    : I allocated 500 bytes of storage on the stack to artificially 'eat' up stack space.
*********************************************************************************************************
*/
#define SPIN2X 50
#define SPIN2Y 18
#define SPIN2DLY 10
void  Task3 (void *data)
{
  char    dummy[200];
  INT16U  i;


  data = data;
  for (i = 0; i < 199; i++) {        
    dummy[i] = '?';
  }
  for (;;) {
    console_putc(SPIN2X, SPIN2Y, '|');
    OSTimeDly(SPIN2DLY);
    console_putc(SPIN2X, SPIN2Y, '\\');
    OSTimeDly(SPIN2DLY);
    console_putc(SPIN2X, SPIN2Y, '-');
    OSTimeDly(SPIN2DLY);
    console_putc(SPIN2X, SPIN2Y, '/');
    OSTimeDly(SPIN2DLY);
  }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               CLOCK TASK
*********************************************************************************************************
*/

void  TaskClk (void *data)
{
  char s[64];
  int secs = 0;

  data = data;
  for (;;) {
    //PC_GetDateTime(s);
    //PC_DispStr(60, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
    sprintf(s, "Seconds: %d\n",secs);
    console_puts(33, 8, s);
    OSTimeDly(OS_TICKS_PER_SEC);
    secs++;
  }
}

