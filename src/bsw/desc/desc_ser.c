/***********************************************************************
*   Copyright (C) JWD Automotive Co., Ltd.				               * 
*	All Rights Reserved.            					               *
*   Department : iCAS SW      									       *
*   AUTHOR	   :            										   *
************************************************************************
* Object        : 
* Module        : DescService.c
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

#include <stdlib.h>
#include <string.h>

#include "desc.h"
#include "desc_ser.h"
#include "desc_mgr.h"
#include "adc_sample.h"
#include "eepRom.h"
#include "can_com.h"
#include "rte.h"


#define DESC_SERVICE_GLOBALS

/**** Definition of variables ****/
INT8U descReqData[8] = {0u};
INT8U descResData[256] = {0u};

INT8U descResSnapshotData[36] = {0u};
INT8U descResExtendedData[12] = {0u};

BOOLEAN descNmFrameRxEnable = TRUE;
BOOLEAN descNmFrameTxEnable = TRUE;
BOOLEAN descApplFrameRxEnable = TRUE;
BOOLEAN descApplFrameTxEnable = TRUE;

DescMsgContext descMsgContext = {0u};
DescDataIdentifier descDataIdentifier = {0u};

DESC_SERVICE_STAT INT32U descSecurityRandSeedLevel1 = 0u;
DESC_SERVICE_STAT INT32U descSecurityRandSeedLevel2 = 0u;

DESC_SERVICE_STAT INT32U descSendSecurityKeyLevel1 = 0u;
DESC_SERVICE_STAT INT32U descSendSecurityKeyLevel2 = 0u;

DESC_SERVICE_STAT INT16U descOemProcessTesterPresentTimeoutCnt = 0u;

DESC_SERVICE_STAT BOOLEAN descOemProcessTesterPresentFlag = FALSE;
DESC_SERVICE_STAT BOOLEAN descSecurityAccessRequestSeedLevelFlag_1 = FALSE;
DESC_SERVICE_STAT BOOLEAN descSecurityAccessRequestSeedLevelFlag_2 = FALSE;

/* $22 F187:Vehicle Manufacturer Spare Part Number Data Identifier */ 
const INT8U VehicleManufacturerSparePartNumberDataIdentifier[10] = "XAA1234567";

/* $22 F18A:System Supplier Identifier Data Identifier */
const INT8U SystemSupplierIdentifierDataIdentifier[10] = "0000000000";

/* $22 F18C:ECU Serial Number Data Identifier */
const INT8U SystemSerialNumberDataIdentifier[16] = "XXXXXXXXXXXXXXXX";

/* $22 F190:VIN Data Identifier */
const INT8U YEMA_VIN_DataIdentifier[17] = "ABCDEFG0123456789";

/* $22 F193:System Supplier ECU Hardware Version Number Data Identifier */
const INT8U SystemHardwareVersionDataIdentifier[4] = "HW02";

/* $22 F195:System Supplier ECU Software Version Number Data Identifier */
const INT8U SystemSoftwareVersionDataIdentifier[15] = "ECU_20.02.25_D1";

/* $22 F197:System Name Data Identifier */
const INT8U SystemNameDataIdentifier[10] = "iCAS000000";

/* $27 01:Security Access Seed Level 1 */
const INT8U SecurityAccessSeedLevel_1[4] = {0x08, 0x27, 0x11, 0xF0};

/* $27 02:Security Access Seed Level 1 */
const INT8U SecurityAccessKeyLevel_1[4] = {0xF0, 0x11, 0x27, 0x08};

/* $27 09:Security Access Seed Level 1 */
const INT8U SecurityAccessSeedLevel_2[4] = {0x08, 0x27, 0x11, 0xF0};

/* $27 0A:Security Access Seed Level 1 */
const INT8U SecurityAccessKeyLevel_2[4] = {0xF0, 0x11, 0x27, 0x08};

/* $27 level 1 security alogrithem mask */
const INT32U SecurityAccessMaskLevel_1 = 0x21156136;

/* $27 level 2 security alogrithem mask */
const INT32U SecurityAccessMaskLevel_2 = 0x21156138;


/**** Declaration of functions ****/
DESC_SERVICE_STAT INT32U seedToKey(INT32U seed, INT32U mask);

