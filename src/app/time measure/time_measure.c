/***********************************************************************
*   Copyright (C) JWD Automotive Co., Ltd.				               * 
*		All Rights Reserved.          					               *
*   Department : iCAS SW      									       *
*   AUTHOR	   :            										   *
************************************************************************
* Object        : 
* Module        : time_measure.c
* Instance      : 
* Description   : A time measurement function that measures execution time 
*				  and scheduling cycles
*-----------------------------------------------------------------------
* Version: v0.1
* Date   : 2020.3.19 
* Author : Zhu jinfeng
***********************************************************************/
/*-History--------------------------------------------------------------
* Version    Date           Name            Changes and comments
------------------------------------------------------------------------
* 0.1	    2020.3.19     Zhu jinfeng     		Initial version
*=====================================================================*/
#include "XC23xxBREGS.h"
#include "Para_Fun_Type.h"
#include "GPT2.h"
#include "time_measure.h"

static BOOLEAN bTimMeasureEnable = FALSE;
static T_TimeMeasure tTimeMeasure[TIME_MESSAGE_NUMBER] = {0};
static BOOLEAN btaskPeriodMeasureCondition[TIME_MESSAGE_NUMBER] = {0};
static BOOLEAN btaskExecuteMeasureCondition[TIME_MESSAGE_NUMBER] = {0};


/***********************************************************************
*  Name        : TimeMeasure_Init
*  Description : After power on, Initializes the variables used for 
*                timing measurements and start GPT2 Timer5.
*  Parameter   : None
*  Returns     : None
***********************************************************************/
void TimeMeasure_Init(void)
{     
    INT8U i = 0; 
    for(i = 0; i < TIME_MESSAGE_NUMBER; i++)
    {
		tTimeMeasure[i].u16taskStartTime = 0;
		tTimeMeasure[i].u16taskEndTime = 0;

		tTimeMeasure[i].u16taskExecuteTime = 0;
		tTimeMeasure[i].u16taskExecuteTimeMin = 0xFFFFu;
		tTimeMeasure[i].u16taskExecuteTimeMax = 0;

		tTimeMeasure[i].u16taskPeriodTime = 0;
		tTimeMeasure[i].u16taskPeriodTimeMin = 0xFFFFu;
		tTimeMeasure[i].u16taskPeriodTimeMax = 0;

		btaskPeriodMeasureCondition[i] = FALSE;
		btaskExecuteMeasureCondition[i] = FALSE;
    } 
}


/***********************************************************************
*  Name        : TimeMeasure_StartTimer
*  Description : Start the GPT timer 5 counter.
*  Parameter   : None
*  Returns     : None
***********************************************************************/
void TimeMeasure_StartTimer(void)
{
	GPT2_vStartTmr_GPT2_TIMER_5(); /* GPT2 Timer5 start counting */
	bTimMeasureEnable = TRUE;
}

/***********************************************************************
*  Name        : TimeMeasure_StopTimer
*  Description : Stop the GPT timer 5 counter.
*  Parameter   : None
*  Returns     : None
***********************************************************************/
void TimeMeasure_StopTimer(void)
{
	GPT2_vStopTmr_GPT2_TIMER_5(); /* GPT2 Timer5 stop counting */
	bTimMeasureEnable = FALSE;
}


/***********************************************************************
*  Name        : TimeMeasure_Start
*  Description : Record the counter value before the task starts. And the 
*				call cycle can be calculated based on two differences.				 
*  Parameter   : None
*  Returns     : None
***********************************************************************/
void TimeMeasure_Start(INT8U taskId)
{
	static INT16U u16taskStartTimeLast[TIME_MESSAGE_NUMBER] = {0u, 0u, 0u, 0u};
	
    if (TRUE == bTimMeasureEnable)
    {
		tTimeMeasure[taskId].u16taskStartTime = GPT2_uwReadTmr_GPT2_TIMER_5();
		
		btaskExecuteMeasureCondition[taskId] = TRUE;
		
		if(TRUE == btaskPeriodMeasureCondition[taskId])
		{
			if(tTimeMeasure[taskId].u16taskStartTime > u16taskStartTimeLast[taskId])
			{
				tTimeMeasure[taskId].u16taskPeriodTime = tTimeMeasure[taskId].u16taskStartTime - u16taskStartTimeLast[taskId];
			}
			else
			{
				tTimeMeasure[taskId].u16taskPeriodTime = 0xFFFFu - u16taskStartTimeLast[taskId] + tTimeMeasure[taskId].u16taskStartTime;
			}
			
			if(tTimeMeasure[taskId].u16taskPeriodTime < tTimeMeasure[taskId].u16taskPeriodTimeMin)
			{
				tTimeMeasure[taskId].u16taskPeriodTimeMin = tTimeMeasure[taskId].u16taskPeriodTime;
			}
			if(tTimeMeasure[taskId].u16taskPeriodTime > tTimeMeasure[taskId].u16taskPeriodTimeMax)
			{
				tTimeMeasure[taskId].u16taskPeriodTimeMax = tTimeMeasure[taskId].u16taskPeriodTime;
			}
		}
		else
		{
			btaskPeriodMeasureCondition[taskId] = TRUE;
		}

		u16taskStartTimeLast[taskId] = tTimeMeasure[taskId].u16taskStartTime;
    }
}


/***********************************************************************
*  Name        : TimeMeasure_End
*  Description : The counter value is recorded after the task. And the 
*				 task execution time can be calculated based on two differences.
*  Parameter   : None
*  Returns     : None
***********************************************************************/
void TimeMeasure_End(INT8U taskId)
{
    if (TRUE == bTimMeasureEnable)
    {
		tTimeMeasure[taskId].u16taskEndTime = GPT2_uwReadTmr_GPT2_TIMER_5();
      
		if(TRUE == btaskExecuteMeasureCondition[taskId])
		{
			if(tTimeMeasure[taskId].u16taskEndTime > tTimeMeasure[taskId].u16taskStartTime)
			{
				tTimeMeasure[taskId].u16taskExecuteTime = tTimeMeasure[taskId].u16taskEndTime - tTimeMeasure[taskId].u16taskStartTime;
			}
			else
			{
				tTimeMeasure[taskId].u16taskExecuteTime = 0xFFFFu - tTimeMeasure[taskId].u16taskStartTime + tTimeMeasure[taskId].u16taskEndTime;
			}
			
			if(tTimeMeasure[taskId].u16taskExecuteTime < tTimeMeasure[taskId].u16taskExecuteTimeMin)
			{
				tTimeMeasure[taskId].u16taskExecuteTimeMin = tTimeMeasure[taskId].u16taskExecuteTime;
			}
			if(tTimeMeasure[taskId].u16taskExecuteTime > tTimeMeasure[taskId].u16taskExecuteTimeMax)
			{
				tTimeMeasure[taskId].u16taskExecuteTimeMax = tTimeMeasure[taskId].u16taskExecuteTime;
			}
		}
		else
		{
			btaskExecuteMeasureCondition[taskId] = FALSE;
		}
    }
}

