#include "timer_interface.h"
#include "cmsis_os.h"



//Create Global Timers

#define MAX_NUM_TIMERS 10

typedef struct st_TimerHandle{
	
	TimerHandle_t xTimer;
	uint32_t TimeToExpire;
	uint32_t Allocated;
	
}st_TimerHandle_t, *pst_TimerHandle_t;

/* An array to hold handles to the created timers. */
st_TimerHandle_t stTimers[MAX_NUM_TIMERS]; 
static int32_t AllocatedTimerIndex = -1;

void vTimerCallback( TimerHandle_t pxTimer )
{
}



 /* ...    Create before creating tasks     ... */
void create_global_timers( void )
{
	uint32_t x;

  /* Create some timers.  Don't start them. Starting the timers before the RTOS
     scheduler has been started means the timers will start running
     immediately that the RTOS scheduler starts. Starting will be done inside code */
  
  for( x = 0; x < MAX_NUM_TIMERS; x++ )
  {
//          xTimers[ x ] = xTimerCreate
    stTimers[x].xTimer = xTimerCreate
                   (  /* Just a text name, not used by the RTOS kernel. */
                     "Timer",
                     /* The timer period in ticks, must be greater than 0 */
                     2000,
                     /* The timers will auto-reload themselves when they expire. */
                     pdFALSE,
                     /* Assign each timer a unique id equal to its array index. */
                     ( void * ) x,
                     /* Each timer calls the same callback when it expires. */
                     vTimerCallback
                   );

         if( stTimers[x].xTimer == NULL )
         {
             /* The timer was not created. */
//					 printf("\r\ntimer was not created index = %d\r\n", x);
         }
         else
         {
         }
     }

}


///**
// * @brief Initialize a timer
// *
// * Performs any initialization required to the timer passed in.
// *
// * @param Timer - pointer to the timer to be initialized
// */


void InitTimer(Timer* timer)
{
	uint32_t i = 0;
	AllocatedTimerIndex++;
  
	
	for(i=0; i < MAX_NUM_TIMERS; i++)
	{
		if(!stTimers[i].Allocated)
		{
			stTimers[i].Allocated = 1;
			*timer = &stTimers[i];
//			printf("Allocated TimerIndex =  %d \r\n", i);
			goto exit;
		}
	}
//	printf("Reached MAX timers =  %d \r\n", MAX_NUM_TIMERS);

	exit:	
	;
}

void DeInitTimer(Timer* timer)
{

// 	printf("DeInitTimer\r\n");
//	printf("DeInitTimer: %d\r\n", pvTimerGetTimerID(((st_TimerHandle_t *)*timer)->xTimer ));
	AllocatedTimerIndex--;
	xTimerStop( ((pst_TimerHandle_t)*timer)->xTimer, 0 );
	((pst_TimerHandle_t)*timer)->TimeToExpire = 0;
	((pst_TimerHandle_t)*timer)->Allocated = 0;
}








/**
 * @brief Check if a timer is expired
 *
 * Call this function passing in a timer to check if that timer has expired.
 *
 * @param Timer - pointer to the timer to be checked for expiration
 * @return character - 1 = timer expired, 0 = timer not expired
 */
char expired(Timer* timer)
{

	//printf("expired: %d\r\n", pvTimerGetTimerID(((pst_TimerHandle_t)*timer)->xTimer) );
	if( xTimerIsTimerActive(((pst_TimerHandle_t)*timer)->xTimer) != pdFALSE )// active means not expired
	{
//		printf("Timer NOT expired TimerId: %d\r\n", pvTimerGetTimerID(((pst_TimerHandle_t)*timer)->xTimer));
		return pdFALSE; //0 = timer not expired
	}
	else
	{
		((pst_TimerHandle_t)*timer)->TimeToExpire = 0;
//		printf("Timer expired TimerId: %d\r\n", pvTimerGetTimerID(((pst_TimerHandle_t)*timer)->xTimer));

		return !pdFALSE;//1 = timer expired
	}
}

/**
 * @brief Create a timer (milliseconds)
 *
 * Sets the timer to expire in a specified number of milliseconds.
 *
 * @param Timer - pointer to the timer to be set to expire in milliseconds
 * @param unsigned int - set the timer to expire in this number of milliseconds
 */
void countdown_ms(Timer* timer, unsigned int time_ms)
{

    if( xTimerChangePeriod( ((pst_TimerHandle_t)*timer)->xTimer, time_ms / portTICK_PERIOD_MS, 0 ) == pdPASS )
    {
			((pst_TimerHandle_t)*timer)->TimeToExpire = xTaskGetTickCount();;
    }
		else
		{
			 //printf("xTimerReset failed\r\n");
			//((pst_TimerHandle_t)*timer)->TimeToExpire = xTaskGetTickCount();;
		}
		 
}

/**
 * @brief Create a timer (seconds)
 *
 * Sets the timer to expire in a specified number of seconds.
 *
 * @param Timer - pointer to the timer to be set to expire in seconds
 * @param unsigned int - set the timer to expire in this number of seconds
 */
void countdown(Timer* timer, unsigned int time_s)
{
//  printf("++ countdown\r\n");
	countdown_ms(timer, time_s * 1000);
}

/**
 * @brief Check the time remaining on a give timer
 *
 * Checks the input timer and returns the number of milliseconds remaining on the timer.
 *
 * @param Timer - pointer to the timer to be set to checked
 * @return int - milliseconds left on the countdown timer
 */
int left_ms(Timer* timer)
{
	unsigned int time_s = 0; 
	if( xTimerIsTimerActive(((pst_TimerHandle_t)*timer)->xTimer ) != pdFALSE )
	{
		//Active
		time_s = xTimerLeftMs(((pst_TimerHandle_t)*timer)->xTimer);
		time_s = time_s - ((xTaskGetTickCount()) - ((pst_TimerHandle_t)*timer)->TimeToExpire);
//		printf("left_ms:%d of TimerID %d\r\n", time_s, pvTimerGetTimerID(((pst_TimerHandle_t)*timer)->xTimer));
	}
	else
	{
		//Not active
		time_s = 0;
//		printf("left_ms:%d of TimerID %d\r\n", time_s, pvTimerGetTimerID(((pst_TimerHandle_t)*timer)->xTimer));
	}
  	
	return time_s;
//	return 2000;
}

/**
* @brief   Wait for Timeout (Time Delay)
* @param   millisec      time delay value
* @retval  status code that indicates the execution status of the function.
*/

void fsleep(int parm) 
{    
	osDelay(parm);
	
	#if INCLUDE_vTaskDelay
  TickType_t ticks = parm / portTICK_PERIOD_MS;
  
  vTaskDelay(ticks ? ticks : 1);          /* Minimum delay = 1 tick */
  
#else
  (void) parm;
  
#endif
	
}