/* Table of service (instance) relevant information. */
/* $11:ECU Reset */
DESC_SERVICE_STAT void ApplDescEcuHardwareResetHard(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescEcuResetKeyOffOn(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescEcuSoftwareResetHard(DescMsgContext* pMsgContext);

/* $14:Fault Memory */
DESC_SERVICE_STAT void ApplDescClearDiagInfo(DescMsgContext* pMsgContext);

/* $19:Fault Memory*/
DESC_SERVICE_STAT void ApplDescReadDtcInfo_ReportNumberOfDtcByStatusMask(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescReadDtcInfo_ReportDtcByStatusMask(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescReadDtcInfo_ReportDtcSnapshotRecordByDtcNumber(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescReadDtcInfo_ReportDtcExtendedDataRecordByDtcNumber(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescReadDtcInfo_ReportSupportedDtc(DescMsgContext* pMsgContext);

/* $22:Read By Data Identifier */
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_OEM_SparePartNumber(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_SupplierInformation(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_ECU_SerialNumber(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_OEM_VIN(DescMsgContext* pMsgContext);	
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_ECU_HardwareVersionNumber(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_ECU_SoftwareVersionNumber(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_SystemNameDataNumber(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescSimulateAdcData(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_EepromData(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescReadDataByIdentifier_Pedal_Cylinder_Data(DescMsgContext* pMsgContext);

/* $23:Read Memory By Address */
DESC_SERVICE_STAT void ApplDescReadMemoryByAddress(DescMsgContext* pMsgContext);

/* $27:Security Access */
DESC_SERVICE_STAT void ApplDescOemGetLevel_1_RequestSeed(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescOemSendLevel_1_SendKey(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescOemGetLevel_2_RequestSeed(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescOemSendLevel_2_SendKey(DescMsgContext* pMsgContext);

/* $28:Communication Control  */
DESC_SERVICE_STAT void ApplDescOemCommCtrlEnableRxEnableTx(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescOemCommCtrlEnableRxDisableTx(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescOemCommCtrlDisableRxEnableTx(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescOemCommCtrlDisableRxDisableTx(DescMsgContext* pMsgContext);

/* $2A:Read Data By Periodic Indentifier */
DESC_SERVICE_STAT void ApplDesReadDataByPeriodicIndentifier_SendAtSlowRate(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDesReadDataByPeriodicIndentifier_SendAtMediumRate(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDesReadDataByPeriodicIndentifier_SendAtFastRate(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDesReadDataByPeriodicIndentifier_StopSending(DescMsgContext* pMsgContext);

/* $2C:Dynamically Define Data Indentifier */
DESC_SERVICE_STAT void ApplDescDynamicallyDefineDataIdentifier_DefineByIdentifier(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescDynamicallyDefineDataIdentifier_ClearDynamicallyDefineDataIdentifier(DescMsgContext* pMsgContext);

/* $2E:Write By Data Identifier */	  
DESC_SERVICE_STAT void ApplDescWriteByDataIdentifierOEM_VIN(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescWriteEepromDataById_6668(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescWritePedalCylinderDataById_2E01(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescWritePvTestFlagById_2E02(DescMsgContext* pMsgContext);

/* $2F:Input Output Control By Identifier */

/* $31:Service Routine Control*/
DESC_SERVICE_STAT void ApplDescRoutineControlByIdentifier_CheckProgrammingIntegrity(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescRoutineControlByIdentifier_CheckProgrammingPreCondition(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescRoutineControlByIdentifier_EraseMemory(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescRoutineControlByIdentifier_CheckProgrammingDependency(DescMsgContext* pMsgContext);

/* $34:Request Download */ 
DESC_SERVICE_STAT void ApplDescRequestDownload(DescMsgContext* pMsgContext);

/* $36:Transfer Data */ 
DESC_SERVICE_STAT void ApplDescTransferData(DescMsgContext* pMsgContext);

/* $37:Request Transfer Exit */ 
DESC_SERVICE_STAT void ApplDescRequestTransferExit(DescMsgContext* pMsgContext);	

/* $3D:Write Memory By Address */ 
DESC_SERVICE_STAT void ApplDescWriteMemoryByAddress(DescMsgContext* pMsgContext);

/* $3E:Tester Present */ 
DESC_SERVICE_STAT void ApplDescOemProcessTesterPresent(DescMsgContext* pMsgContext);	

/* $85:Control DTC Setting */
DESC_SERVICE_STAT void ApplDescControlDtcSettingEnable(DescMsgContext* pMsgContext);
DESC_SERVICE_STAT void ApplDescControlDtcSettingDisable(DescMsgContext* pMsgContext);

DESC_SERVICE_STAT DiagHandler CanDiagHandler[DESC_MAX_TYPE]=
{
	/* $10:Diagnostic Session Control. Had been handle by "CANdesc" component. */
    NULL,//DESC_10_01_DEFAULT_SESSION,
    NULL,//DESC_10_01_DEFAULT_SESSION,
    NULL,//DESC_10_03_EXTENDED_SESSION,

	/* $11:ECU Reset */
    ApplDescEcuHardwareResetHard, /* $11 01 */
    ApplDescEcuResetKeyOffOn,     /* $11 02 */
    ApplDescEcuSoftwareResetHard, /* $11 03 */    

	/* $14:Clear Diagnostic Information */
    ApplDescClearDiagInfo, 

	/* $19:Read DTC Information */
    ApplDescReadDtcInfo_ReportNumberOfDtcByStatusMask,	        /* $19 01 */	
    ApplDescReadDtcInfo_ReportDtcByStatusMask,                  /* $19 02 */
	ApplDescReadDtcInfo_ReportDtcSnapshotRecordByDtcNumber,	    /* $19 04 */	
    ApplDescReadDtcInfo_ReportDtcExtendedDataRecordByDtcNumber, /* $19 06 */
    ApplDescReadDtcInfo_ReportSupportedDtc,	                    /* $19 0A */

    /* $22:Read By Data Identifier */
	ApplDescReadByDataIdentifier_OEM_SparePartNumber,       /* $22 F187 */	
	ApplDescReadByDataIdentifier_SupplierInformation,       /* $22 F18A */
	ApplDescReadByDataIdentifier_ECU_SerialNumber,	        /* $22 F18C */
	ApplDescReadByDataIdentifier_OEM_VIN,	                /* $22 F190 */
	ApplDescReadByDataIdentifier_ECU_HardwareVersionNumber,	/* $22 F193 */
	ApplDescReadByDataIdentifier_ECU_SoftwareVersionNumber,	/* $22 F195 */
	ApplDescReadByDataIdentifier_SystemNameDataNumber,	    /* $22 F197 */
	ApplDescSimulateAdcData, 								/* $22 6666 */
	ApplDescReadByDataIdentifier_EepromData, 				/* $22 6667 */
	ApplDescReadDataByIdentifier_Pedal_Cylinder_Data,  		/* $22 2201 */

	/* $23:Read Memory By Address */
	ApplDescReadMemoryByAddress, /* $23 24 */

	/* $27:Security Access */
	ApplDescOemGetLevel_1_RequestSeed, /* $27 01 */
	ApplDescOemSendLevel_1_SendKey,    /* $27 02 */
	ApplDescOemGetLevel_2_RequestSeed, /* $27 09 */
	ApplDescOemSendLevel_2_SendKey,    /* $27 0A */

	/* $28:Communication Control */
	ApplDescOemCommCtrlEnableRxEnableTx,   /* $28 00 */
	ApplDescOemCommCtrlEnableRxDisableTx,  /* $28 01 */
	ApplDescOemCommCtrlDisableRxEnableTx,  /* $28 02 */
	ApplDescOemCommCtrlDisableRxDisableTx, /* $28 03 */ 

	/* $2A:Read Data By Periodic Indentifier */        
	ApplDesReadDataByPeriodicIndentifier_SendAtSlowRate,   /* $2A 01 */
	ApplDesReadDataByPeriodicIndentifier_SendAtMediumRate, /* $2A 02 */
	ApplDesReadDataByPeriodicIndentifier_SendAtFastRate,   /* $2A 03 */
	ApplDesReadDataByPeriodicIndentifier_StopSending,      /* $2A 04 */

	/* $2C:Dynamically Define Data Indentifier */
	ApplDescDynamicallyDefineDataIdentifier_DefineByIdentifier,                   /* $2C 01 */
	ApplDescDynamicallyDefineDataIdentifier_ClearDynamicallyDefineDataIdentifier, /* $2C 03 */
	
	/* $2E:Write By Data Identifier */
	ApplDescWriteByDataIdentifierOEM_VIN, /* $2E F190 */
	ApplDescWriteEepromDataById_6668,	  /* $2E 6668 */
	ApplDescWritePedalCylinderDataById_2E01,/* $2E 2E01 */
	ApplDescWritePvTestFlagById_2E02,		/* $2E 2E02 */

	/* $2F:Input Output Control By Identifier */	
	
    /* $31:Routine Control */	
    ApplDescRoutineControlByIdentifier_CheckProgrammingIntegrity,    /* $31 01 F0 01 */	
    ApplDescRoutineControlByIdentifier_CheckProgrammingPreCondition, /* $31 01 FF 02 */    
    ApplDescRoutineControlByIdentifier_EraseMemory,                  /* $31 01 FF 00 */
    ApplDescRoutineControlByIdentifier_CheckProgrammingDependency,	 /* $31 01 FF 01 */
    
	/* $34:Request Download */ 
    ApplDescRequestDownload,	

	/* $36:Transfer Data */ 
    ApplDescTransferData,

	/* $37:Request Transfer Exit */ 
    ApplDescRequestTransferExit,

	/* $3D:Write Memory By Address */ 
	ApplDescWriteMemoryByAddress,	

	/* $3E:Tester Present */ 
    ApplDescOemProcessTesterPresent,		

	/* $85:Control DTC Setting */
	ApplDescControlDtcSettingEnable,  /* $85 01 */
	ApplDescControlDtcSettingDisable, /* $85 02 */
};


/*****************************************************************************************
*  Name        : CanDesc_TaskEntry
*  Description : none       
*  Parameter   : none
*  Returns     : none 
*****************************************************************************************/
void CanDesc_TaskEntry(void)
{
	descMsgContext.reqData = descReqData;
	descMsgContext.resData = descResData;
	
	if (TRUE == g_descReqMsgRxFlag)
	{
		g_descReqMsgRxFlag = FALSE;
		DescServiceClassifier(&descMsgContext);	

		/* Check OBD tester present status, if long time not receive
		 * $3E service, meaning the OBD at present timeout, ECU should
		 * switch session mode to default session state. */
		descOemProcessTesterPresentTimeoutCnt++;
		if (descOemProcessTesterPresentTimeoutCnt >= 65535u)
		{
			descOemProcessTesterPresentFlag = FALSE;
			DescSetStateSession(kDescStateSessionDefault);
		}
	}
	
	return;
}

/*  ********************************************************************************
 * Function name:ApplDescEcuHardwareResetHard (Service request header:$11 $1 )
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
DESC_SERVICE_STAT void ApplDescEcuHardwareResetHard(DescMsgContext* pMsgContext)
{
#if (kDescEnableService11_HardwareReset == 1)

	if (0x02 == pMsgContext->reqDataLen)
	{
		if (0u == VCU_VehicleSpeed)
		{
			pMsgContext->resDataLen = 0x02;
			DescSetPosResponse(pMsgContext);

			/* Hardware Reset */
            // TODO:Hardware reset
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);
		}		
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
#else
	DescProcessingDone();
#endif
}

/*  ********************************************************************************
 * Function name:ApplDescEcuResetKeyOffOn (Service request header:$11 $2 )
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
DESC_SERVICE_STAT void ApplDescEcuResetKeyOffOn(DescMsgContext* pMsgContext)
{
#if (kDescEnableService11_KeyOffOnReset == 1)

	if (0x02 == pMsgContext->reqDataLen)
	{
		if (0u == VCU_VehicleSpeed)
		{
			pMsgContext->resDataLen = 0x02;
			DescSetPosResponse(pMsgContext);
			
			// TODO:Key Off On Reset  			
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);
		}		
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
#else
	DescProcessingDone();
#endif
}

/*  ********************************************************************************
 * Function name:ApplDescEcuSoftwareResetHard (Service request header:$11 $3 )
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
DESC_SERVICE_STAT void ApplDescEcuSoftwareResetHard(DescMsgContext* pMsgContext)
{
	static INT16U temp = 0u;
	
#if (kDescEnableService11_SoftwareReset == 1)

	if (0x02 == pMsgContext->reqDataLen)
	{
		if (0u == VCU_VehicleSpeed)
		{
			pMsgContext->resDataLen = 0x02;
			DescSetPosResponse(pMsgContext);

			DESC_vSWRST();
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);
		}		
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);
	}
#else
	DescProcessingDone();
#endif
}

/*  ********************************************************************************
 * Function name:ApplDescClearDiagInfo (Service request header:$14 )
* Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request byte after the Sid.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response after the Sid.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the service request length, excluding Sid byte only.
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the total service response length, excluding the Sid only.
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
DESC_SERVICE_STAT void ApplDescClearDiagInfo(DescMsgContext* pMsgContext)
{
	INT32U dtc_group_type = 0;	

    dtc_group_type = (((INT32U)pMsgContext->reqData[1]) << 16u) & 0x00FF0000; /* High byte */
    dtc_group_type |= (((INT32U)pMsgContext->reqData[2]) << 8u) & 0x0000FF00; /* Middle byte */
    dtc_group_type |= (((INT32U)pMsgContext->reqData[3])) & 0x000000FF;       /* low byte */

	if (4u == pMsgContext->reqDataLen)
	{
		if (DTC_GRP_ALL == dtc_group_type)
		{
			if (0u == EepRom_Check())
			{
				/* Clear Diagnostic Information */
				
				//EarsePageEeprom_v_g();

				pMsgContext->resDataLen = 1u;
				pMsgContext->reqData[1] = 0x00;
				pMsgContext->reqData[2] = 0x00;

				DescSetPosResponse(pMsgContext);					
			}
			else
			{
				DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);			
			}
		}
		else
		{
	        DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);			
		}
	}
	else
	{
        DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
}

/*  ********************************************************************************
 * Function name:ApplDescReadDtcInfo_ReportNumberOfDtcByStatusMask (Service request header:$19 $1 )
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
DESC_SERVICE_STAT void ApplDescReadDtcInfo_ReportNumberOfDtcByStatusMask(DescMsgContext* pMsgContext)
{
	INT16U dtc_num = 0; 
	
	if (3u == pMsgContext->reqDataLen)
	{
		if (0 != (pMsgContext->reqData[2] & DTC_STATUS_AVAILABILITY_MASK))
		{
			dtc_num = DescMgr_CanDiagGetDtcCount(pMsgContext->reqData[2]);	   	/*get stc number through dtc status*/

			pMsgContext->resDataLen = 6u;
			pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
			pMsgContext->resData[1] = pMsgContext->reqData[1];
			pMsgContext->resData[2] = DTC_STATUS_AVAILABILITY_MASK;
		    pMsgContext->resData[3] = DTC_FORMAT_INDENTIFIER_1;
		    pMsgContext->resData[4] = (INT8U)(dtc_num >> 8u);/*High byte*/
		    pMsgContext->resData[5] = (INT8U)(dtc_num & 0xff);/*Low byte*/

			CanTx_descResMsgSendEnable();				
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);				
		}	
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
}

/*  ********************************************************************************
 * Function name:ApplDescReadDtcInfo_ReportDtcByStatusMask (Service request header:$19 $2 )
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
DESC_SERVICE_STAT void ApplDescReadDtcInfo_ReportDtcByStatusMask(DescMsgContext* pMsgContext)
{
	INT8U i;
	INT16U dtc_num = 0u; 

	DESC_SERVICE_STAT INT8U descResDtcByStatusMask[DTC_CODE_MAX_NUM*4] = {0u};

	if (3u == pMsgContext->reqDataLen)
	{
		if (0 != (pMsgContext->reqData[2] & DTC_STATUS_AVAILABILITY_MASK))
		{
			dtc_num = DescMgr_CanDiagGetDtcStatusByStatusMask(descResDtcByStatusMask, pMsgContext->reqData[2]);
			pMsgContext->resDataLen = (INT8U)(3u + dtc_num * 4u);

			pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
			pMsgContext->resData[1] = pMsgContext->reqData[1];
			pMsgContext->resData[2] = DTC_STATUS_AVAILABILITY_MASK;

			for (i = 0u; i < (4 * dtc_num); i++)
			{
				pMsgContext->resData[i+3] = descResDtcByStatusMask[i];
			}

			CanTx_descResMsgSendEnable();
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);				
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}  
}

/*****************************************************************************************
*  Name        : ApplDesc_CompareDtcNumber
*  Description : none    
*  Parameter   : pMsgContext
*  Returns     : none 
*****************************************************************************************/
DESC_SERVICE_STAT BOOLEAN ApplDesc_CompareDtcNumber(DescMsgContext* pMsgContext)
{
	INT8U record_count;
    BOOLEAN NRC_occured = TRUE;

	for (record_count=0u; record_count<DTC_CODE_MAX_NUM; record_count++)
    {
        if ((dtc_code_data[record_count].dtc_bytes.dtc_high_byte == pMsgContext->reqData[2]) 
	     && (dtc_code_data[record_count].dtc_bytes.dtc_middle_byte == pMsgContext->reqData[3])
	     && (dtc_code_data[record_count].dtc_bytes.dtc_low_byte == pMsgContext->reqData[4]))
        {
            NRC_occured = FALSE;
        }
		
        if(!NRC_occured)
        {
            return TRUE;
        }
    }
	
    return FALSE;  
}

/*  ********************************************************************************
 * Function name:ApplDescReadDtcInfo_ReportDtcSnapshotRecordByDtcNumber (Service request header:$19 $4 )
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
DESC_SERVICE_STAT void ApplDescReadDtcInfo_ReportDtcSnapshotRecordByDtcNumber(DescMsgContext* pMsgContext)
{
	INT8U i;

	if (6u == pMsgContext->reqDataLen)
	{
		if (TRUE == ApplDesc_CompareDtcNumber(pMsgContext))
		{
		   	switch (pMsgContext->reqData[5])  
			{
				case 0x01:
				case 0xFF:
					DescMgr_CanDiagGetSnapshotData(descResSnapshotData, &(pMsgContext->reqData[0]));
				
				    pMsgContext->resDataLen = sizeof(descResSnapshotData) + 2u;

					pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
					pMsgContext->resData[1] = pMsgContext->reqData[1];

					for (i=0u; i<sizeof(descResSnapshotData); i++)
					{
						pMsgContext->resData[i+2] = descResSnapshotData[i];
					}
					CanTx_descResMsgSendEnable();
	
					break;
				default:
					DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);
					break;
			}		
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);			
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	} 
}

/*  ********************************************************************************
 * Function name:ApplDescReadDtcInfo_ReportDtcExtendedDataRecordByDtcNumber (Service request header:$19 $6 )
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
DESC_SERVICE_STAT void ApplDescReadDtcInfo_ReportDtcExtendedDataRecordByDtcNumber(DescMsgContext* pMsgContext)
{
	INT8U i;

    /**
     *  pMsgContext->reqData[0]-----> DTC Number High byte
     *  pMsgContext->reqData[1]-----> DTC Number Middle byte
     *  pMsgContext->reqData[2]-----> DTC Number Low byte
     *  pMsgContext->reqData[3]-----> Request extend data type
     *  All can be config in canoe or vspy
     */
	if (TRUE == ApplDesc_CompareDtcNumber(pMsgContext))
	{
	   	switch (pMsgContext->reqData[5]) 
		{
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
				DescMgr_CanDiagGetExtendData(descResExtendedData, &(pMsgContext->reqData[0])); 

				pMsgContext->resDataLen = 8u;				
				pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
				pMsgContext->resData[1] = pMsgContext->reqData[1];

				for (i = 0u; i < 6u; i++)
				{
					pMsgContext->resData[i+2] = descResExtendedData[i];
				}

				CanTx_descResMsgSendEnable();
				
			    break;
				
			case 0xFF:
			    DescMgr_CanDiagGetExtendData(descResExtendedData, &(pMsgContext->reqData[0]));    

				pMsgContext->resDataLen = sizeof(descResExtendedData) + 2u;				
				pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
				pMsgContext->resData[1] = pMsgContext->reqData[1];

				for (i = 0u; i < sizeof(descResExtendedData); i++)
				{
					pMsgContext->resData[i+2] = descResExtendedData[i];
				}
				CanTx_descResMsgSendEnable();
			
				break;
			
			default:
				DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);
				break;
		}		
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);		
	}
}

/*  ********************************************************************************
 * Function name:ApplDescReadDtcInfo_ReportSupportedDtc (Service request header:$19 $A )
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
DESC_SERVICE_STAT void ApplDescReadDtcInfo_ReportSupportedDtc(DescMsgContext* pMsgContext)
{
	INT8U i;
	INT16U dtc_num = 0u;
	DESC_SERVICE_STAT INT8U descResDtcReportStatus[DTC_CODE_MAX_NUM*4] = {0u};

	if (2u == pMsgContext->reqDataLen)
	{
		pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
		pMsgContext->resData[1] = pMsgContext->reqData[1];
		pMsgContext->resData[2] = DTC_STATUS_AVAILABILITY_MASK;	

	    dtc_num = DescMgr_CanDiagGetSupportedDtc(descResDtcReportStatus);
	    pMsgContext->resDataLen = (INT8U)(3u + dtc_num * 4u);

		for (i = 0u; i < (dtc_num * 4u); i++)
		{
			pMsgContext->resData[i+3] = descResDtcReportStatus[i];
		}
		CanTx_descResMsgSendEnable();		
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}	
}

/*  ********************************************************************************
 * Function name:ApplDescReadByDataIdentifier_OEM_SparePartNumber (Service request header:$22 $F1 $87 )
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
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_OEM_SparePartNumber(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Contains no request data */
	/* Dummy example of how to write the response data. */

	INT8U i; 

	if (0x03 == pMsgContext->reqDataLen)
	{
		if (0u == VCU_VehicleSpeed)
		{
			pMsgContext->resDataLen = sizeof(VehicleManufacturerSparePartNumberDataIdentifier) + 3u;

			pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
			pMsgContext->resData[1] = pMsgContext->reqData[1];
			pMsgContext->resData[2] = pMsgContext->reqData[2];	
			
			for (i = 0u; i < sizeof(VehicleManufacturerSparePartNumberDataIdentifier); i++)
			{
				pMsgContext->resData[i+3] = VehicleManufacturerSparePartNumberDataIdentifier[i];
			}		
			CanTx_descResMsgSendEnable();
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);					
		} 
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);	
	}
}

/*  ********************************************************************************
 * Function name:ApplDescReadByDataIdentifier_SupplierInformation (Service request header:$22 $F1 $8A )
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
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_SupplierInformation(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Contains no request data */
	/* Dummy example of how to write the response data. */

	INT8U i; 

	if (0x03 == pMsgContext->reqDataLen) 
	{ 
		if (0u == VCU_VehicleSpeed)
		{
			pMsgContext->resDataLen = sizeof(SystemSupplierIdentifierDataIdentifier) + 3u;

			pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
			pMsgContext->resData[1] = pMsgContext->reqData[1];
			pMsgContext->resData[2] = pMsgContext->reqData[2];
			
			for (i = 0u; i < sizeof(SystemSupplierIdentifierDataIdentifier); i++)
			{
				pMsgContext->resData[i+3] = SystemSupplierIdentifierDataIdentifier[i];
			}
			CanTx_descResMsgSendEnable();
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);					
		} 
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
}

/*  ********************************************************************************
 * Function name:ApplDescReadByDataIdentifier_ECU_SerialNumber (Service request header:$22 $F1 $8C )
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
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_ECU_SerialNumber(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Contains no request data */
	/* Dummy example of how to write the response data. */

	INT8U i; 

	if (0x03 == pMsgContext->reqDataLen) 
	{
		if (0u == VCU_VehicleSpeed)
		{
			pMsgContext->resDataLen = sizeof(SystemSerialNumberDataIdentifier) + 3u;

			pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
			pMsgContext->resData[1] = pMsgContext->reqData[1];
			pMsgContext->resData[2] = pMsgContext->reqData[2];
			
			for (i = 0u; i < sizeof(SystemSerialNumberDataIdentifier); i++)
			{
				pMsgContext->resData[i+3] = SystemSerialNumberDataIdentifier[i];
			}
			CanTx_descResMsgSendEnable();
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);						
		} 
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
}

/*  ********************************************************************************
 * Function name:ApplDescReadByDataIdentifier_OEM_VIN (Service request header:$22 $F1 $90 )
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
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_OEM_VIN(DescMsgContext* pMsgContext)
{
	INT8U i; 

	if (0x03 == pMsgContext->reqDataLen) 
	{ 
		if (0u == VCU_VehicleSpeed)
		{
			ReadString_v_g(descDataIdentifier.YEMA_VIN, VIN_VALUE_ADDRESS, VIN_VALUE_LENGTH);	

			pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
			pMsgContext->resData[1] = pMsgContext->reqData[1];
			pMsgContext->resData[2] = pMsgContext->reqData[2];
			
			if(0x01 == descDataIdentifier.YEMA_VIN[VIN_VALUE_LENGTH - 1])
			{
				for (i = 0u; i < (VIN_VALUE_LENGTH - 1); i++)
				{
					pMsgContext->resData[i + 3] = descDataIdentifier.YEMA_VIN[i];
				}
			}
			else
			{
				for (i = 0u; i < (VIN_VALUE_LENGTH - 1); i++)
				{
					pMsgContext->resData[i + 3] = 0xFF;
				}
			}
			
			pMsgContext->resDataLen = (VIN_VALUE_LENGTH - 1) + 3u;
			CanTx_descResMsgSendEnable();
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);						
		} 
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);	
	}
}
	
/*  ********************************************************************************
 * Function name:ApplDescReadByDataIdentifier_ECU_HardwareVersionNumber (Service request header:$22 $F1 $93 )
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
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_ECU_HardwareVersionNumber(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Contains no request data */
	/* Dummy example of how to write the response data. */

	INT8U i; 

	if (0x03 == pMsgContext->reqDataLen)
	{
		if (0u == VCU_VehicleSpeed)
		{
			pMsgContext->resDataLen = sizeof(SystemHardwareVersionDataIdentifier) + 3u;	

			pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
			pMsgContext->resData[1] = pMsgContext->reqData[1];
			pMsgContext->resData[2] = pMsgContext->reqData[2];
			
			for (i=0u; i<sizeof(SystemHardwareVersionDataIdentifier); i++)
			{
				pMsgContext->resData[i+3] = SystemHardwareVersionDataIdentifier[i];					
			}
			CanTx_descResMsgSendEnable();				
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);					
		} 
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);	
	}
}

/*  ********************************************************************************
 * Function name:ApplDescReadByDataIdentifier_ECU_SoftwareVersionNumber (Service request header:$22 $F1 $95 )
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
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_ECU_SoftwareVersionNumber(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Contains no request data */
	/* Dummy example of how to write the response data. */

	INT8U i; 
	
	if (0x03 == pMsgContext->reqDataLen) 
	{ 
		if (0u == VCU_VehicleSpeed)
		{
			pMsgContext->resDataLen = sizeof(SystemSoftwareVersionDataIdentifier) + 3u;	

			pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
			pMsgContext->resData[1] = pMsgContext->reqData[1];
			pMsgContext->resData[2] = pMsgContext->reqData[2];
			
			for (i = 0u; i < sizeof(SystemSoftwareVersionDataIdentifier); i++)
			{
				pMsgContext->resData[i+3] = SystemSoftwareVersionDataIdentifier[i];
			}
			CanTx_descResMsgSendEnable();
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);						
		} 
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);	
	}
}

/*  ********************************************************************************
 * Function name:ApplDescReadByDataIdentifier_SystemNameDataNumber (Service request header:$22 $F1 $97 )
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
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_SystemNameDataNumber(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Contains no request data */
	/* Dummy example of how to write the response data. */

	INT8U i; 

	if (0x03 == pMsgContext->reqDataLen) 
	{ 
		if (0u == VCU_VehicleSpeed)
		{
			pMsgContext->resDataLen = sizeof(SystemNameDataIdentifier) + 3u;

			pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
			pMsgContext->resData[1] = pMsgContext->reqData[1];
			pMsgContext->resData[2] = pMsgContext->reqData[2];
			
			/* First data frame */
			for (i=0u; i<sizeof(SystemNameDataIdentifier); i++)
			{
				pMsgContext->resData[i+3] = SystemNameDataIdentifier[i];
			}
			CanTx_descResMsgSendEnable();
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);						
		} 
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
}


/*  ********************************************************************************
 * Function name:ApplDescSimulateAdcData (Service request header:$22 $66 $66 )
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
DESC_SERVICE_STAT void ApplDescSimulateAdcData(DescMsgContext* pMsgContext)
{

	if ((7 == pMsgContext->reqDataLen)
		&& (pMsgContext->reqData[4] < 15))
	{
		if (0x01 == pMsgContext->reqData[3])
		{
			g_adcSimulateDataFlag = TRUE;
			
			g_adcSimulateDataIndex = pMsgContext->reqData[4];// adc channel index
			g_adcSimulateData = (pMsgContext->reqData[5] << 8) | pMsgContext->reqData[6];// adc channel data
		}
		else
		{
			g_adcSimulateDataFlag = FALSE;
			g_adcSimulateDataIndex = 0;
			g_adcSimulateData = 0;
		}
		pMsgContext->resDataLen = 0x03;		
		DescSetPosResponse(pMsgContext);
	}
	else
	{
        DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);
	}
}


/*  ********************************************************************************
 * Function name:ApplDescReadByDataIdentifier_EepromData (Service request header:$22 $66 $67 )
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
DESC_SERVICE_STAT void ApplDescReadByDataIdentifier_EepromData(DescMsgContext* pMsgContext)
{
	INT8U i;
	INT16U eepromDataLength = 0;
	INT32U eepromDataAddress = 0;
	INT8U dataBuffer[256] = {0};
	
	if (6 == pMsgContext->reqDataLen)
	{
		eepromDataAddress = ((INT32U)0xC40000) | ((INT32U)pMsgContext->reqData[3] << 8) | ((INT32U)pMsgContext->reqData[4]);
		eepromDataLength = pMsgContext->reqData[5];
		ReadString_v_g(dataBuffer, eepromDataAddress, eepromDataLength);

		pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
		pMsgContext->resData[1] = pMsgContext->reqData[1];
		pMsgContext->resData[2] = pMsgContext->reqData[2];
		pMsgContext->resDataLen = eepromDataLength + 3u;
		
		for (i = 0u; i < eepromDataLength; i++)
		{
			pMsgContext->resData[i + 3] = dataBuffer[i];
		}
		CanTx_descResMsgSendEnable();
	}
	else
	{
        DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);
	}
}

/*  ********************************************************************************
 * Function name:ApplDescReadDataByIdentifier_Pedal_Cylinder_Data (Service request header:$22 $22 $01 )
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
DESC_SERVICE_STAT void ApplDescReadDataByIdentifier_Pedal_Cylinder_Data(DescMsgContext* pMsgContext)
{
	const T_ADC *ptr_adc = Adc_SampleData();
	
	if (3 == pMsgContext->reqDataLen)
	{
		pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
		pMsgContext->resData[1] = pMsgContext->reqData[1];
		pMsgContext->resData[2] = pMsgContext->reqData[2];

		pMsgContext->resData[3] = (INT8U)(g_brakePedalInitLocationValue >> 8);
		pMsgContext->resData[4] = (INT8U)(g_brakePedalInitLocationValue);
		pMsgContext->resData[5] = (INT8U)(ptr_adc->brakePedalTrip_Adc[0] >> 8);
		pMsgContext->resData[6] = (INT8U)(ptr_adc->brakePedalTrip_Adc[0]);
		
		pMsgContext->resData[7] = (INT8U)(g_cylinderInitLocationValue >> 8);
		pMsgContext->resData[8] = (INT8U)(g_cylinderInitLocationValue);
		pMsgContext->resData[9] = (INT8U)(ptr_adc->mainCylinderTrip_Adc[0] >> 8);
		pMsgContext->resData[10] = (INT8U)(ptr_adc->mainCylinderTrip_Adc[0]);
		
		pMsgContext->resDataLen = 8u + 3u;
		
		CanTx_descResMsgSendEnable();
	}
	else
	{
        DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);
	}
}



/*  ********************************************************************************
 * Function name:ApplDescReadMemoryByAddress (Service request header:$23 $24 )
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
DESC_SERVICE_STAT void ApplDescReadMemoryByAddress(DescMsgContext* pMsgContext)
{
#if (kDescEnableService23_ReadMemoryByAddress == 1)

	if (0x08 == pMsgContext->reqDataLen)
	{
		if (0u == VCU_VehicleSpeed)
		{
			if (1) // TODO: Check request out of range
			{
				/* Check address invalid, request memory size invalid 
				 * and addressAndLengthFormatIdentifier invalid. */
			}
			else
			{
				DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);				
			}
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);		
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
#else
	DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);	

	DescProcessingDone();
#endif
}

/*  ********************************************************************************
 * Function name:ApplDescOemGetLevel_1_RequestSeed (Service request header:$27 $01 )
 * Description:Manages the security get seed function.
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDescOemGetLevel_1_RequestSeed(DescMsgContext* pMsgContext)
{
#if (kDescEnableService27_Level1 == 1)

	DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);	

	if (0x02 == pMsgContext->reqDataLen) 
	{
		if (0u == VCU_VehicleSpeed)
		{
			/* When dalay timer active receive the request, send NRC */
			descSecurityRandSeedLevel1 = rand();
		
			if (1) // TODO:Delay timer arrive
			{
				pMsgContext->resDataLen = 6u;
				pMsgContext->reqData[2] = (INT8U)((descSecurityRandSeedLevel1 & 0xFF000000) >> 24u);

				pMsgContext->resData[3] = (INT8U)((descSecurityRandSeedLevel1 & 0x00FF0000) >> 16u);
				pMsgContext->resData[4] = (INT8U)((descSecurityRandSeedLevel1 & 0x0000FF00) >> 8u);
				pMsgContext->resData[5] = (INT8U)(descSecurityRandSeedLevel1 & 0x000000FF);

				descSecurityAccessRequestSeedLevelFlag_1 = TRUE;
				
				DescSetPosResponse(pMsgContext);
			}
			else
			{
				descSecurityAccessRequestSeedLevelFlag_1 = FALSE;

				DescSetNegResponse(pMsgContext, kDescNrcTimeDelayNotExpired);				
			}
		}
		else
		{
			descSecurityAccessRequestSeedLevelFlag_1 = FALSE;

			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);				
		}
	}
	else
	{
		descSecurityAccessRequestSeedLevelFlag_1 = FALSE;

		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
#else
	DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);	

	DescProcessingDone();
