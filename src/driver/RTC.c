//****************************************************************************
// @Module        Real Time Clock (RTC)
// @Filename      RTC.c
// @Project       P300.dav
//----------------------------------------------------------------------------
// @Controller    Infineon XC2365B-40F80
//
// @Compiler      Keil
//
// @Codegenerator 2.0
//
// @Description   This file contains functions that use the RTC module.
//
//----------------------------------------------------------------------------
// @Date          2020/5/8 09:17:38
//
//****************************************************************************

// USER CODE BEGIN (RTC_General,1)

// USER CODE END



//****************************************************************************
// @Project Includes
//****************************************************************************

#include "MAIN.h"

// USER CODE BEGIN (RTC_General,2)

#include "MAIN.h"

#include "can_com.h"

// USER CODE END


//****************************************************************************
// @Macros
//****************************************************************************

// USER CODE BEGIN (RTC_General,3)

// USER CODE END


//****************************************************************************
// @Defines
//****************************************************************************

// USER CODE BEGIN (RTC_General,4)

// USER CODE END


//****************************************************************************
// @Typedefs
//****************************************************************************

// USER CODE BEGIN (RTC_General,5)

// USER CODE END


//****************************************************************************
// @Imported Global Variables
//****************************************************************************

// USER CODE BEGIN (RTC_General,6)

// USER CODE END


//****************************************************************************
// @Global Variables
//****************************************************************************

// USER CODE BEGIN (RTC_General,7)

unsigned long uRtcSystemTickCount = 0u;

unsigned char bRtcOsApp2msTaskEntryFlag = FALSE;
unsigned char bRtcOsApp3msTaskEntryFlag = FALSE;
unsigned char bRtcOsApp5msTaskEntryFlag = FALSE;
unsigned char bRtcOsApp10msTaskEntryFlag = FALSE;
unsigned char bRtcOsApp20msTaskEntryFlag = FALSE;
unsigned char bRtcOsApp50msTaskEntryFlag = FALSE;
unsigned char bRtcOsApp100msTaskEntryFlag = FALSE;

// USER CODE END


//****************************************************************************
// @External Prototypes
//****************************************************************************

// USER CODE BEGIN (RTC_General,8)

// USER CODE END


//****************************************************************************
// @Prototypes Of Local Functions
//****************************************************************************

// USER CODE BEGIN (RTC_General,9)

// USER CODE END


//****************************************************************************
// @Function      void RTC_vInit(void) 
//
//----------------------------------------------------------------------------
// @Description   This is the initialization function of the RTC function 
//                library. It is assumed that the SFRs used by this library 
//                are in reset state. 
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          2020/5/8
//
//****************************************************************************

// USER CODE BEGIN (Init,1)

// USER CODE END

void RTC_vInit(void)
{

  volatile unsigned int uwTemp;

  // USER CODE BEGIN (Init,2)

  // USER CODE END

  ///  -----------------------------------------------------------------------
  ///  Configuration of the RTC Kernal 
  ///  -----------------------------------------------------------------------

  RTC_KSCCFG     =  0x0003;      // Module Enable

  uwTemp         =  RTC_KSCCFG;  // dummy read to avoid pipeline effects

  SCU_RTCCLKCON  =  0x0005;      // RTC working in synchronous mode with osc. 
                                 // freq. as a input freq.

  ///  -----------------------------------------------------------------------
  ///  Configuration of the RTC Counter:
  ///  -----------------------------------------------------------------------
  ///  - RTC input frequency is 8000.000 kHz
  ///  - the input clock is divided by factor 32
  ///  - the input clock is not divided
  ///  - counter is not started after initialization
  ///  - RTC count clock frequency is 250.000 kHz

  while(!(RTC_CON & 0x8000));       // wait until access is possible
  RTC_CON        =  0x0000;      // load RTC control register

  ///  - overflow period on T14:  1.0000 [ms]
  ///  - overflow period on CNT0: 1.0240 [s]
  ///  - overflow period on CNT1: 1.0923 [min]
  ///  - overflow period on CNT2: 1.1651 [h]
  ///  - overflow period on CNT3: 1.6570 [months]

  RTC_T14REL     =  0xFF06;      // load T14 count/reload register
  RTC_RTCL       =  0x0000;      // load RTC low register
  RTC_RELL       =  0x0000;      // load RTC reload low register
  RTC_RTCH       =  0x0000;      // load RTC high register
  RTC_RELH       =  0x0000;      // load RTC reload high register

  ///  -----------------------------------------------------------------------
  ///  Configuration of the used RTC Interrupts:
  ///  -----------------------------------------------------------------------
  ///  RTC service request node configuration:
  ///  - RTC interrupt priority level (ILVL) = 6
  ///  - RTC interrupt group level (GLVL) = 0
  ///  - RTC group priority extension (GPX) = 0

  RTC_IC         =  0x0058;     

  RTC_ISNC       =  0x0001;      // load RTC interrupt sub node register



  // USER CODE BEGIN (Init,3)

  // USER CODE END

} //  End of function RTC_vInit


