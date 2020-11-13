/***********************************************************************
*   Copyright (C) JWD Automotive Co., Ltd.				               * 
*	All Rights Reserved.            					               *
*   Department : iCAS SW      									       *
*   AUTHOR	   :            										   *
************************************************************************
* Object        : 
* Module        : Nm.c
* Instance      : 
* Description   : Network Management
*-----------------------------------------------------------------------
* Version: v0.1
* Date   : Dec 7,2019 
* Author : Gao zehngzhong
***********************************************************************/
/*-History--------------------------------------------------------------
* Version    Date           Name            Changes and comments
------------------------------------------------------------------------
* 0.1	     Dec 7,2019     Gao Zhengzhong  Initial version
*=====================================================================*/

#include "nm.h"

#include "can.h"
#include "can_if.h"
#include "desc_ser.h"
#include "IO.h"

#define DESC_GLOBALS

/**** Definition of variables ****/
NM_STAT INT16U CanNm_BusOff_REC = 0u;
NM_STAT INT16U CanNm_BusOff_TEC = 0u;
NM_STAT NM_STATUS_TYPE CanNm_NmState = NM_STATUS_OFF;
NM_STAT BUSOFF_STATUS_TYPE CanNm_BusOffState = BUSOFF_INIT;

/**** Declaration of functions ****/
NM_STAT void CanNm_MainFunction(void);
NM_STAT void CanNm_Transmit(void);
NM_STAT void CanNm_BusOffTransmit(void);
NM_STAT void CanNm_GetBusOffRecIndication(void);
NM_STAT void CanNm_GetBusOffTecIndication(void);
NM_STAT void CanNm_NmStatus(NM_STATUS_TYPE nm_state);
NM_STAT void CanNm_NmBusOffStatus(BUSOFF_STATUS_TYPE busoff_state);


/**********************************************************************************************************************
 * CanNm_TaskEntry
 **********************************************************************************************************************/
/*! \brief      Application of the CAN Network Management
 *              By default the NM starts in the normal state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
void CanNm_TaskEntry(void)
{
	CanNm_MainFunction();
}

/**********************************************************************************************************************
 * CanNm_vInit
 **********************************************************************************************************************/
/*! \brief      Initialization of the CAN Network Management
 *              and its internal state machine
 *              By default the NM starts in the Off state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
void CanNm_vInit(void)
{
	CanNm_BusOff_REC = 0u;
	CanNm_BusOff_TEC = 0u;
	CanNm_BusOffState = BUSOFF_INIT;
	CanNm_NmState = NM_STATUS_OFF;
}

/**********************************************************************************************************************
 * CanNm_MainFunction
 **********************************************************************************************************************/
/*! \brief      CAN Network Management and its internal state machine
 *              By default the NM starts in the all state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
NM_STAT void CanNm_MainFunction(void)
{
	CanNm_Transmit();
	CanNm_BusOffTransmit();
}

/***********************************************************************************************************************
 *  CanNm_Transmit
 **********************************************************************************************************************/
/*! \brief      This function is called by the NM or application to transmit a NM message within the next main function
 *  \param[in]  CanNmTxPduId  L-PDU handle of CAN L-PDU to be transmitted. This handle specifies the corresponding CAN
 *                            LPDU ID and implicitly the CAN Driver instance as well as the corresponding CAN
 *                            controller device.
 *  \param[in]  PduInfoPtr    Pointer to a structure with CAN L-PDU related data: DLC and pointer to CAN L-SDU buffer.
 *  \pre        NM is initialized
 *  \return     E_OK - transmit request has been accepted
 *  \return     E_NOT_OK - transmit request has not been accepted due to wrong state
 *  \context    Function could be called from interrupt level or from task level
 *  \note       Called from PduR
 **********************************************************************************************************************/
NM_STAT void CanNm_Transmit(void)
{
	switch (CanNm_NmState)
	{		
		case NM_STATUS_OFF:
			
			if (STD_ON == IO_ubReadPin(IO_P7_3)) // Read IG signal pin status
			{
				CanNm_NmStatus(NM_STATUS_STARTUP_MODE);				
			}
			else
			{
				CanNm_NmStatus(NM_STATUS_OFF);
			}

			break;

		case NM_STATUS_STARTUP_MODE:
			
			CanNm_NmStatus(NM_STATUS_NORMAL_WORKING);
			
			break;

		case NM_STATUS_NORMAL_WORKING:
			
			if (STD_OFF == IO_ubReadPin(IO_P7_3))
			{
				CanNm_NmStatus(NM_STATUS_SHUTDOWN_MODE);
			}
			else
			{
				if (BUSOFF_SHUTDOWN_ERR == CanNm_BusOffState)
				{
					CanNm_NmStatus(NM_STATUS_BUS_ERROR);
				}
			}
			
			break;

		case NM_STATUS_BUS_ERROR:
			
			if (STD_OFF == IO_ubReadPin(IO_P7_3))
			{
				CanNm_NmStatus(NM_STATUS_SHUTDOWN_MODE);
			}
			else
			{
				if (BUSOFF_SHUTDOWN_ERR != CanNm_BusOffState)
				{
					CanNm_NmStatus(NM_STATUS_STARTUP_MODE);
				}
			}
			
			break;

		case NM_STATUS_SHUTDOWN_MODE:
			
			CanNm_NmStatus(NM_STATUS_OFF);
			
			break;

		default:
			
			CanNm_NmStatus(NM_STATUS_OFF);
			
			break;
	}
}