#endif
}

/*  ********************************************************************************
 * Function name:ApplDescOemSendLevel_1_SendKey (Service request header:$27 $02 )
 * Description:Manages the security send key function.
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDescOemSendLevel_1_SendKey(DescMsgContext* pMsgContext)
{
#if (kDescEnableService27_Level1 == 1)

	if (0x04 == pMsgContext->reqDataLen) 
	{
		if (0u == VCU_VehicleSpeed)
		{
			/* When dalay timer active receive the request, send NRC */
			if (1) // TODO:Delay timer arrive
			{
				if (TRUE == descSecurityAccessRequestSeedLevelFlag_1)
				{
					descSendSecurityKeyLevel1 = seedToKey(descSecurityRandSeedLevel1, SecurityAccessMaskLevel_1);

					pMsgContext->resDataLen = 6u;
					pMsgContext->reqData[2] = (INT8U)((descSendSecurityKeyLevel1 & 0xFF000000) >> 24u);
					
					pMsgContext->resData[3] = (INT8U)((descSendSecurityKeyLevel1 & 0x00FF0000) >> 16u);
					pMsgContext->resData[4] = (INT8U)((descSendSecurityKeyLevel1 & 0x0000FF00) >> 8u);
					pMsgContext->resData[5] = (INT8U)(descSendSecurityKeyLevel1 & 0x000000FF);
					
					descSecurityAccessRequestSeedLevelFlag_1 = FALSE;
					
					DescSetStateSecurityAccess(kDescStateSecurityAccessLevel_1);
					
					DescSetPosResponse(pMsgContext);				
				}
				else
				{
					DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);
					
					DescSetNegResponse(pMsgContext, kDescNrcRequestSequenceError);				
				}
			}
			else
			{
				DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);
				
				DescSetNegResponse(pMsgContext, kDescNrcTimeDelayNotExpired);				
			}
		}
		else
		{
			DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);

			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);				
		}
	}
	else
	{
		DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);

		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);	
	}