//****************************************************************************
// @Function      void RTC_vRTCRun(void) 
//
//----------------------------------------------------------------------------
// @Description   This function starts the RTC timer.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          2020/5/8
//
//****************************************************************************

// USER CODE BEGIN (RTCRun,1)

// USER CODE END

void RTC_vRTCRun(void)
{

  RTC_CON_RUN = 1;

} //  End of function RTC_vRTCRun


//****************************************************************************
// @Function      void RTC_vRTCStop(void) 
//
//----------------------------------------------------------------------------
// @Description   This function stops the RTC timer.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          2020/5/8
//
//****************************************************************************

// USER CODE BEGIN (RTCStop,1)

// USER CODE END

void RTC_vRTCStop(void)
{

  RTC_CON_RUN = 0;

} //  End of function RTC_vRTCStop


//****************************************************************************
// @Function      void RTC_viRTC(void) 
//
//----------------------------------------------------------------------------
// @Description   This is the RTC interrupt service routine. It is called up 
//                when:
//                 - counter T14 overflows and is reloaded
//                 - counter CNT0 overflows and is reloaded
//                 - counter CNT1 overflows and is reloaded
//                 - counter CNT2 overflows and is reloaded
//                 - counter CNT3 overflows and is reloaded
//                This interrupt request may eg. be used to provide a system 
//                time tick independent of the CPU frequency, or to wake up 
//                regularly from idle mode.
//                
//                Please note that you have to add application specific code 
//                to this function.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          2020/5/8
//
//****************************************************************************

// USER CODE BEGIN (RTC,1)

// USER CODE END

void RTC_viRTC(void) interrupt RTCINT
{

  // USER CODE BEGIN (RTC,2)

  // USER CODE END

  if(RTC_ISNC_T14IR)             // if counter T14 overflow
  {

    // USER CODE BEGIN (RTC,3)
    
	//vTaskSuspendAll();

	//vTaskIncrementTick();

	//xTaskResumeAll();

	uRtcSystemTickCount++;

	if ((0u == (uRtcSystemTickCount % 2u)) && (FALSE == bRtcOsApp2msTaskEntryFlag))		
	{
		bRtcOsApp2msTaskEntryFlag = TRUE;
	}
	  
	if ((0u == (uRtcSystemTickCount % 3u)) && (FALSE == bRtcOsApp3msTaskEntryFlag))		
	{
		bRtcOsApp3msTaskEntryFlag = TRUE;
	}
	  
	if ((0u == (uRtcSystemTickCount % 5u)) && (FALSE == bRtcOsApp5msTaskEntryFlag))		
	{
		bRtcOsApp5msTaskEntryFlag = TRUE;
	}

	if ((0u == (uRtcSystemTickCount % 10u)) && (FALSE == bRtcOsApp10msTaskEntryFlag))		
	{
		bRtcOsApp10msTaskEntryFlag = TRUE;
	}	

	if ((0u == (uRtcSystemTickCount % 20u)) && (FALSE == bRtcOsApp20msTaskEntryFlag))		
	{
		bRtcOsApp20msTaskEntryFlag = TRUE;
	}	

	if ((0u == (uRtcSystemTickCount % 50u)) && (FALSE == bRtcOsApp50msTaskEntryFlag))		
	{
		bRtcOsApp50msTaskEntryFlag = TRUE;
	}	

	if ((0u == (uRtcSystemTickCount % 100u)) && (FALSE == bRtcOsApp100msTaskEntryFlag))		
	{
		bRtcOsApp100msTaskEntryFlag = TRUE;
	}  	
	
    // USER CODE END

    RTC_ISNC_T14IR = 0;

  }


  // USER CODE BEGIN (RTC,8)

  // USER CODE END

} //  End of function RTC_viRTC




// USER CODE BEGIN (RTC_General,10)

// USER CODE END