/**********************************************************************************************************************
 * CanNm_NmStatus
 **********************************************************************************************************************/
/*! \brief      Running of the CAN Network Management
 *              and its internal state machine
 *              By default the NM starts in the all state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
NM_STAT void CanNm_NmStatus(NM_STATUS_TYPE nm_state)
{
	/* NM_STATUS_OFF */
	if (NM_STATUS_OFF == nm_state)
	{
		CanNm_DisableCommunication();
		CanNm_NmState = NM_STATUS_OFF;
	}

	/* NM_STATUS_STARTUP_MODE */
	if (NM_STATUS_STARTUP_MODE == nm_state)
	{
		Can_lInitializeController(CAN_CONTROLLER0, 0x3447);
		CanNm_EnableCommunication();
		CanNm_NmState = NM_STATUS_STARTUP_MODE;
	}

	/* NM_STATUS_NORMAL_WORKING */
	if (NM_STATUS_NORMAL_WORKING == nm_state)
	{
		CanNm_NmState = NM_STATUS_NORMAL_WORKING;
	}	

	/* NM_STATUS_BUS_ERROR */
	if (NM_STATUS_BUS_ERROR == nm_state)
	{
		CanNm_DisableCommunication();
		Can_lInitializeController(CAN_CONTROLLER0, 0x3447);		
	}		

	/* NM_STATUS_SHUTDOWN_MODE */
	if (NM_STATUS_SHUTDOWN_MODE == nm_state)
	{
		CanNm_DisableCommunication();
	}		
}

/**********************************************************************************************************************
 * CanNm_BusOffTransmit
 **********************************************************************************************************************/
/*! \brief      Running of the CAN Bus-Off
 *              and its internal state machine
 *              By default the NM starts in the normal state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
NM_STAT void CanNm_BusOffTransmit(void)
{
	CanNm_GetBusOffRecIndication();
	CanNm_GetBusOffTecIndication();

	switch (CanNm_BusOffState)
	{
		case BUSOFF_INIT:
			CanNm_NmBusOffStatus(BUSOFF_INIT);
			break;

		case BUSOFF_NORMAL:			
			CanNm_NmBusOffStatus(BUSOFF_NORMAL);
			break;

		case BUSOFF_ACTIVE_ERR:			
			CanNm_NmBusOffStatus(BUSOFF_ACTIVE_ERR);
			break;

		case BUSOFF_PASSIVE_ERR:			
			CanNm_NmBusOffStatus(BUSOFF_PASSIVE_ERR);
			break;

		case BUSOFF_SHUTDOWN_ERR:			
			CanNm_NmBusOffStatus(BUSOFF_SHUTDOWN_ERR);
			break;

		default:
			CanNm_NmBusOffStatus(BUSOFF_INIT);			
			break;
	}
}

/**********************************************************************************************************************
 * CanNm_NmBusOffStatus
 **********************************************************************************************************************/