#else
	DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);

	DescProcessingDone();
#endif
}

/*  ********************************************************************************
 * Function name:ApplDescOemGetLevel_2_RequestSeed (Service request header:$27 $09 )
 * Description:Manages the security get seed function.
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDescOemGetLevel_2_RequestSeed(DescMsgContext* pMsgContext)
{
#if (kDescEnableService27_Level2 == 1)

	DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);

	if (0x02 == pMsgContext->reqDataLen) 
	{
		if (0u == VCU_VehicleSpeed)
		{
			/* When dalay timer active receive the request, send NRC */
			descSecurityRandSeedLevel2 = rand();
			
			if (1) // TODO:Delay timer arrive
			{
				pMsgContext->resDataLen = 6u;
				pMsgContext->reqData[2] = (INT8U)((descSecurityRandSeedLevel2 & 0xFF000000) >> 24u);

				pMsgContext->resData[3] = (INT8U)((descSecurityRandSeedLevel2 & 0x00FF0000) >> 16u);
				pMsgContext->resData[4] = (INT8U)((descSecurityRandSeedLevel2 & 0x0000FF00) >> 8u);
				pMsgContext->resData[5] = (INT8U)(descSecurityRandSeedLevel2 & 0x000000FF);

				descSecurityAccessRequestSeedLevelFlag_2 = TRUE;
				
				DescSetPosResponse(pMsgContext);
			}
			else
			{
				descSecurityAccessRequestSeedLevelFlag_2 = FALSE;

				DescSetNegResponse(pMsgContext, kDescNrcTimeDelayNotExpired);					
			}
		}
		else
		{
			descSecurityAccessRequestSeedLevelFlag_2 = FALSE;

			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);			
		}
	}
	else
	{
		descSecurityAccessRequestSeedLevelFlag_2 = FALSE;

		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
#else
	DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);

	DescProcessingDone();
