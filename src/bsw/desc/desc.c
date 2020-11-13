/***********************************************************************
*   Copyright (C) JWD Automotive Co., Ltd.				               * 
*	All Rights Reserved.            					               *
*   Department : iCAS SW      									       *
*   AUTHOR	   :            										   *
************************************************************************
* Object        : 
* Module        : Desc.c
* Instance      : 
* Description   : DTC service
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

#include "desc.h"
#include "desc_ser.h"
#include "desc_mgr.h"

#include "eepRom.h"
#include "rte.h"

#define DESC_GLOBALS

/* State machine of CANdesc described by CANdelaStudio */
DESC_STAT DescStateInfo g_descCurState;
DTC_ADDRESS_TYPE g_descAddressType = PHY_ADDRESS;

/**** Declaration of functions ****/
DESC_STAT void DescServiceClassifier_10h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_11h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_14h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_19h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_22h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_23h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_27h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_28h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_2Ah(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_2Ch(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_2Eh(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_2Fh(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_31h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_34h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_36h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_37h(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_3Dh(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_3Eh(DescMsgContext* pMsgContext);
DESC_STAT void DescServiceClassifier_85h(DescMsgContext* pMsgContext);

DESC_STAT void DescOemStartSessionDefault(DescMsgContext* pMsgContext);
DESC_STAT void DescOemStartSessionProgramming(DescMsgContext* pMsgContext);
DESC_STAT void DescOemStartSessionExtended(DescMsgContext* pMsgContext);


/*******************************************************************************
* NAME:              CanDesc_vInit
*
* CALLED BY:         CanDesc_vInit
* PRECONDITIONS:     
*
* DESCRIPTION:       Initilizes the state subcomponent common data.
*
*******************************************************************************/
void CanDesc_vInit(void)
{
	g_descCurState.stateSession = kDescStateSessionDefault;
	g_descCurState.stateSecurityAccess = kDescStateSecurityAccessLocked;
}

void DESC_API_CALL_TYPE DescSetStateSession(INT8U sessionStatus)
{
	g_descCurState.stateSession = sessionStatus;
}

void DESC_API_CALL_TYPE DescSetStateSecurityAccess(INT8U securityAccessStatus)
{
	g_descCurState.stateSecurityAccess = securityAccessStatus;
}

/*  ********************************************************************************
 * Function name:DescOemStartSessionDefault
 * Description:Processes the session change request, parametrizing the common processing
 * function.
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_STAT void DescOemStartSessionDefault(DescMsgContext* pMsgContext)
{
	g_descCurState.stateSession = kDescStateSessionDefault;
	DescMgr_CanDiagDtcControlSettingOn(); 

	pMsgContext->resDataLen = 0x02;
	DescSetPosResponse(pMsgContext);
}

/*  ********************************************************************************
 * Function name:DescOemStartSessionProgramming
 * Description:Processes the session change request, parametrizing the common processing
 * function.
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_STAT void DescOemStartSessionProgramming(DescMsgContext* pMsgContext)
{		
	if (kDescStateSessionExtended == g_descCurState.stateSession)
	{
		g_descCurState.stateSession = kDescStateSessionProgramming;

		DescMgr_CanDiagDtcControlSettingOff(); 

		/* Write programming request to ROM */
		//*(INT32U*)(DESC_BOOT_RAM_ADDRESS) = DESC_BOOT_PROGRAM_REQUST;
		*(INT32U *)0xA000 = 0x2A2A2A2A;
	
		pMsgContext->resDataLen = 0x02;
		DescSetPosResponse(pMsgContext);

		/* Reset ECU and waiting enter boot */ 
		g_ecuResetFlag = TRUE;
		
	}	
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);		
	}
}

/*  ********************************************************************************
 * Function name:DescOemStartSessionExtended
 * Description:Processes the session change request, parametrizing the common processing
 * function.
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_STAT void DescOemStartSessionExtended(DescMsgContext* pMsgContext)
{
	g_descCurState.stateSession = kDescStateSessionExtended;

	pMsgContext->resDataLen = 0x02;
	DescSetPosResponse(pMsgContext);	
}

/*  ********************************************************************************
 * Function name:DescOnTransitionSession
 * Description:Notification function for state change of the given state group, defined by
 * CANdelaStudio.
 * Returns:  nothing
 * Parameter(s):
 *   - newState:
 *       - The state which will be set.
 *       - Access type: read
 *   - formerState:
 *       - The current state of this state group.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" may not be called.
 *   - The function "DescSetNegResponse" may not be called.
 ********************************************************************************  */
void DESC_API_CALLBACK_TYPE DescOnTransitionSession(DescStateGroup newState, DescMsgContext* pMsgContext)
{
    /* <<TBD>> Remove this comment once you have completely implemented this function!!! */
    /* This is only a notification function. Using the "formerState" and the "newState" 
    * parameter you can distinguish the different transitions for this state group.
    */
	if (0u == VCU_VehicleSpeed)
	{
	    switch (newState)
	    {
			case kDescStateSessionDefault:
				DescOemStartSessionDefault(pMsgContext);
				break;
				
			case kDescStateSessionProgramming:
				DescOemStartSessionProgramming(pMsgContext);	
				break;
				
			case kDescStateSessionExtended:
				DescOemStartSessionExtended(pMsgContext);
				break;
				
			default:
				g_descCurState.stateSession = kDescStateSessionDefault;
				break;
	    }		
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);
	}

    /*use app_diagnostic_current_session can get current session status*/
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
void DESC_API_CALLBACK_TYPE DescServiceClassifier(DescMsgContext* pMsgContext)
{
	/* $10:Diagnostic Session Control. Had been handle by "CANdesc" component. */
	switch (pMsgContext->reqData[0])
	{
		case 0x10: // $10 Service Classifier
			DescServiceClassifier_10h(pMsgContext);
			break;

		case 0x11: // $11 Service Classifier
			DescServiceClassifier_11h(pMsgContext);
			break;

		case 0x14: // $14 Service Classifier
			if ((kDescStateSessionDefault == g_descCurState.stateSession) || (kDescStateSessionExtended == g_descCurState.stateSession))
			{
				DescServiceClassifier_14h(pMsgContext);
			}
			break;

		case 0x19: // $19 Service Classifier
			if ((kDescStateSessionDefault == g_descCurState.stateSession) || (kDescStateSessionExtended == g_descCurState.stateSession))
			{
				DescServiceClassifier_19h(pMsgContext);
			}
			break;

		case 0x22: // $22 Service Classifier
			DescServiceClassifier_22h(pMsgContext);
			break;

		case 0x23: // $23 Service Classifier
			if ((kDescStateSessionDefault == g_descCurState.stateSession) || (kDescStateSessionExtended == g_descCurState.stateSession))
			{
				DescServiceClassifier_23h(pMsgContext);
			}
			break;

		case 0x27: // $27 Service Classifier
			if ((kDescStateSessionExtended == g_descCurState.stateSession) || (kDescStateSessionProgramming == g_descCurState.stateSession))
			{
				DescServiceClassifier_27h(pMsgContext);
			}
			break;

		case 0x28: // $28 Service Classifier
			if (kDescStateSessionExtended == g_descCurState.stateSession)
			{
				DescServiceClassifier_28h(pMsgContext);
			}	
			break;

		case 0x2A: // $2A Service Classifier
			if (kDescStateSessionExtended == g_descCurState.stateSession)
			{
				DescServiceClassifier_2Ah(pMsgContext);
			}
			break;

		case 0x2C: // $2C Service Classifier
			if (kDescStateSessionExtended == g_descCurState.stateSession)
			{
				DescServiceClassifier_2Ch(pMsgContext);
			}
			break;

		case 0x2E: // $2E Service Classifier
			//if ((kDescStateSessionExtended == g_descCurState.stateSession) || (kDescStateSessionProgramming == g_descCurState.stateSession))
			{
				DescServiceClassifier_2Eh(pMsgContext);
			}		
			break;

		case 0x2F: // $2F Service Classifier
			if (kDescStateSessionExtended == g_descCurState.stateSession)
			{
				DescServiceClassifier_2Fh(pMsgContext);
			}
			break;

		case 0x31: // $31 Service Classifier
			if ((kDescStateSessionExtended == g_descCurState.stateSession) || (kDescStateSessionProgramming == g_descCurState.stateSession))
			{
				DescServiceClassifier_31h(pMsgContext);
			}
			break;

		case 0x34: // $34 Service Classifier
			if (kDescStateSessionProgramming == g_descCurState.stateSession)
			{
				DescServiceClassifier_34h(pMsgContext);
			}		
			break;

		case 0x36: // $36 Service Classifier
			if (kDescStateSessionProgramming == g_descCurState.stateSession)
			{
				DescServiceClassifier_36h(pMsgContext);
			}		
			break;

		case 0x37: // $37 Service Classifier
			if (kDescStateSessionProgramming == g_descCurState.stateSession)
			{
				DescServiceClassifier_37h(pMsgContext);
			}		
			break;

		case 0x3D: // $3D Service Classifier
			if (kDescStateSessionExtended == g_descCurState.stateSession)
			{
				DescServiceClassifier_3Dh(pMsgContext);
			}		
			break;

		case 0x3E: // $3E Service Classifier
			DescServiceClassifier_3Eh(pMsgContext);
			break;

		case 0x85: // $85 Service Classifier
			if (kDescStateSessionExtended == g_descCurState.stateSession)
			{
				DescServiceClassifier_85h(pMsgContext);
			}
			break;

		default:
			DescSetNegResponse(pMsgContext, kDescNrcServiceNotSupported);
			break;
	}
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_10h (Service request header:$10)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_10h(DescMsgContext* pMsgContext)
{
	DESC_STAT INT32U readProgrammingRequest = 0u;
	
	/* $10:Diagnostic Session Control */
	if (0x02 == pMsgContext->reqDataLen)
	{
		switch (pMsgContext->reqData[1])
		{
			case 0x01: // $10 01 Service CallBack
				DescOnTransitionSession(kDescStateSessionDefault, pMsgContext);	
				break;

			case 0x02: // $10 02 Service CallBack
				DescOnTransitionSession(kDescStateSessionProgramming, pMsgContext);
				break;

			case 0x03: // $10 03 Service CallBack
				DescOnTransitionSession(kDescStateSessionExtended, pMsgContext);
				break;
#if ( DEBUG_BOOT_PROGRAM == 1)
			case 0x04:
				//readProgrammingRequest = *(INT32U*)(DESC_BOOT_RAM_ADDRESS);
				readProgrammingRequest = *(INT32U*)(0xE03E00);
				break;
#endif
			default:
				DescSetNegResponse(pMsgContext, kDescNrcSubfunctionNotSupported);
				break;
		}		
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);
	}
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_11h (Service request header:$11)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_11h(DescMsgContext* pMsgContext)
{
	/* $11:ECU Reset */
	switch (pMsgContext->reqData[1])
	{
		case 0x01: // $11 01 Service CallBack
			CanDiagAppCallBack(DESC_11_01_HARDWARE_RESET, pMsgContext);
			break;

		case 0x02: // $11 02 Service CallBack
			CanDiagAppCallBack(DESC_11_02_KEYOFFON_RESET, pMsgContext);
			break;

		case 0x03: // $11 03 Service CallBack
			CanDiagAppCallBack(DESC_11_03_SOFTWARE_RESET, pMsgContext);
			break;

		default:
			DescSetNegResponse(pMsgContext, kDescNrcSubfunctionNotSupported);
			break;
	}					
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_14h (Service request header:$14)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_14h(DescMsgContext* pMsgContext)
{
	/* $14:Fault Memory */
	CanDiagAppCallBack(DESC_14_CLR_DIAGNOSTIC_INFORMATION, pMsgContext); // $14 Service CallBack
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_19h (Service request header:$19)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_19h(DescMsgContext* pMsgContext)
{
	/* $19:Fault Memory*/
	switch (pMsgContext->reqData[1])
	{
		case 0x01: // $19 01 Service CallBack
			CanDiagAppCallBack(DESC_19_01_READ_NUMBER_OF_FAULTMEMORY, pMsgContext);
			break;

		case 0x02: // $19 02 Service CallBack
			CanDiagAppCallBack(DESC_19_02_READ_MASK_FAULT_MEMORY, pMsgContext);
			break;

		case 0x04: // $19 04 Service CallBack
			CanDiagAppCallBack(DESC_19_04_READ_SNAPSHOT_DATA_BY_DTC_NUMBER, pMsgContext);
			break;

		case 0x06: // $19 06 Service CallBack
			CanDiagAppCallBack(DESC_19_06_READ_EXTEND_DATA_BY_DTC_NUMBER, pMsgContext);
			break;

		case 0x0A: // $19 0A Service CallBack
			CanDiagAppCallBack(DESC_19_0A_RPT_SUPPORTED_DTC, pMsgContext);
			break;			

		default:
			DescSetNegResponse(pMsgContext, kDescNrcSubfunctionNotSupported);
			break;
	}		
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_22h (Service request header:$22)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_22h(DescMsgContext* pMsgContext)
{
	INT16U reqData = 0u;

	reqData = (INT16U)((pMsgContext->reqData[1] << 8u) | pMsgContext->reqData[2]);

	/* $22:Read By Data Identifier */
	switch (reqData)
	{
		case 0xF187: // $22 F187 Service CallBack
			CanDiagAppCallBack(DESC_22_F187_READ_YEMA_SPARE_PART_NUMBER_DATA_IDENTIFIER, pMsgContext);
			break;

		case 0xF18A: // $22 F18A Service CallBack
			CanDiagAppCallBack(DESC_22_F18A_READ_TIER1_IDENTIFIER_DATA_IDENTIFIER, pMsgContext);
			break;

		case 0xF18C: // $22 F18C Service CallBack
			CanDiagAppCallBack(DESC_22_F18C_READ_TIER1_ECU_SERIAL_NUMBER_IDENTIFIER, pMsgContext);
			break;

		case 0xF190: // $22 F190 Service CallBack
			CanDiagAppCallBack(DESC_22_F190_READ_YEMA_VIN_DATA_IDENTIFIER, pMsgContext);
			break;

		case 0xF193: // $22 F193 Service CallBack
			CanDiagAppCallBack(DESC_22_F193_READ_TIER1_ECU_HARDWARE_VERSION_NUMBER_IDENTIFIER, pMsgContext);
			break;

		case 0xF195: // $22 F195 Service CallBack
			CanDiagAppCallBack(DESC_22_F195_READ_TIER1_ECU_SOFTWARE_VERSION_NUMBER_IDENTIFIER, pMsgContext);
			break;

		case 0xF197: // $22 F197 Service CallBack
			CanDiagAppCallBack(DESC_22_F197_READ_TIER1_SYSTERM_NAME_DATA_IDENTIFIER, pMsgContext);
			break;
		
		case 0x6666: // $22 6666 Service CallBack
			CanDiagAppCallBack(DESC_22_6666_SIMULATT_ADC_DATA, pMsgContext);
			break;
		
		case 0x6667: // $22 6667 Service CallBack
			CanDiagAppCallBack(DESC_22_6667_READ_EEPROM_DATA, pMsgContext);
			break;
		
		case 0x2201: // $22 2201 Service CallBack
			CanDiagAppCallBack(DESC_22_2201_READ_PEDAL_CYLINDER_RATING_DATA, pMsgContext);
			break;

		default:
			DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);
			break;
	}		
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_23h (Service request header:$23)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_23h(DescMsgContext* pMsgContext)
{
	/* $23:Read Memory By Address */
	if (PHY_ADDRESS == g_descAddressType) // Only physical address
	{
		if ((kDescStateSecurityAccessLevel_1 == g_descCurState.stateSecurityAccess) || (kDescStateSecurityAccessLevel_2 == g_descCurState.stateSecurityAccess))
		{
			CanDiagAppCallBack(DESC_23_24_READ_MEMORY_BY_ADDRESS, pMsgContext);
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcAccessDenied);			
		}		
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcGeneralReject);	
	}
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_27h (Service request header:$27)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_27h(DescMsgContext* pMsgContext)
{
	/* $27:Security Access */
	if (PHY_ADDRESS == g_descAddressType) // Only physical address
	{
		switch (pMsgContext->reqData[1])
		{
			case 0x01: // $27 01 Service CallBack
				CanDiagAppCallBack(DESC_27_01_REQUEST_SEED_TO_REACH_SECURITY_LEVEL, pMsgContext);
				break;

			case 0x02: // $27 02 Service CallBack
				CanDiagAppCallBack(DESC_27_02_SEND_TO_REACH_SECURITY_LEVEL, pMsgContext);
				break;

			case 0x09: // $27 09 Service CallBack
				CanDiagAppCallBack(DESC_27_09_REQUEST_SEED_TO_REACH_SECURITY_LEVEL, pMsgContext);
				break;

			case 0x0A: // $27 0A Service CallBack
				CanDiagAppCallBack(DESC_27_0A_SEND_KEY_TO_REACH_SECURITY_LEVEL, pMsgContext);
				break;			

			default:
				DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);

				DescSetNegResponse(pMsgContext, kDescNrcSubfunctionNotSupported);
				break;
		}		
	}
	else
	{
		DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);

		DescSetNegResponse(pMsgContext, kDescNrcGeneralReject);	
	}
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_28h (Service request header:$28)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_28h(DescMsgContext* pMsgContext)
{
	/* $28:Communication Control */
	switch (pMsgContext->reqData[1])
	{
		case 0x00: // $28 00 Service CallBack
			CanDiagAppCallBack(DESC_28_00_COMMUNICATION_CONTROL_ENABLE_RX_AND_TX, pMsgContext);
			break;

		case 0x01: // $28 01 Service CallBack
			CanDiagAppCallBack(DESC_28_01_COMMUNICATION_CONTROL_ENABLE_RX_AND_DISABLE_TX, pMsgContext);
			break;

		case 0x02: // $28 02 Service CallBack
			CanDiagAppCallBack(DESC_28_02_COMMUNICATION_CONTROL_DISABLE_RX_AND_ENABLE_TX, pMsgContext);
			break;

		case 0x03: // $28 03 Service CallBack
			CanDiagAppCallBack(DESC_28_03_COMMUNICATION_CONTROL_DISABLE_RX_AND_TX, pMsgContext);
			break;			

		default:
			DescSetNegResponse(pMsgContext, kDescNrcSubfunctionNotSupported);
			break;
	}		
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_2Ah (Service request header:$2A)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_2Ah(DescMsgContext* pMsgContext)
{
	/* $2A:Read Data By Periodic Indentifier */
	if (PHY_ADDRESS == g_descAddressType) // Only physical address
	{
		switch (pMsgContext->reqData[1])
		{
			case 0x01: // $2A 01 Service CallBack
				CanDiagAppCallBack(DESC_2A_01_READ_DATA_BY_PERIODIC_INDENTIFIER_SEND_AT_SLOW_RATE, pMsgContext);
				break;

			case 0x02: // $2A 02 Service CallBack
				CanDiagAppCallBack(DESC_2A_02_READ_DATA_BY_PERIODIC_INDENTIFIER_SEND_AT_MEDIUM_RATE, pMsgContext);
				break;

			case 0x03: // $2A 03 Service CallBack
				CanDiagAppCallBack(DESC_2A_03_READ_DATA_BY_PERIODIC_INDENTIFIER_SEND_AT_FAST_RATE, pMsgContext);
				break;

			case 0x04: // $2A 04 Service CallBack
				CanDiagAppCallBack(DESC_2A_04_READ_DATA_BY_PERIODIC_INDENTIFIER_STOP_SENDING, pMsgContext);
				break;			

			default:
				DescSetNegResponse(pMsgContext, kDescNrcSubfunctionNotSupported);		
				break;
		}		
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcGeneralReject);
	}
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_2Ch (Service request header:$2C)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_2Ch(DescMsgContext* pMsgContext)
{
	/* $2C:Dynamically Define Data Indentifier */
	if (PHY_ADDRESS == g_descAddressType) // Only physical address
	{
		switch (pMsgContext->reqData[1])
		{
			case 0x01: // $2C 01 Service CallBack
				CanDiagAppCallBack(DESC_2C_01_DEFINE_BY_IDENTIFIER, pMsgContext);
				break;

			case 0x03: // $2C 03 Service CallBack
				CanDiagAppCallBack(DESC_2C_03_CLEAR_DYNAMICALLY_DEFINE_DATA_IDENTIFIER, pMsgContext);
				break;	

			default:
				DescSetNegResponse(pMsgContext, kDescNrcSubfunctionNotSupported);
				break;
		}		
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcGeneralReject);		
	}
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_2Eh (Service request header:$2E)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_2Eh(DescMsgContext* pMsgContext)
{
	INT16U reqData = 0u;

	reqData = (INT16U)((pMsgContext->reqData[1] << 8u) | pMsgContext->reqData[2]);

	/* $2E:Write By Data Identifier */
	if (PHY_ADDRESS == g_descAddressType) // Only physical address
	{
		//if ((kDescStateSecurityAccessLevel_1 == g_descCurState.stateSecurityAccess) || (kDescStateSecurityAccessLevel_2 == g_descCurState.stateSecurityAccess))
		{
			switch (reqData)
			{
				case 0xF190: // $2E F190 Service CallBack
					CanDiagAppCallBack(DESC_2E_F190_WRITE_YEMA_VIN, pMsgContext);
					break;
				
				case 0x6668: // $2E 6668 Service CallBack
					CanDiagAppCallBack(DESC_2E_6668_WRITE_EEPROM_DATA, pMsgContext);
					break;
				
				case 0x2E01: // $2E 2E01 Service CallBack
					CanDiagAppCallBack(DESC_2E_2E01_WRITE_PEDAL_CYLINDER_INIT_DATA, pMsgContext);
					break;
				
				case 0x2E02: // $2E 2E02 Service CallBack
					CanDiagAppCallBack(DESC_2E_2E02_WRITE_PV_TEST_FLAG, pMsgContext);
					break;

				default:
					DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);		
					break;
			}				
		}
		//else
		{
			//DescSetNegResponse(pMsgContext, kDescNrcAccessDenied);			
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcGeneralReject);		
	}
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_2Fh (Service request header:$2F)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_2Fh(DescMsgContext* pMsgContext)
{
	/* $2F:Input Output Control By Identifier */
	if (PHY_ADDRESS == g_descAddressType) // Only physical address
	{
		if (kDescStateSecurityAccessLevel_1 == g_descCurState.stateSecurityAccess)
		{
			// TODO:$2F Service
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcAccessDenied);		
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcGeneralReject);		
	}
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_31h (Service request header:$31)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_31h(DescMsgContext* pMsgContext)
{
	INT32U reqData = 0u;

	reqData = (INT32U)((pMsgContext->reqData[1] << 16u) | (pMsgContext->reqData[2] << 8u) | pMsgContext->reqData[3]);

	/* $31:Service Routine Control */
	if (PHY_ADDRESS == g_descAddressType) // Only physical address
	{
		if ((kDescStateSecurityAccessLevel_1 == g_descCurState.stateSecurityAccess) || (kDescStateSecurityAccessLevel_2 == g_descCurState.stateSecurityAccess))
		{
			switch (reqData)
			{
				case 0x01F001:
					CanDiagAppCallBack(DESC_31_01_F001_CHECK_PROGRAMMING_INTEGRITY, pMsgContext);
					break;

				case 0x01FF02:
					CanDiagAppCallBack(DESC_31_01_FF02_CHECK_PROGRAMMING_PRE_CONDITION, pMsgContext);
					break;

				case 0x01FF00:
					CanDiagAppCallBack(DESC_31_01_FF00_ERASE_MEMORY, pMsgContext);
					break;

				case 0x01FF01:
					CanDiagAppCallBack(DESC_31_01_FF01_CHECK_PROGRAMMING_DEPENDENCY, pMsgContext);
					break;

				default:
					DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);
					break;
			}
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcAccessDenied);			
		}	
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcGeneralReject);	
	}
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_34h (Service request header:$34)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_34h(DescMsgContext* pMsgContext)
{
	/* $34:Request Download */ 
	if (PHY_ADDRESS == g_descAddressType) // Only physical address
	{
		if (kDescStateSecurityAccessLevel_2 == g_descCurState.stateSecurityAccess)
		{
			CanDiagAppCallBack(DESC_34_REQUEST_DOWNLOAD, pMsgContext); // $34 Service CallBack	
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcAccessDenied);			
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcGeneralReject);		
	}
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_36h (Service request header:$36)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_36h(DescMsgContext* pMsgContext)
{
	/* $36:Transfer Data */
	if (PHY_ADDRESS == g_descAddressType) // Only physical address
	{
		if (kDescStateSecurityAccessLevel_2 == g_descCurState.stateSecurityAccess)
		{
			CanDiagAppCallBack(DESC_36_TRANSFER_DATA, pMsgContext); // $36 Service CallBack
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcAccessDenied);			
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcGeneralReject);		
	}	
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_37h (Service request header:$37)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_37h(DescMsgContext* pMsgContext)
{
	/* $37:Transfer Data */
	if (PHY_ADDRESS == g_descAddressType) // Only physical address
	{
		if (kDescStateSecurityAccessLevel_2 == g_descCurState.stateSecurityAccess)
		{
			CanDiagAppCallBack(DESC_37_REQUEST_TRANSFER_EXIT, pMsgContext); // $37 Service CallBack			
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcAccessDenied);			
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcGeneralReject);		
	}	
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_3Dh (Service request header:$3D)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_3Dh(DescMsgContext* pMsgContext)
{
	/* $3D:Write Memory By Address */ 
	if (PHY_ADDRESS == g_descAddressType) // Only physical address
	{
		if (kDescStateSecurityAccessLevel_1 == g_descCurState.stateSecurityAccess)
		{
			CanDiagAppCallBack(DESC_3D_WRITE_MEMORY_BY_ADDRESS, pMsgContext); // $3D Service CallBack	
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcAccessDenied);				
		}		
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcGeneralReject);		
	}
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_3Eh (Service request header:$3E)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_3Eh(DescMsgContext* pMsgContext)
{
	/* $3E:Tester Present */ 
	CanDiagAppCallBack(DESC_3E_TESTER_PRESENT, pMsgContext); // $3E Service CallBack
}

/*  ********************************************************************************
 * Function name:DescServiceClassifier_85h (Service request header:$85)
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request data byte.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response data.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the count of the service request data bytes (Sid is excluded).
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the count of the service response data bytes (Sid is excluded).
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.reqType:
 *       - Shows the addressing type of the request (kDescPhysReq or kDescFuncReq).
 *       - Access type: read
 *   - pMsgContext->msgAddInfo.resOnReq:
 *       - Indicates if there will be response. Allowed only to write only 0 !!!
 *       - Access type: read/write
 *   - pMsgContext->msgAddInfo.suppPosRes:
 *       - UDS only!If set no positive response will be sent on this request.
 *       - Access type: read
 * Particularitie(s) and limitation(s):
 *   - The function "DescProcessingDone" must be called from now on (within this
 * main-handler or later).
 *   - The function "DescSetNegResponse" can be called within this main-handler or later
 * but before calling "DescProcessingDone".
 ********************************************************************************  */
DESC_STAT void DescServiceClassifier_85h(DescMsgContext* pMsgContext)
{
	/* $85:Control DTC Setting */	
	switch (pMsgContext->reqData[1])
	{
		case 0x01: // $85 01 Service CallBack
			CanDiagAppCallBack(DESC_85_01_Control_DTC_Setting_on, pMsgContext);
			break;

		case 0x02: // $85 02 Service CallBack
			CanDiagAppCallBack(DESC_85_02_Control_DTC_Setting_off, pMsgContext);
			break;	

		default:
			DescSetNegResponse(pMsgContext, kDescNrcSubfunctionNotSupported);
			break;
	}
}

/* _END_OF_DESC_ */