/*! \brief      Running of the CAN Bus-Off
 *              and its internal state machine
 *              By default the NM starts in the all state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
NM_STAT void CanNm_NmBusOffStatus(BUSOFF_STATUS_TYPE busoff_state)
{
	NM_STAT INT16U busoffResumeCnt = 0u; 

	/* BUSOFF_INIT */
	if (BUSOFF_INIT == busoff_state)
	{
		CanNm_BusOff_REC = 0u;
		CanNm_BusOff_TEC = 0u;
		CanNm_BusOffState = BUSOFF_NORMAL;
	}

	/* BUSOFF_NORMAL */
	if (BUSOFF_NORMAL == CanNm_BusOffState)
	{
		if ((CanNm_BusOff_REC > 0u) || (CanNm_BusOff_TEC > 0u))
		{
			CanNm_BusOffState = BUSOFF_ACTIVE_ERR;
		}
	}
	
	/* BUSOFF_ACTIVE_ERR */
	if (BUSOFF_ACTIVE_ERR == CanNm_BusOffState)
	{
		if ((CanNm_BusOff_REC > 127u) || (CanNm_BusOff_TEC > 127u))
		{
			CanNm_BusOffState = BUSOFF_PASSIVE_ERR;
		}
		else
		{
			if ((0u == CanNm_BusOff_REC) && (0u == CanNm_BusOff_TEC))
			{
				CanNm_BusOffState = BUSOFF_NORMAL;
			}
		}
	}

	/* BUSOFF_PASSIVE_ERR */
	if (BUSOFF_PASSIVE_ERR == CanNm_BusOffState)
	{
		if ((CanNm_BusOff_REC < 128u) && (CanNm_BusOff_TEC < 128u))
		{
			CanNm_BusOffState = BUSOFF_ACTIVE_ERR;
		}
		else
		{
			if (CanNm_BusOff_TEC > 255u)
			{
				CanNm_BusOffState = BUSOFF_SHUTDOWN_ERR;
				
				CanIf_ControllerBusOff(CAN_CONTROLLER0);
			}
		}
	}

	/* BUSOFF_SHUTDOWN_ERR */
	if (BUSOFF_SHUTDOWN_ERR == CanNm_BusOffState)
	{
		busoffResumeCnt++;
		if (busoffResumeCnt >= 65535u)
		{
			CanNm_BusOffState = BUSOFF_ACTIVE_ERR;
			busoffResumeCnt = 0u;
		}
	}
}

/**********************************************************************************************************************
 * CanNm_GetBusOffRecIndication
 **********************************************************************************************************************/
/*! \brief      Get Rx bus-off REC value
 *              By default the NM starts in the normal state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
NM_STAT void CanNm_GetBusOffRecIndication(void)
{
	CanNm_BusOff_REC = CAN_ubGetRxErrorCounter(CAN_CONTROLLER0);	
}

/**********************************************************************************************************************
 * CanNm_GetBusOffTecIndication
 **********************************************************************************************************************/
/*! \brief      Get Tx bus-off TEC value
 *              and send message to CAN bus in cycle.
 *              By default the NM starts in the normal state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
NM_STAT void CanNm_GetBusOffTecIndication(void)
{
	CanNm_BusOff_TEC = CAN_ubGetTxErrorCounter(CAN_CONTROLLER0);		
}

/**********************************************************************************************************************
 * CanNm_EnableCommunication
 **********************************************************************************************************************/
/*! \brief      Enable of the CAN Network Management
 *              and send message to CAN bus in cycle.
 *              By default the NM starts in the normal state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
void CanNm_EnableCommunication(void)
{	
	if (TRUE == Can_SetControllerMode(CAN_CONTROLLER0, CAN_T_START))
	{
		Can_EnableControllerInterrupts(CAN_CONTROLLER0);
	}
}

/**********************************************************************************************************************
 * CanNm_DisableCommunication
 **********************************************************************************************************************/
/*! \brief      Enable of the CAN Network Management
 *              and not send message to CAN bus.
 *              By default the NM starts in the normal state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
void CanNm_DisableCommunication(void)
{	
	if (TRUE == Can_SetControllerMode(CAN_CONTROLLER0, CAN_T_STOP))
	{
		Can_DisableControllerInterrupts(CAN_CONTROLLER0);
	}
}

/**********************************************************************************************************************
 * CanNm_EnableNmCommunication
 **********************************************************************************************************************/
/*! \brief      Enable of the CAN Network Management
 *              and send NM message to CAN bus in cycle.
 *              By default the NM starts in the Off state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
void CanNm_EnableNmCommunication(void)
{
	descNmFrameTxEnable = TRUE;
	descNmFrameRxEnable = TRUE;	
}

/**********************************************************************************************************************
 * CanNm_DisableNmCommunication
 **********************************************************************************************************************/
/*! \brief      Enable of the CAN Network Management
 *              and not send NM message to CAN bus.
 *              By default the NM starts in the Off state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
void CanNm_DisableNmCommunication(void)
{
	descNmFrameTxEnable = FALSE;
	descNmFrameRxEnable = FALSE;	
}

/**********************************************************************************************************************
 * CanNm_GetBusoffStatus
 **********************************************************************************************************************/
/*! \brief      Get CAN Bus off status
 *              By default the NM starts in the Off state Mode.
 *  \param[in]  cannmConfigPtr               Configuration structure for initializing the module
 *  \pre        Interrupts must be disabled
 *  \pre        CANNM142: Initialization after CanIf and before call of any NM service
 *  \context    System Startup
 *  \note       Called by Application (EcuM)
 **********************************************************************************************************************/
BUSOFF_STATUS_TYPE CanNm_GetBusoffStatus(void)
{
	return CanNm_BusOffState;
}