#endif
}

/*  ********************************************************************************
 * Function name:ApplDescOemSendLevel_2_SendKey (Service request header:$27 $0A )
 * Description:Manages the security send key function.
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDescOemSendLevel_2_SendKey(DescMsgContext* pMsgContext)
{
#if (kDescEnableService27_Level2 == 1)

	if (0x04 == pMsgContext->reqDataLen) 
	{
		if (0u == VCU_VehicleSpeed)
		{
			/* When dalay timer active receive the request, send NRC */
			if (1) // TODO:Delay timer arrive
			{
				if (TRUE == descSecurityAccessRequestSeedLevelFlag_2)
				{
					descSendSecurityKeyLevel2 = seedToKey(descSecurityRandSeedLevel2, SecurityAccessMaskLevel_2);

					pMsgContext->resDataLen = 6u;
					pMsgContext->reqData[2] = (INT8U)((descSendSecurityKeyLevel2 & 0xFF000000) >> 24u);
					
					pMsgContext->resData[3] = (INT8U)((descSendSecurityKeyLevel2 & 0x00FF0000) >> 16u);
					pMsgContext->resData[4] = (INT8U)((descSendSecurityKeyLevel2 & 0x0000FF00) >> 8u);
					pMsgContext->resData[5] = (INT8U)(descSendSecurityKeyLevel2 & 0x000000FF);
					
					descSecurityAccessRequestSeedLevelFlag_2 = FALSE;
					
					DescSetStateSecurityAccess(kDescStateSecurityAccessLevel_1);
					
					DescSetPosResponse(pMsgContext);				
				}
				else
				{
					DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);

					DescSetNegResponse(pMsgContext, kDescNrcRequestSequenceError);					
				}

			}
			else
			{
				DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);
	
				DescSetNegResponse(pMsgContext, kDescNrcTimeDelayNotExpired);				
			}
		}
		else
		{
			DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);

			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);			
		}
	}
	else
	{
		DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);

		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
#else
	DescSetStateSecurityAccess(kDescStateSecurityAccessLocked);

#endif
}

/*  ********************************************************************************
 * Function name:ApplDescOemCommCtrlEnableRxEnableTx (Service request header:$28 $00 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDescOemCommCtrlEnableRxEnableTx(DescMsgContext* pMsgContext)
{
#if (kDescEnableService28_RxTxAllCtrl == 1)

	if (3u == pMsgContext->reqDataLen) 
	{
		if (0u == VCU_VehicleSpeed)
		{
			switch (pMsgContext->reqData[2])
			{
				case 0x01:
					// normal communication messages
					descApplFrameRxEnable = TRUE;
					descApplFrameTxEnable = TRUE;

					pMsgContext->resDataLen = 3u;
					DescSetPosResponse(pMsgContext);
					break;

				case 0x02:
					// network management communication messages
					descNmFrameRxEnable = TRUE;
					descNmFrameTxEnable = TRUE;

					pMsgContext->resDataLen = 3u;					
					DescSetPosResponse(pMsgContext);				
					break;

				case 0x03:
					// network management communication messages and normal communication messages
					descNmFrameRxEnable = TRUE;
					descNmFrameTxEnable = TRUE;

					descApplFrameRxEnable = TRUE;
					descApplFrameTxEnable = TRUE;

					pMsgContext->resDataLen = 3u;
					DescSetPosResponse(pMsgContext);				
					break;

				default:
					DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);					
					break;
			}
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);						
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
#else
	DescProcessingDone();
#endif
}

/*  ********************************************************************************
 * Function name:ApplDescOemCommCtrlEnableRxDisableTx (Service request header:$28 $01 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDescOemCommCtrlEnableRxDisableTx(DescMsgContext* pMsgContext)
{
#if (kDescEnableService28_RxTxDivideCtrl == 1)

	if (3u == pMsgContext->reqDataLen) 
	{
		if (0u == VCU_VehicleSpeed)
		{
			switch (pMsgContext->reqData[2])
			{
				case 0x01:
					// normal communication messages
					descApplFrameRxEnable = TRUE;
					descApplFrameTxEnable = FALSE;

					pMsgContext->resDataLen = 3u;
					DescSetPosResponse(pMsgContext);
					break;

				case 0x02:
					// network management communication messages
					descNmFrameRxEnable = TRUE;
					descNmFrameTxEnable = FALSE;					

					pMsgContext->resDataLen = 3u;
					DescSetPosResponse(pMsgContext);				
					break;

				case 0x03:
					// network management communication messages and normal communication messages
					descNmFrameRxEnable = TRUE;
					descNmFrameTxEnable = FALSE;						
					
					descApplFrameRxEnable = TRUE;
					descApplFrameTxEnable = FALSE;

					pMsgContext->resDataLen = 3u;					
					DescSetPosResponse(pMsgContext);				
					break;

				default:
					DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);						
					break;
			}
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);						
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);	
	}
#else
	DescProcessingDone();
#endif
}

/*  ********************************************************************************
 * Function name:ApplDescOemCommCtrlDisableRxEnableTx (Service request header:$28 $02 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDescOemCommCtrlDisableRxEnableTx(DescMsgContext* pMsgContext)
{
#if (kDescEnableService28_RxTxDivideCtrl == 1)

	if (3u == pMsgContext->reqDataLen) 
	{
		if (0u == VCU_VehicleSpeed)
		{
			switch (pMsgContext->reqData[2])
			{
				case 0x01:
					// normal communication messages
					descApplFrameRxEnable = FALSE;
					descApplFrameTxEnable = TRUE;

					pMsgContext->resDataLen = 3u;
					DescSetPosResponse(pMsgContext);					
					break;

				case 0x02:
					// network management communication messages
					descNmFrameRxEnable = FALSE;
					descNmFrameTxEnable = TRUE;

					pMsgContext->resDataLen = 3u;					
					DescSetPosResponse(pMsgContext);				
					break;

				case 0x03:
					// network management communication messages and normal communication messages
					descNmFrameRxEnable = FALSE;
					descNmFrameTxEnable = TRUE;					

					descApplFrameRxEnable = FALSE;
					descApplFrameTxEnable = TRUE;

					pMsgContext->resDataLen = 3u;
					DescSetPosResponse(pMsgContext);					
					break;

				default:
					DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);					
					break;
			}
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);					
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
#else
	DescProcessingDone();
#endif
}

/*  ********************************************************************************
 * Function name:ApplDescOemCommCtrlDisableRxDisableTx (Service request header:$28 $03 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDescOemCommCtrlDisableRxDisableTx(DescMsgContext* pMsgContext)
{
#if (kDescEnableService28_RxTxAllCtrl == 1)

	if (3u == pMsgContext->reqDataLen) 
	{
		if (0u == VCU_VehicleSpeed)
		{
			switch (pMsgContext->reqData[2])
			{
				case 0x01:
					// normal communication messages
					descApplFrameRxEnable = FALSE;
					descApplFrameTxEnable = FALSE;

					pMsgContext->resDataLen = 3u;
					DescSetPosResponse(pMsgContext);					
					break;

				case 0x02:
					// network management communication messages
					descNmFrameRxEnable = FALSE;
					descNmFrameTxEnable = FALSE;

					pMsgContext->resDataLen = 3u;					
					DescSetPosResponse(pMsgContext);				
					break;

				case 0x03:
					// network management communication messages and normal communication messages
					descNmFrameRxEnable = FALSE;
					descNmFrameTxEnable = FALSE;
					
					descApplFrameRxEnable = FALSE;
					descApplFrameTxEnable = FALSE;

					pMsgContext->resDataLen = 3u;
					DescSetPosResponse(pMsgContext);					
					break;

				default:
					DescSetNegResponse(pMsgContext, kDescNrcRequestOutOfRange);						
					break;
			}
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);						
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);		
	}
#else
	DescProcessingDone();
#endif
}

/*  ********************************************************************************
 * Function name:ApplDesReadDataByPeriodicIndentifier_SendAtSlowRate (Service request header:$2A $01 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDesReadDataByPeriodicIndentifier_SendAtSlowRate(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */
	DescClearResData(pMsgContext);
	DescProcessingDone();	
}

/*  ********************************************************************************
 * Function name:ApplDesReadDataByPeriodicIndentifier_SendAtMediumRate (Service request header:$2A $02 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDesReadDataByPeriodicIndentifier_SendAtMediumRate(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */
	DescClearResData(pMsgContext);
	DescProcessingDone();	
}

/*  ********************************************************************************
 * Function name:ApplDesReadDataByPeriodicIndentifier_SendAtFastRate (Service request header:$2A $03 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDesReadDataByPeriodicIndentifier_SendAtFastRate(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */
	DescClearResData(pMsgContext);
	DescProcessingDone();
}

/*  ********************************************************************************
 * Function name:ApplDesReadDataByPeriodicIndentifier_StopSending (Service request header:$2A $04 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDesReadDataByPeriodicIndentifier_StopSending(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */
	DescClearResData(pMsgContext);
	DescProcessingDone();
}

/*  ********************************************************************************
 * Function name:ApplDescDynamicallyDefineDataIdentifier_DefineByIdentifier (Service request header:$2C $01 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDescDynamicallyDefineDataIdentifier_DefineByIdentifier(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */
	DescClearResData(pMsgContext);
	DescProcessingDone();		
}

/*  ********************************************************************************
 * Function name:ApplDescDynamicallyDefineDataIdentifier_ClearDynamicallyDefineDataIdentifier (Service request header:$2C $03 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext:
 *       - Contains all request properties.
 *       - Access type: read/write
 * Particularitie(s) and limitation(s): none
 ********************************************************************************  */
DESC_SERVICE_STAT void ApplDescDynamicallyDefineDataIdentifier_ClearDynamicallyDefineDataIdentifier(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */
	DescClearResData(pMsgContext);
	DescProcessingDone();		
}

/*  ********************************************************************************
 * Function name:ApplDescWriteByDataIdentifierOEM_VIN (Service request header:$2E $F1 $90 )
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
DESC_SERVICE_STAT void ApplDescWriteByDataIdentifierOEM_VIN(DescMsgContext* pMsgContext)
{
	INT8U i = 0;
	if (20u == pMsgContext->reqDataLen)
	{
		if (0u == VCU_VehicleSpeed)
		{
			for(i = 0; i < 17; i++)
			{
				descDataIdentifier.YEMA_VIN[i] = pMsgContext->reqData[i + 3];
			}
			descDataIdentifier.YEMA_VIN[17] = 0x01; //write flag

			CopyString_v_g(descDataIdentifier.YEMA_VIN, VIN_VALUE_ADDRESS, VIN_VALUE_LENGTH, 1);
			
			pMsgContext->resDataLen = 0x03;
			DescSetPosResponse(pMsgContext);
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);
	}
}


/*  ********************************************************************************
 * Function name:ApplDescWriteEepromDataById_6668 (Service request header:$2E $66 $68 )
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
DESC_SERVICE_STAT void ApplDescWriteEepromDataById_6668(DescMsgContext* pMsgContext)
{
	INT8U i = 0;
	INT32U writeDataAddress = 0;
	INT16U writeDataLength = 0;
	INT8U writeDataBuffer[50] = 0;
	
	if (7u == pMsgContext->reqDataLen)
	{
		writeDataAddress = (INT32U)0xC40000 | ((INT32U)pMsgContext->reqData[3]) << 8 | (INT32U)pMsgContext->reqData[4];
		writeDataLength = pMsgContext->reqData[5];
		for(i = 0; i < sizeof(writeDataBuffer); i++)
		{
			writeDataBuffer[i] = pMsgContext->reqData[6];
		}

		CopyString_v_g(writeDataBuffer, writeDataAddress, writeDataLength, 1);
		pMsgContext->resDataLen = 3u;
		DescSetPosResponse(pMsgContext);
	}
	else
	{
        DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);
	}
}


DESC_SERVICE_STAT void ApplDescWritePedalCylinderDataById_2E01(DescMsgContext* pMsgContext)
{
	if (7u == pMsgContext->reqDataLen)
	{
		descDataIdentifier.pedalCylinderInitValue[0] = pMsgContext->reqData[3];
		descDataIdentifier.pedalCylinderInitValue[1] = pMsgContext->reqData[4];
		descDataIdentifier.pedalCylinderInitValue[2] = pMsgContext->reqData[5];
		descDataIdentifier.pedalCylinderInitValue[3] = pMsgContext->reqData[6];
		descDataIdentifier.pedalCylinderInitValue[4] = 0x01;
		CopyString_v_g(descDataIdentifier.pedalCylinderInitValue, PEDAL_CYLINDER_INIT_LOCATION_VALUE_ADDRESS, PEDAL_CYLINDER_INIT_LOCATION_VALUE_LENGTH, 1);
		
		g_brakePedalInitLocationValue = ((INT16U)pMsgContext->reqData[3]) << 8 | (INT16U)pMsgContext->reqData[4];
		g_cylinderInitLocationValue = ((INT16U)pMsgContext->reqData[5]) << 8 | (INT16U)pMsgContext->reqData[6];
	
		pMsgContext->resDataLen = 3u;
		DescSetPosResponse(pMsgContext);
	}
	else
	{
        DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);
	}
}



DESC_SERVICE_STAT void ApplDescWritePvTestFlagById_2E02(DescMsgContext* pMsgContext)
{
	if (4u == pMsgContext->reqDataLen)
	{
		if(0x01 == pMsgContext->reqData[3])
		{
			g_pvTestFlag = TRUE;
		}
		else
		{
			g_pvTestFlag = FALSE;
		}	
		
		pMsgContext->resDataLen = 3u;
		DescSetPosResponse(pMsgContext);
	}
	else
	{
        DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);
	}
}

/*  ********************************************************************************
 * Function name:ApplDescRoutineControlByIdentifier_CheckProgrammingIntegrity (Service request header:$31 $01 $F0 $01 )
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
DESC_SERVICE_STAT void ApplDescRoutineControlByIdentifier_CheckProgrammingIntegrity(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */	

	/* Function implementation in bootloader */
	DescClearResData(pMsgContext);
	DescProcessingDone();
}

/*  ********************************************************************************
 * Function name:ApplDescRoutineControlByIdentifier_CheckProgrammingPreCondition (Service request header:$31 $01 $FF $02 )
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
DESC_SERVICE_STAT void ApplDescRoutineControlByIdentifier_CheckProgrammingPreCondition(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */	
	DescClearResData(pMsgContext);

	/* Check programming condition, if programming condition is OK,
	 * the routine status record is correctResult(0x00), else
	 * is incorrectResult(0x01). */
	if (0u == VCU_VehicleSpeed)
	{
		pMsgContext->resData[4] = 0x00;
	}
	else
	{
		pMsgContext->resData[4] = 0x01;
	}

	pMsgContext->resData[3] = pMsgContext->reqData[3];
	
	pMsgContext->resDataLen = 0x05;
	DescSetPosResponse(pMsgContext);
}

/*  ********************************************************************************
 * Function name:ApplDescRoutineControlByIdentifier_EraseMemory (Service request header:$31 $01 $FF $00 )
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
DESC_SERVICE_STAT void ApplDescRoutineControlByIdentifier_EraseMemory(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */

	/* Function implementation in bootloader */
	DescClearResData(pMsgContext);
	DescProcessingDone();
}

/*  ********************************************************************************
 * Function name:ApplDescRoutineControlByIdentifier_CheckProgrammingDependency (Service request header:$31 $01 $FF $01 )
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
DESC_SERVICE_STAT void ApplDescRoutineControlByIdentifier_CheckProgrammingDependency(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */

	/* Function implementation in bootloader */
	DescClearResData(pMsgContext);
	DescProcessingDone();
}

/*  ********************************************************************************
 * Function name:ApplDescRequestDownload (Service request header:$34 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request byte after the Sid.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response after the Sid.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the service request length, excluding Sid byte only.
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the total service response length, excluding the Sid only.
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
DESC_SERVICE_STAT void ApplDescRequestDownload(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */
	DescClearResData(pMsgContext);
	DescProcessingDone();
}

/*  ********************************************************************************
 * Function name:ApplDescTransferData (Service request header:$36 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request byte after the Sid.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response after the Sid.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the service request length, excluding Sid byte only.
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the total service response length, excluding the Sid only.
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
DESC_SERVICE_STAT void ApplDescTransferData(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */
	DescClearResData(pMsgContext);
	DescProcessingDone();
}

/*  ********************************************************************************
 * Function name:ApplDescRequestTransferExit (Service request header:$37 )
 * Description: not available 
 * Returns:  nothing
 * Parameter(s):
 *   - pMsgContext->reqData:
 *       - Points to the first service request byte after the Sid.
 *       - Access type: read
 *   - pMsgContext->resData:
 *       - Points to the first writeable byte for the service response after the Sid.
 *       - Access type: read/write
 *   - pMsgContext->reqDataLen:
 *       - Contains the service request length, excluding Sid byte only.
 *       - Access type: read
 *   - pMsgContext->resDataLen:
 *       - Must be initialized with the total service response length, excluding the Sid only.
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
DESC_SERVICE_STAT void ApplDescRequestTransferExit(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */
	DescClearResData(pMsgContext);
	DescProcessingDone();
}

/***********************************************************************
*  Name        : ApplDescWriteMemoryByAddress
*  Description : Service request header:$3D         
*  Parameter   : pMsgContext
*  Returns     : none 
***********************************************************************/
DESC_SERVICE_STAT void ApplDescWriteMemoryByAddress(DescMsgContext* pMsgContext)
{
	/* <<TBD>> Remove this comment once you have completely implemented this function!!! */
	/* Dummy example how to access the request data. */
	/* Assumming the FIRST DATA byte contains important data which has to be less than a constant value. */
	DescClearResData(pMsgContext);
	DescProcessingDone();		
}

/***********************************************************************
*  Name        : ApplDescOemProcessTesterPresent
*  Description : Service request header:$3E         
*  Parameter   : pMsgContext
*  Returns     : none 
***********************************************************************/
DESC_SERVICE_STAT void ApplDescOemProcessTesterPresent(DescMsgContext* pMsgContext)
{
	DescClearResData(pMsgContext);

	if (2u == pMsgContext->reqDataLen) 
	{
		if (0x00 == pMsgContext->reqData[1])
		{
			descOemProcessTesterPresentFlag = TRUE;
			descOemProcessTesterPresentTimeoutCnt = 0u;

			pMsgContext->resDataLen = 0x02;
			DescSetPosResponse(pMsgContext);
		}
		else if (0x80 == pMsgContext->reqData[1])
		{
			descOemProcessTesterPresentFlag = TRUE;
			descOemProcessTesterPresentTimeoutCnt = 0u;

			/* 0x80 not need to response */

		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcSubfunctionNotSupported);				
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);			
	}
}

/*  ********************************************************************************
 * Function name:ApplDescControlDtcSettingEnable (Service request header:$85 $1 )
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
DESC_SERVICE_STAT void ApplDescControlDtcSettingEnable(DescMsgContext* pMsgContext)
{

	if (2u == pMsgContext->reqDataLen)
	{
		if (0u == VCU_VehicleSpeed)
		{
			DescMgr_CanDiagDtcControlSettingOn();	
	
			pMsgContext->resDataLen = 0x02;
			DescSetPosResponse(pMsgContext);
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);		
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);			
	}
}

/*  ********************************************************************************
 * Function name:ApplDescControlDtcSettingDisable (Service request header:$85 $2 )
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
DESC_SERVICE_STAT void ApplDescControlDtcSettingDisable(DescMsgContext* pMsgContext)
{

	if (2u == pMsgContext->reqDataLen)
	{
		if (0u == VCU_VehicleSpeed)
		{
			DescMgr_CanDiagDtcControlSettingOff();	

			pMsgContext->resDataLen = 0x02;
			DescSetPosResponse(pMsgContext);
		}
		else
		{
			DescSetNegResponse(pMsgContext, kDescNrcConditionsNotCorrect);			
		}
	}
	else
	{
		DescSetNegResponse(pMsgContext, kDescNrcInvalidFormat);			
	}
}

/***********************************************************************
*  Name        : CanDiagAppCallBack
*  Description : call back all service rountine         
*  Parameter   : pMsgContext
*  Returns     : none 
***********************************************************************/
void CanDiagAppCallBack(APPL_DESC_SERVICE_ID_TYPE MsgContextId, DescMsgContext* pMsgContext)
{
	DescClearResData(pMsgContext);

    if (MsgContextId < DESC_MAX_TYPE)
    {
        if (CanDiagHandler[MsgContextId] != NULL)
        {   
            CanDiagHandler[MsgContextId](pMsgContext);
        }        
    }
}

/***********************************************************************
*  Name        : DescClearResData
*  Description : none        
*  Parameter   : pMsgContext
*  Returns     : none 
***********************************************************************/
void DescClearResData(DescMsgContext* pMsgContext)
{
	INT16U i;

	for (i = 0u; i < pMsgContext->resDataLen; i++)
	{
		pMsgContext->resData[i] = 0u;
	}	
}

/***********************************************************************
*  Name        : DescSetNegResponse
*  Description : none        
*  Parameter   : pMsgContext
*  Returns     : none 
***********************************************************************/
void DescSetNegResponse(DescMsgContext* pMsgContext, DescNegResCode errorCode)
{
	pMsgContext->resDataLen = 0x03;
	pMsgContext->resData[0] = 0x7F;
	pMsgContext->resData[1] = pMsgContext->reqData[0];
	pMsgContext->resData[2] = errorCode;
	pMsgContext->resData[3] = 0x00;
	pMsgContext->resData[4] = 0x00;
	pMsgContext->resData[5]	= 0x00;
	pMsgContext->resData[6] = 0x00;

	CanTx_descResMsgSendEnable();
}

/***********************************************************************
*  Name        : DescSetPosResponse
*  Description : none        
*  Parameter   : pMsgContext
*  Returns     : none 
***********************************************************************/
void DescSetPosResponse(DescMsgContext* pMsgContext)
{
	pMsgContext->resData[0] = pMsgContext->reqData[0] + 0x40;
	pMsgContext->resData[1] = pMsgContext->reqData[1];
	pMsgContext->resData[2] = pMsgContext->reqData[2];

	CanTx_descResMsgSendEnable();
}

/***********************************************************************
*  Name        : DescProcessingDone
*  Description : none        
*  Parameter   : pMsgContext
*  Returns     : none 
***********************************************************************/
void DescProcessingDone(void)
{
	g_descReqMsgRxFlag = FALSE;		
}

/***********************************************************************
*  Name        : seedToKey
*  Description : none        
*  Parameter   : pMsgContext
*  Returns     : none 
***********************************************************************/
DESC_SERVICE_STAT INT32U seedToKey(INT32U seed, INT32U mask)
{
	INT8U i;
	INT32U key = 0u;

	if (seed != 0u)
	{
		for (i=0u; i<35u; i++)
		{
			if (seed & 0x80000000)
			{
				seed = seed << 1u;
				seed = seed ^ mask;
			}
			else
			{
				seed = seed << 1u;
			}
		}
	}
	else
	{
		key = seed;
	}

	return key;
}

/* _END_OF_DESC_SERVICE_ */

