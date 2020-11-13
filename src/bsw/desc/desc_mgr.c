/***********************************************************************
*   Copyright (C) JWD Automotive Co., Ltd.				               * 
*	All Rights Reserved.            					               *
*   Department : iCAS SW      									       *
*   AUTHOR	   :            										   *
************************************************************************
* Object        : 
* Module        : DescDtcMgr.c
* Instance      : 
* Description   : UDS
*-----------------------------------------------------------------------
* Version: v0.1
* Date   : Dec 27,2019 
* Author : Gao zehngzhong
***********************************************************************/
/*-History--------------------------------------------------------------
* Version    Date           Name            Changes and comments
------------------------------------------------------------------------
* 0.1	     Dec 27,2019    Gao Zhengzhong  Initial version
*=====================================================================*/

#define DESC_MGR_GLOBALS

#include <string.h>

#include "desc_mgr.h"
#include "desc_ser.h"
#include "EEPROM_Cfg.h"
#include "adc_sample.h"
#include "eepRom.h"

/**** Definition of variables ****/
DESC_MGR_STAT INT16U can_diag_init_delay_timer = 0u;
DESC_MGR_STAT INT16U can_diag_ign_delay_timer = 0u;
DESC_MGR_STAT INT16U can_diag_init_nm_delay_timer = 0u;
DESC_MGR_STAT INT16U can_diag_bat_volt_recover_delay_timer = 0u;

DESC_MGR_STAT BOOLEAN dtc_control_setting_on_off = TRUE;
DESC_MGR_STAT BOOLEAN bDtcDataNeedSendToEep = FALSE;
DESC_MGR_STAT DTC_INIT_STEP_TYPE dtc_eeprom_init_step = DTC_INIT_STEP_START;
DESC_MGR_STAT INT8U dtc_status_svc14_clear_counter = DTC_STATUS_SVC14_CLEAR_FINISHED;

DESC_MGR_STAT INT8U dtc_occurance_count[DTC_CODE_MAX_NUM] = {0u};
DESC_MGR_STAT INT8U dtc_aging_count[DTC_CODE_MAX_NUM] = {0u};
DESC_MGR_STAT INT8U dtc_pending_count[DTC_CODE_MAX_NUM] = {0u}; 
DESC_MGR_STAT INT8U dtc_aged_count[DTC_CODE_MAX_NUM] = {0u};

DESC_MGR_STAT INT8U dtc_record_enable_mask = DTC_RECORD_ENABLE_NONE;
DESC_MGR_STAT INT32U delta_trip_count = 0x00000000;

DESC_MGR_STAT BOOLEAN bSetClearHistoryFaultCntFlag[DTC_CODE_MAX_NUM] = {FALSE}; 
DESC_MGR_STAT BOOLEAN bSetClearHistoryPendingCntFlag[DTC_CODE_MAX_NUM] = {FALSE}; 
DESC_MGR_STAT BOOLEAN bSetIncreaseHistoryPendingCntFlag[DTC_CODE_MAX_NUM] = {FALSE}; 

/* DTC status mask statusOfDTC */
DESC_MGR_STAT DTC_RECORD_TYPE dtc_current_status_record[DTC_CODE_MAX_NUM] = {0x00};

/* DTC create count Fault occurrence counter */
DESC_MGR_STAT DTC_RECORD_CNT_TYPE dtc_create_record_cnt[DTC_CODE_MAX_NUM] = {0u};

/* DTC create and recover time count */
DESC_MGR_STAT DTC_RECORD_TIME_CNT_TYPE dtc_status_time_record_cnt[DTC_CODE_MAX_NUM] = {0u}; 

/* DTC snapshot data */
DESC_MGR_STAT DTC_SNAPSHOT_DATA_RECORD dtc_snapshot_data_record[DTC_CODE_MAX_NUM] = {0u};

/* DTC Number */
DTC_CODE_DATA_TYPE dtc_code_data[DTC_CODE_MAX_NUM] = 
{
    {0xC0, 0x73, 0x00}, /* can bus off */
    {0x90, 0x00, 0x16}, /* power voltage low */
    {0x90, 0x00, 0x17}, /* power voltage high */ 

	{0xD0, 0x02, 0x87}, /* 0x118 message lost */
	{0xD2, 0x01, 0x87}, /* 0x119 message lost */
	{0xD4, 0x00, 0x87}, /* 0x126 meaasge lost */
	{0xD4, 0x02, 0x87}, /* 0x130 message lost */
	{0xD4, 0x03, 0x87}, /* 0x132 message lost */
	{0xD4, 0x04, 0x87}, /* 0x140 message lost */
	{0xD4, 0x0A, 0x87}, /* 0x148 message lost */
	{0xD4, 0x0B, 0x87}, /* 0x155 message lost */
	{0xD6, 0x00, 0x87}, /* 0x16E message lost */
	{0xE0, 0x03, 0x87}, /* 0x16F message lost */
	{0xE0, 0x04, 0x87}, /* 0x278 message lost */

	{0xD0, 0x02, 0x83}, /* 0x118 message checksum or rolling count error */
	{0xD2, 0x01, 0x83}, /* 0x119 message checksum or rolling count error */
	{0xD4, 0x00, 0x83}, /* 0x126 meaasge checksum or rolling count error */
	{0xD4, 0x02, 0x83}, /* 0x130 message checksum or rolling count error */
	{0xD4, 0x03, 0x83}, /* 0x132 message checksum or rolling count error */
	{0xD4, 0x04, 0x83}, /* 0x140 message checksum or rolling count error */
	{0xD4, 0x0A, 0x83}, /* 0x148 message checksum or rolling count error */
	{0xD4, 0x0B, 0x83}, /* 0x155 message checksum or rolling count error */
	{0xD6, 0x00, 0x83}, /* 0x16E message checksum or rolling count error */
	{0xE0, 0x03, 0x83}, /* 0x16F message checksum or rolling count error */
	{0xE0, 0x04, 0x83}, /* 0x278 message checksum or rolling count error */	
	
	{0x60, 0x04, 0x83}, /* Test error */
};

/**** Declaration of functions ****/
DESC_MGR_STAT void DescMgr_ReadDtcFromEEP(void);
DESC_MGR_STAT void DescMgr_WriteAllDtcToEEP(void);
DESC_MGR_STAT void DescMgr_WriteExtendDataToEEP(INT8U dtc_code);
DESC_MGR_STAT void DescMgr_WriteSnapshotDataToEEP(INT8U dtc_code);
DESC_MGR_STAT void DescMgr_ExtendDataReport(INT8U dtc_code);
DESC_MGR_STAT void DescMgr_SnapshotDataReport(INT8U dtc_code);
DESC_MGR_STAT void DescMgr_CanDigDtcRecordEnableAdjust(void);
DESC_MGR_STAT void DescMgr_InitDtcCreateRecoverCounterAndStatus(void);
DESC_MGR_STAT void DescMgr_SetDeltaTripCounter(INT8U dtc_code, BOOLEAN delta_Trip_flag);
DESC_MGR_STAT void DescMgr_ReturnSnapshotData(INT8U *res_data, DTC_CODE_NUM_TYPE dtc_number);
DESC_MGR_STAT void DescMgr_ReturnSnapshotDataDefault(INT8U *res_data, DTC_CODE_NUM_TYPE dtc_number);
DESC_MGR_STAT void DescMgr_ReturnExtendData(INT8U *res_data, DTC_CODE_NUM_TYPE dtc_number, INT8U RecordNum);
DESC_MGR_STAT void DescMgr_ReturnExtendDataDefault(INT8U *res_data, DTC_CODE_NUM_TYPE dtc_number, INT8U RecordNum);

DESC_MGR_STAT BOOLEAN DescMgr_CanDiagDtcStatusServiceClearAll(void);
DESC_MGR_STAT BOOLEAN DescMgr_CanDiagDtcStatusServiceClearFinished(void);

#define DELAY_1s 10
#define POWER_9V_VOLTAGE 9000
#define POWER_9_5V_VOLTAGE 9500


/***********************************************************************
* FUNCTION    : CanDescDtc_vInit
* DESCRIPTION : 
* INPUTS      : none
* RETURN      : none
***********************************************************************/
void CanDescDtc_vInit(void)
{
	/* control dtc setting to on in default */
    dtc_control_setting_on_off = TRUE; 
    dtc_eeprom_init_step = DTC_INIT_STEP_START;
    dtc_status_svc14_clear_counter = DTC_STATUS_SVC14_CLEAR_FINISHED;

    bDtcDataNeedSendToEep = FALSE;
    
    can_diag_init_delay_timer = CAN_DIAG_INIT_DELAY_TIME;
    
    can_diag_init_nm_delay_timer = CAN_DIAG_NM_INIT_DELAY_TIME;
    can_diag_ign_delay_timer = CAN_DIAG_NM_IGN_ON_DELAY_TIME;
    can_diag_bat_volt_recover_delay_timer = CAN_DIAG_BAT_VOLT_RECOVER_DELAY_TIMEOUT;    

    /* init to disable all dtc record */
    dtc_record_enable_mask = DTC_RECORD_ENABLE_NONE;

    /* Adjust dtc record enable mask */
	
#ifdef DTC_AGING_COUNTER
	memset(bSetClearHistoryFaultCntFlag, TRUE, DTC_CODE_MAX_NUM);
#endif

    //EEP_Read_Status_Before_S3_Monitor();
}

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagDtcDeInit
* DESCRIPTION : 
* INPUTS      : none
* RETURN      : none
***********************************************************************/
void DescMgr_CanDiagDtcDeInit(void)
{
	return;
}

/***********************************************************************
* FUNCTION    : can_diag_dtc_eeprom_handle
* DESCRIPTION : Initialize all the dtc eeprom data at one time will lead to other can function delay,
* such as nm or app cycle msg transmition. So we separate the init action of dtc eeprom.
* INPUTS      : none
* RETURN      : none
***********************************************************************/
DESC_MGR_STAT void DescMgr_CanDiagDtcEepromHandle(void)
{
	DESC_MGR_STAT INT8U write_delay = 0u;

	switch (dtc_eeprom_init_step)
	{
		case DTC_INIT_STEP_START:
			dtc_eeprom_init_step = DTC_INIT_STEP_READ_DTC_ST;
			break;
			
		case DTC_INIT_STEP_READ_DTC_ST:
			DescMgr_ReadDtcFromEEP();
			dtc_eeprom_init_step = DTC_INIT_STEP_CALCULATE_DTC_ST;
			break;
			
		case DTC_INIT_STEP_CALCULATE_DTC_ST:
			DescMgr_InitDtcCreateRecoverCounterAndStatus();
			dtc_eeprom_init_step = DTC_INIT_STEP_WRITE_EEPROM_ST;
			break;
			
		case DTC_INIT_STEP_WRITE_EEPROM_ST:
            dtc_eeprom_init_step = DTC_INIT_STEP_FINISHED;
            dtc_record_enable_mask |= DTC_RECORD_ENALBE_BDY; /* enable body diag */			
			break;

		case DTC_INIT_STEP_FINISHED:
			if (TRUE == bDtcDataNeedSendToEep)
			{
				write_delay++;
				if (write_delay >= EEP_WRITE_FRE) /* write eep frequence 1s once */
				{
					write_delay = 0u;
					bDtcDataNeedSendToEep = FALSE; // EED_nDTCRecord;

					DescMgr_WriteAllDtcToEEP();
				}             
			}			
			break;

		default:
			break;
	}
}

/***********************************************************************
* FUNCTION    : DescMgr_ReadDtcFromEEP
* DESCRIPTION : None
* INPUTS      : None
* RETURN      : None
***********************************************************************/
DESC_MGR_STAT void DescMgr_ReadDtcFromEEP(void)
{
	INT8U i;

	for (i=0; i<DTC_CODE_MAX_NUM; i++)
	{
		ReadString_v_g((INT8U *)&dtc_current_status_record[i], DTC_EEPROM_STATUS_BYTE_START_ADDRESS+(i * 16u), sizeof(dtc_current_status_record[i]));
		ReadString_v_g((INT8U *)&dtc_snapshot_data_record[i], DTC_EEPROM_SNAPSHOT_DATA_START_ADDRESS+(i * 16u), sizeof(dtc_snapshot_data_record[i]));

#if 0
		dtc_current_status_record[i].dtc_status.status_byte
		dtc_occurance_count[i] = EED_CAN_OCCR_CNT_0[i];
		dtc_pending_count[i] = EED_CAN_PNDING_CNT_0[i];
		dtc_aging_count[i] = EED_CAN_AGING_CNT_0[i];
		dtc_aged_count[i] = EED_CAN_AGED_CNT_0[i];	
#endif
	}
}

/***********************************************************************
* FUNCTION    : DescMgr_WriteAllDtcToEEP
* DESCRIPTION : None
* INPUTS      : None
* RETURN      : None
***********************************************************************/
DESC_MGR_STAT void DescMgr_WriteAllDtcToEEP(void)
{
	INT8U i;	

	for (i=0; i<DTC_CODE_MAX_NUM; i++)
	{
		CopyString_v_g(&dtc_current_status_record[i], DTC_EEPROM_STATUS_BYTE_START_ADDRESS + (i * 16u), sizeof(dtc_current_status_record[i]), 1);
		CopyString_v_g(&dtc_snapshot_data_record[i], DTC_EEPROM_SNAPSHOT_DATA_START_ADDRESS + (i * 16u), sizeof(dtc_snapshot_data_record[i]), 1);		

		// TODO:Write extend data to EEPROM
	}	
}

/***********************************************************************
* FUNCTION    : DescMgr_WriteExtendDataToEEP
* DESCRIPTION : None
* INPUTS      : None
* RETURN      : None
***********************************************************************/
DESC_MGR_STAT void DescMgr_WriteExtendDataToEEP(INT8U dtc_code)
{
	// TODO:Write extend data to EEPROM
}

/***********************************************************************
* FUNCTION    : DescMgr_WriteSnapshotDataToEEP
* DESCRIPTION : None
* INPUTS      : INT8U dtc_code
* RETURN      : None
***********************************************************************/
DESC_MGR_STAT void DescMgr_WriteSnapshotDataToEEP(INT8U dtc_code)
{
	INT16U snapshotDataLength = 0;
	INT32U snapshotDataAddress = 0;
	
	snapshotDataLength = sizeof(DTC_SNAPSHOT_DATA_RECORD) + 2;
	snapshotDataAddress = DTC_EEPROM_SNAPSHOT_DATA_START_ADDRESS + (dtc_code * snapshotDataLength);
	CopyString_v_g(&dtc_snapshot_data_record[dtc_code], snapshotDataAddress, snapshotDataLength, 1);
}

/***********************************************************************
* FUNCTION    : Init_DTC_Creat_Recover_Counter_And_Status
* DESCRIPTION : 
* INPUTS      : none
* RETURN      : none
***********************************************************************/
DESC_MGR_STAT void DescMgr_InitDtcCreateRecoverCounterAndStatus(void)
{
    INT8U i = 0u;

    for (i=0u; i<DTC_CODE_MAX_NUM; i++)
    {
        dtc_status_time_record_cnt[i].dtc_create_time_cnt = 0u;
        dtc_status_time_record_cnt[i].dtc_recover_time_cnt = 0u;

        DescMgr_DtcStatusDataReport(i, (DTC_TEST_INIT | DTC_TEST_NEW_OPR));
    }	
}

/***********************************************************************
* FUNCTION    : DescMgr_DtcStatusDataReport
* DESCRIPTION : None
* INPUTS      : None
* RETURN      : None
***********************************************************************/
void DescMgr_DtcStatusDataReport(INT8U dtc_code, INT8U dtc_test_type)
{
	BOOLEAN dtc_status_changed = FALSE; 
		
	if ((dtc_code < DTC_CODE_MAX_NUM) && ((TRUE == DescMgr_CanDiagDtcControlSettingCheck()) || (DTC_TEST_CLEAR == dtc_test_type)))
	{
#if defined (DESC_ENABLE_DTC_STATUS_BIT0)		
		/* DTC Status Bit0: Test Fail */
		if (TRUE == DescMgr_ProcTestFailed(dtc_code, dtc_test_type))
		{
			dtc_status_changed = TRUE;
		}
#endif

#if defined (DESC_ENABLE_DTC_STATUS_BIT1)	
		/* DTC Status Bit1: Test Fail This Operation Cycle */
		if (TRUE == DescMgr_ProcTestFailedThisOperationCycle(dtc_code, dtc_test_type))
		{
			dtc_status_changed = TRUE;
		}
#endif

#if defined (DESC_ENABLE_DTC_STATUS_BIT2)	
		/* DTC Status Bit2: Not Confirmed DTC */
		if (TRUE == DescMgr_ProcPendingDTC(dtc_code, dtc_test_type))
		{
			dtc_status_changed = TRUE;
		}
#endif

#if defined (DESC_ENABLE_DTC_STATUS_BIT3)	
		/* DTC Status Bit3: Confirmed DTC */
		if (TRUE == DescMgr_ProcConfirmedDTC(dtc_code, dtc_test_type))
		{
			dtc_status_changed = TRUE;
			bDtcDataNeedSendToEep = TRUE;
		}
#endif

#if defined (DESC_ENABLE_DTC_STATUS_BIT4)	
		/* DTC Status Bit4: Test Not Completed Since Last Clear */
		if (TRUE == DescMgr_ProcTestNotCompletedSinceLastClear(dtc_code, dtc_test_type))
		{
			dtc_status_changed = TRUE;
		}
#endif

#if defined (DESC_ENABLE_DTC_STATUS_BIT5)	
		/* DTC Status Bit5: Test Fail Since Last Clear */
		if (TRUE == DescMgr_ProcTestFailedSinceLastClear(dtc_code, dtc_test_type))
		{
			dtc_status_changed = TRUE;
		}		
#endif

#if defined (DESC_ENABLE_DTC_STATUS_BIT6)	
		/* DTC Status Bit6: Test Not Completed This Operation Cycle */
		if (TRUE == DescMgr_ProcTestNotCompletedThisOperationCycle(dtc_code, dtc_test_type))
		{
			dtc_status_changed = TRUE;
		}
#endif

#if defined (DESC_ENABLE_DTC_STATUS_BIT7)	
		/* DTC Status Bit7: Warning Indicator Requested */
		if (TRUE == DescMgr_ProcWarningIndicatorRequested(dtc_code, dtc_test_type))
		{
			dtc_status_changed = TRUE;
		}
#endif
	}
}

/***********************************************************************
* FUNCTION    : DescMgr_ProcTestFailed
* DESCRIPTION : None
* INPUTS      : INT8U dtc_code, INT8U dtc_test_type
* RETURN      : None
***********************************************************************/
BOOLEAN DescMgr_ProcTestFailed(INT8U dtc_code, INT8U dtc_test_type)
{
	BOOLEAN dtc_status_changed = FALSE;

	if (dtc_code < DTC_CODE_MAX_NUM)
	{
		/* DTC Status Bit0 Supported Check */
		if (0 != (DTC_STATUS_BIT0_MASK_TF & DTC_STATUS_AVAILABILITY_MASK)) 
		{
			/* Check test initial */
			if (0 != (dtc_test_type & DTC_TEST_INIT))
			{
				if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailed)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailed = FALSE;
					dtc_status_changed = TRUE;						
				}			
			}

			/* Check test passed */
			if (0 != (dtc_test_type & DTC_TEST_PASSED))
			{
				if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailed)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailed = FALSE;
					dtc_status_changed = TRUE;						
				}
			}

			/* Check test failed */
			if (0 != (dtc_test_type & DTC_TEST_FAILED))
			{
				if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailed)
				{
					dtc_create_record_cnt[dtc_code].dtc_create_cnt++;	/*error count add*/
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailed = TRUE;

					DescMgr_SnapshotDataReport(dtc_code);
					DescMgr_WriteSnapshotDataToEEP(dtc_code);

					DescMgr_ExtendDataReport(dtc_code);
					DescMgr_WriteExtendDataToEEP(dtc_code);

					dtc_status_changed = TRUE;
#if defined (DTC_AGING_COUNTER)
					if (dtc_occurance_count[dtc_code] != 255u)
					{
						dtc_occurance_count[dtc_code]++;
					}

					if (DTC_STATUS_AGING_CNT_RESET != dtc_aging_count[dtc_code])
					{
						dtc_aging_count[dtc_code] = DTC_STATUS_AGING_CNT_RESET;
						bDtcDataNeedSendToEep = TRUE;
						
						dtc_status_changed = TRUE;
					}
#endif
				}
			}

			/* Check test clear */
			if (0 != (dtc_test_type & DTC_TEST_CLEAR))
			{
				/* Clear all counter */
				dtc_status_time_record_cnt[dtc_code].dtc_create_time_cnt = 0u;
				dtc_status_time_record_cnt[dtc_code].dtc_recover_time_cnt = 0u;
				dtc_create_record_cnt[dtc_code].dtc_create_cnt = 0u;

				if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailed)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailed = FALSE;
					dtc_status_changed = TRUE;						
				}
			}

			/* Check test new operation */
			if (0u != (dtc_test_type & DTC_TEST_NEW_OPR))
			{
				/* Nothing to do. */
			}
			
		}
		else
		{
			/* DTC Status Bit not supported, set to a default value if needed. 
			 * Please adapter the default value according to SPEC. */
			if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailed)
			{
				dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailed = FALSE;
				dtc_status_changed = TRUE;
			}
		}
	}

	return dtc_status_changed;
}

/***********************************************************************
* FUNCTION    : DescMgr_ProcTestFailedThisOperationCycle
* DESCRIPTION : None
* INPUTS      : INT8U dtc_code, INT8U dtc_test_type
* RETURN      : None
***********************************************************************/
BOOLEAN DescMgr_ProcTestFailedThisOperationCycle(INT8U dtc_code, INT8U dtc_test_type)
{
	BOOLEAN dtc_status_changed = FALSE;

	if (dtc_code < DTC_CODE_MAX_NUM)
	{
		/* DTC Status Bit1 suppored check */
		if (0u != (DTC_STATUS_BIT1_MASK_TFTOC & DTC_STATUS_AVAILABILITY_MASK))
		{
			/* Check test initial */
			if (0 != (dtc_test_type & DTC_TEST_INIT))
			{
				/* Nothing to do. */			
			}

			/* Check test passed */
			if (0 != (dtc_test_type & DTC_TEST_PASSED))
			{
				/* Nothing to do. */
			}

			/* Check test failed */
			if (0 != (dtc_test_type & DTC_TEST_FAILED))
			{
				if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedThisOperationCycle)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedThisOperationCycle = TRUE;
					dtc_status_changed = TRUE;
				}
			}

			/* Check test clear */
			if (0 != (dtc_test_type & DTC_TEST_CLEAR))
			{
				/* Clear all counter */
				dtc_status_time_record_cnt[dtc_code].dtc_create_time_cnt = 0u;
				dtc_status_time_record_cnt[dtc_code].dtc_recover_time_cnt = 0u;
				dtc_create_record_cnt[dtc_code].dtc_create_cnt = 0u;

				if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedThisOperationCycle)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedThisOperationCycle = TRUE;
					dtc_status_changed = TRUE;
				}
			}	

			/* Check test new operation */
			if (0u != (dtc_test_type & DTC_TEST_NEW_OPR))
			{
				if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedThisOperationCycle)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedThisOperationCycle = TRUE;
					dtc_status_changed = TRUE;
				}
			}
		}
		else
		{
			/* DTC Status Bit not supported, set to a default value if needed. 
			 * Please adapter the default value according to SPEC. */	
			 if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedThisOperationCycle)
			 {
			 	dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedThisOperationCycle = FALSE;
				dtc_status_changed = TRUE;
			 }
		}
	}

	return dtc_status_changed;
}

/***********************************************************************
* FUNCTION    : DescMgr_ProcPendingDTC
* DESCRIPTION : None
* INPUTS      : INT8U dtc_code, INT8U dtc_test_type
* RETURN      : None
***********************************************************************/
BOOLEAN DescMgr_ProcPendingDTC(INT8U dtc_code, INT8U dtc_test_type)
{
	BOOLEAN dtc_status_changed = FALSE;

	DESC_MGR_STAT INT32U testDeltaTrip = 0u;
	testDeltaTrip = delta_trip_count >> dtc_code & 1u;

	if (dtc_code < DTC_CODE_MAX_NUM)
	{
		/* DTC Status Bit2 suppored check */
		if (0u != (DTC_STATUS_BIT1_MASK_TFTOC & DTC_STATUS_AVAILABILITY_MASK))
		{
			/* Check test initial */
			if (0 != (dtc_test_type & DTC_TEST_INIT))
			{
				/* Nothing to do. */			
			}

			if (TRUE == bSetIncreaseHistoryPendingCntFlag[dtc_code])
			{
				bSetIncreaseHistoryPendingCntFlag[dtc_code] = FALSE;
#if defined (DTC_AGING_COUNTER)
				if (255u != dtc_pending_count[dtc_code])
				{
					dtc_pending_count[dtc_code]++;
					bDtcDataNeedSendToEep = TRUE;
				}
#endif
			}

			/* Check test passed */
			if (0u != (dtc_test_type & DTC_TEST_PASSED))
			{
#if defined (DTC_AGING_COUNTER)
				if (TRUE == bSetClearHistoryPendingCntFlag[dtc_code])
				{
					bSetClearHistoryPendingCntFlag[dtc_code] = FALSE;

					if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.pendingDTC)
					{
						if (testDeltaTrip)
						{
							dtc_current_status_record[dtc_code].dtc_status.status_bits.pendingDTC = FALSE;
							dtc_pending_count[dtc_code] = 0u;
							DescMgr_SetDeltaTripCounter(dtc_code, FALSE);

							dtc_status_changed = TRUE;
						}
						else
						{
							DescMgr_SetDeltaTripCounter(dtc_code, FALSE);
						}
					}
				}
#endif			
			}

			/* Check test failed */
			if (0 != (dtc_test_type & DTC_TEST_FAILED))
			{
				if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.pendingDTC)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.pendingDTC = TRUE;
#ifdef DTC_AGING_COUNTER
					if (255u != dtc_pending_count[dtc_code])
					{
						dtc_pending_count[dtc_code]++;
						bDtcDataNeedSendToEep = TRUE;
					}
#endif
					dtc_status_changed = TRUE;
				}				
			}

			/* Check test clear */
            if (0u != (dtc_test_type & DTC_TEST_CLEAR))
            {
                dtc_status_time_record_cnt[dtc_code].dtc_create_time_cnt = 0u;
                dtc_status_time_record_cnt[dtc_code].dtc_recover_time_cnt = 0u;

                dtc_create_record_cnt[dtc_code].dtc_create_cnt = 0u;


                if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.pendingDTC)
                {
                    dtc_current_status_record[dtc_code].dtc_status.status_bits.pendingDTC = FALSE;
                    dtc_status_changed = TRUE;
                }
            }

			/* Check test new operation */
            if (0u != (dtc_test_type & DTC_TEST_NEW_OPR))
            {
                /* Nothing to do */
            }			
		}
		else
		{
			/* DTC Status Bit not supported, set to a default value if needed. 
			 * Please adapter the default value according to SPEC. */	
			 if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.pendingDTC)
			 {
			 	dtc_current_status_record[dtc_code].dtc_status.status_bits.pendingDTC = FALSE;
				dtc_status_changed = TRUE;
			 }			
		}
	}
	
	return dtc_status_changed;
}

/***********************************************************************
* FUNCTION    : DescMgr_ProcConfirmedDTC
* DESCRIPTION : None
* INPUTS      : INT8U dtc_code, INT8U dtc_test_type
* RETURN      : None
***********************************************************************/
BOOLEAN DescMgr_ProcConfirmedDTC(INT8U dtc_code, INT8U dtc_test_type)
{
	BOOLEAN dtc_status_changed = FALSE;

	if (dtc_code < DTC_CODE_MAX_NUM)
	{
		/* DTC Status Bit3 supported check */
		if (0u != (DTC_STATUS_BIT3_MASK_CDTC & DTC_STATUS_AVAILABILITY_MASK))
		{
			/* Check test initial */
			if (0u != (dtc_test_type & DTC_TEST_INIT))
            {
                /* Nothing to do */
            }	

			/* Check test passed */
            if (0u != (dtc_test_type & DTC_TEST_PASSED))
            {                
#if defined (DTC_AGING_COUNTER)
                if (TRUE == bSetClearHistoryFaultCntFlag[dtc_code])
                {
					bSetClearHistoryFaultCntFlag[dtc_code] = FALSE;

					if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.confirmedDTC)
                    {
                        if (dtc_aging_count[dtc_code] >= DTC_STATUS_AGING_CNT_MAX)
                        {
                            dtc_current_status_record[dtc_code].dtc_status.status_bits.confirmedDTC = FALSE;
							
							/* DTC aging counter and DTC occurrence counter will be reset to 0 by self healing */
                            dtc_aging_count[dtc_code] = DTC_STATUS_AGING_CNT_RESET;
							dtc_occurance_count[dtc_code] = 0u;
                            dtc_pending_count[dtc_code] = 0u;

							/* When the aging counter is up to self healing value, DTC will be moved out from current memory */
							//CanDiagApp_DTC_StatusDataReport(dtc_code, DTC_TEST_CLEAR);
							
							if(dtc_aged_count[dtc_code] < 255u)
							{
								/* When the DTC's aging counter is up to set-value, Aged counter will be increased by 1. */
								dtc_aged_count[dtc_code]++;
							}
							else
							{
								/* If the aged counter is up to 255, it will keep to 255. */
								dtc_aged_count[dtc_code] = 255u;
							}
							
                            dtc_status_changed = TRUE;
                        }
                        else
                        {
                        	/* ECU error never occur during the last whole operation cycle, driving cycle, or warm-up cycle.The DTC aging
							 * counter value shall be increased by 1. */
                            dtc_aging_count[dtc_code]++;
                            dtc_status_changed = TRUE;
                        }
                    }
                }
#endif
            }

			/* Check test failed */
			if (0u != (dtc_test_type & DTC_TEST_FAILED))
            {
                if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.confirmedDTC)
                {                   
                    dtc_current_status_record[dtc_code].dtc_status.status_bits.confirmedDTC = TRUE;
                    dtc_aging_count[dtc_code] = DTC_STATUS_AGING_CNT_RESET;
                    
                    dtc_status_changed = TRUE;
                }
            }

			/* Check test clear */
            if (0u != (dtc_test_type & DTC_TEST_CLEAR))
            {
                dtc_status_time_record_cnt[dtc_code].dtc_create_time_cnt = 0u;
                dtc_status_time_record_cnt[dtc_code].dtc_recover_time_cnt = 0u;
                dtc_create_record_cnt[dtc_code].dtc_create_cnt = 0u;

                if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.confirmedDTC)
                {
                    dtc_current_status_record[dtc_code].dtc_status.status_bits.confirmedDTC = FALSE;
                    dtc_status_changed = TRUE;
                }
            }

			/* Check test new operation */
            if (0u != (dtc_test_type & DTC_TEST_NEW_OPR))
            {
                /* Nothing to do */
            }
		}
		else
		{
			/* DTC Status Bit not supported, set to a default value if needed. 
			 * Please adapter the default value according to SPEC. */
			if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.confirmedDTC)
            {
                dtc_current_status_record[dtc_code].dtc_status.status_bits.confirmedDTC = FALSE;
                dtc_status_changed = TRUE;
            }			
		}
	}
	
	return dtc_status_changed;
}

/***********************************************************************
* FUNCTION    : DescMgr_ProcTestNotCompletedSinceLastClear
* DESCRIPTION : None
* INPUTS      : INT8U dtc_code, INT8U dtc_test_type
* RETURN      : None
***********************************************************************/
BOOLEAN DescMgr_ProcTestNotCompletedSinceLastClear(INT8U dtc_code, INT8U dtc_test_type)
{
	BOOLEAN dtc_status_changed = FALSE;

	if (dtc_code < DTC_CODE_MAX_NUM)
	{
		/* DTC Status Bit4 supported check */
		if (0u != (DTC_STATUS_BIT4_MASK_TNCSLC & DTC_STATUS_AVAILABILITY_MASK))
		{
			/* Check test initial */
			if (0 != (dtc_test_type & DTC_TEST_INIT))
			{
				if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedSinceLastClear)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedSinceLastClear = TRUE;
					dtc_status_changed = TRUE;
				}
			}

			/* Check test passed */
			if (0 != (dtc_test_type & DTC_TEST_PASSED))
			{
				if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedSinceLastClear)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedSinceLastClear = FALSE;
					dtc_status_changed = TRUE;
				}	
			}

			/* Check test failed */
			if (0 != (dtc_test_type & DTC_TEST_FAILED))
			{
				if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedSinceLastClear)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedSinceLastClear = FALSE;
					dtc_status_changed = TRUE;
				}	
			}	

			/* Check test clear */
			if (0 != (dtc_test_type & DTC_TEST_CLEAR))
			{
				/* Clear all counter */
				dtc_status_time_record_cnt[dtc_code].dtc_create_time_cnt = 0u;
				dtc_status_time_record_cnt[dtc_code].dtc_recover_time_cnt = 0u;
				dtc_create_record_cnt[dtc_code].dtc_create_cnt = 0u;

				if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedSinceLastClear)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedSinceLastClear = TRUE;
					dtc_status_changed = TRUE;
				}
			}	

			/* Check test new operation */
            if (0u != (dtc_test_type&DTC_TEST_NEW_OPR))
            {
                /* Nothing to do */
            } 			
		}
		else
		{
			/* DTC Status Bit not supported, set to a default value if needed. 
			 * Please adapter the default value according to SPEC. */	
			 if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedSinceLastClear)
			 {
			 	dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedSinceLastClear = FALSE;
				dtc_status_changed = TRUE;
			 }			
		}
	}
	
	return dtc_status_changed;
}

/***********************************************************************
* FUNCTION    : DescMgr_ProcTestFailedSinceLastClear
* DESCRIPTION : None
* INPUTS      : INT8U dtc_code, INT8U dtc_test_type
* RETURN      : None
***********************************************************************/
BOOLEAN DescMgr_ProcTestFailedSinceLastClear(INT8U dtc_code, INT8U dtc_test_type)
{
	BOOLEAN dtc_status_changed = FALSE;

	if (dtc_code < DTC_CODE_MAX_NUM)
	{
		/* DTC Status Bit5 supported check */
		if (0u != (DTC_STATUS_BIT5_MASK_TFSLC & DTC_STATUS_AVAILABILITY_MASK))
		{
			/* Check test initial */
			if (0 != (dtc_test_type & DTC_TEST_INIT))
			{
				if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedSinceLastClear)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedSinceLastClear = FALSE;
					dtc_status_changed = TRUE;
				}
			}

			/* Check test passed */
			if (0 != (dtc_test_type & DTC_TEST_PASSED))
			{
				/* Nothing to do. */
			}

			/* Check test failed */
			if (0 != (dtc_test_type & DTC_TEST_FAILED))
			{
				if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedSinceLastClear)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedSinceLastClear = TRUE;
					dtc_status_changed = TRUE;
				}	
			}	

			/* Check test clear */
			if (0 != (dtc_test_type & DTC_TEST_CLEAR))
			{
				/* Clear all counter */
				dtc_status_time_record_cnt[dtc_code].dtc_create_time_cnt = 0u;
				dtc_status_time_record_cnt[dtc_code].dtc_recover_time_cnt = 0u;
				dtc_create_record_cnt[dtc_code].dtc_create_cnt = 0u;

				if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedSinceLastClear)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedSinceLastClear = FALSE;
					dtc_status_changed = TRUE;
				}
			}	

			/* Check test new operation */
            if (0u != (dtc_test_type & DTC_TEST_NEW_OPR))
            {
                /* Nothing to do */
            } 			
		}
		else
		{
			/* DTC Status Bit not supported, set to a default value if needed. 
			 * Please adapter the default value according to SPEC. */	
			 if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedSinceLastClear)
			 {
			 	dtc_current_status_record[dtc_code].dtc_status.status_bits.testFailedSinceLastClear = FALSE;
				dtc_status_changed = TRUE;
			 }			
		}
	}

	return dtc_status_changed;
}

/***********************************************************************
* FUNCTION    : DescMgr_ProcTestNotCompletedThisOperationCycle
* DESCRIPTION : None
* INPUTS      : INT8U dtc_code, INT8U dtc_test_type
* RETURN      : None
***********************************************************************/
BOOLEAN DescMgr_ProcTestNotCompletedThisOperationCycle(INT8U dtc_code, INT8U dtc_test_type)
{
	BOOLEAN dtc_status_changed = FALSE;

	if (dtc_code < DTC_CODE_MAX_NUM)
	{
		/* DTC Status Bit6 supported check */
		if (0u != (DTC_STATUS_BIT6_MASK_TNCTOC & DTC_STATUS_AVAILABILITY_MASK))
		{
			/* Check test initial */
			if (0 != (dtc_test_type & DTC_TEST_INIT))
			{
				if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle = TRUE;
					dtc_status_changed = TRUE;
				}
			}

			/* Check test passed */
			if (0 != (dtc_test_type & DTC_TEST_PASSED))
			{
				if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle = FALSE;
					dtc_status_changed = TRUE;
				}
			}

			/* Check test failed */
			if (0 != (dtc_test_type & DTC_TEST_FAILED))
			{
				if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle = FALSE;
					dtc_status_changed = TRUE;
				}	
			}	

			/* Check test clear */
			if (0 != (dtc_test_type & DTC_TEST_CLEAR))
			{
				/* Clear all counter */
				dtc_status_time_record_cnt[dtc_code].dtc_create_time_cnt = 0u;
				dtc_status_time_record_cnt[dtc_code].dtc_recover_time_cnt = 0u;
				dtc_create_record_cnt[dtc_code].dtc_create_cnt = 0u;

				if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle)
				{
					dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle = TRUE;
					dtc_status_changed = TRUE;
				}
			}	

			/* Check test new operation */
            if (0u != (dtc_test_type&DTC_TEST_NEW_OPR))
            {
                if (FALSE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle)
                {
                    dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle = TRUE;
                    dtc_status_changed = TRUE;
                }
            } 			
		}
		else
		{
			/* DTC Status Bit not supported, set to a default value if needed. 
			 * Please adapter the default value according to SPEC. */	
			 if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle)
			 {
			 	dtc_current_status_record[dtc_code].dtc_status.status_bits.testNotCompletedThisOperationCycle = FALSE;
				dtc_status_changed = TRUE;
			 }			
		}
	}

	return dtc_status_changed;
}

/***********************************************************************
* FUNCTION    : DescMgr_ProcWarningIndicatorRequested
* DESCRIPTION : None
* INPUTS      : INT8U dtc_code, INT8U dtc_test_type
* RETURN      : None
***********************************************************************/
BOOLEAN DescMgr_ProcWarningIndicatorRequested(INT8U dtc_code, INT8U dtc_test_type)
{
	BOOLEAN dtc_status_changed = FALSE;

    if (dtc_code < DTC_CODE_MAX_NUM)
    {
		/* DTC Status Bit7 supported check */
		if (0u != (DTC_STATUS_BIT7_MASK_WIR & DTC_STATUS_AVAILABILITY_MASK)) 
        {
			/* Check test initial */
            if (0u != (dtc_test_type & DTC_TEST_INIT))
            {
				/* Not implement yet */
            }

			/* Check test passed */
            if (0u != (dtc_test_type & DTC_TEST_PASSED))
            {
                /* Not implement yet */
            }

			/* Check test failed */			
            if (0u != (dtc_test_type & DTC_TEST_FAILED))
            {
                /* Not implement yet */
            }

			/* Check test clear */			
            if (0u != (dtc_test_type & DTC_TEST_CLEAR))
            {
                dtc_status_time_record_cnt[dtc_code].dtc_create_time_cnt = 0u;
                dtc_status_time_record_cnt[dtc_code].dtc_recover_time_cnt = 0u;
                dtc_create_record_cnt[dtc_code].dtc_create_cnt = 0u;

                /* Not implement yet */
            }

			/* Check test new operation */
            if (0u != (dtc_test_type & DTC_TEST_NEW_OPR))
            {
                /* Not implement yet */
            }    
        }
        else
        {
			/* DTC Status Bit not supported, set to a default value if needed. 
			 * Please adapter the default value according to SPEC. */
            if (TRUE == dtc_current_status_record[dtc_code].dtc_status.status_bits.warningIndicatorRequested)
            {
                dtc_current_status_record[dtc_code].dtc_status.status_bits.warningIndicatorRequested = FALSE;
                dtc_status_changed = TRUE;
            }
        }
    }
	
	return dtc_status_changed;
}

/*****************************************************************************************
*  Name        : DescMgr_ExtendDataReport
*  Description :     
*  Parameter   : INT8U dtc_code
*  Returns     : none 
*****************************************************************************************/
DESC_MGR_STAT void DescMgr_ExtendDataReport(INT8U dtc_code)
{
	// TODO:
}

/*****************************************************************************************
*  Name        : DescMgr_SnapshotDataReport
*  Description :     
*  Parameter   : INT8U dtc_code
*  Returns     : none 
*****************************************************************************************/
DESC_MGR_STAT void DescMgr_SnapshotDataReport(INT8U dtc_code)
{
	// TODO:Snapshot data need to conversion

	const T_ADC_APP *ptr = AdcApp_SampleData();

	// 19A0:DC Vlink voltage
	dtc_snapshot_data_record[dtc_code].DcVoltage_High_Byte = (INT8U)((ptr->vehiclePowerVoltage & 0xFF00) >> 8u);
	dtc_snapshot_data_record[dtc_code].DcVoltage_Low_Byte = (INT8U)(ptr->vehiclePowerVoltage & 0x00FF);

	// 19A1:ECU output current
	dtc_snapshot_data_record[dtc_code].OutputCurrent_High_Byte = 0u;
	dtc_snapshot_data_record[dtc_code].OutputCurrent_Low_Byte = 0u;

	// 19A2:Target frequency
	dtc_snapshot_data_record[dtc_code].TargetFrequency_High_Byte = 0u;
	dtc_snapshot_data_record[dtc_code].TargetFrequency_Low_Byte = 0u;	

	// 19A3:Actual frequency
	dtc_snapshot_data_record[dtc_code].ActualFrequency_High_Byte = 0u;
	dtc_snapshot_data_record[dtc_code].ActualFrequency_Low_Byte = 0u;		

	// 19A4:Target torque
	dtc_snapshot_data_record[dtc_code].TargetTorque_High_Byte = 0u;
	dtc_snapshot_data_record[dtc_code].TargetTorque_Low_Byte = 0u;

	// 19A5:Motor temperature
	dtc_snapshot_data_record[dtc_code].MotorTemperature = 0u;

	// 19A6:ECU temperature
	dtc_snapshot_data_record[dtc_code].EcuTemperature = 0u;	

	// 19A7:Ext power voltage
	dtc_snapshot_data_record[dtc_code].ExtPowerVoltage_High_Byte = 0u;
	dtc_snapshot_data_record[dtc_code].ExtPowerVoltage_Low_Byte = 0u;	

	return;
}

/******************************************************************************
* Name         :  DescMgr_SetDeltaTripCounter 
* Called by    :   
* Preconditions:  None
* Parameters   :  None
* Description  :  uidq2008
******************************************************************************/
DESC_MGR_STAT void DescMgr_SetDeltaTripCounter(INT8U dtc_code, BOOLEAN delta_Trip_flag)
{
   if (TRUE == delta_Trip_flag)
   {
        delta_trip_count |= (1u << dtc_code);
   }
   else
   {
        delta_trip_count &= ~(1u << dtc_code);
   }
}

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagDtcRecordBodyEnableCheck
* DESCRIPTION : 
* INPUTS      : none
* RETURN      : none
***********************************************************************/
BOOLEAN DescMgr_CanDiagDtcRecordBodyEnableCheck(void)
{
    return (((dtc_record_enable_mask & DTC_RECORD_ENALBE_BDY) != 0u) ? 1u : 0u);
}


/***********************************************************************
* FUNCTION    : DescMgr_CanDiagDtcRecordNmEnableCheck
* DESCRIPTION : 
* INPUTS      : none
* RETURN      : none
***********************************************************************/
BOOLEAN DescMgr_CanDiagDtcRecordNmEnableCheck(void)
{
    return (((dtc_record_enable_mask & DTC_RECORD_ENABLE_NM) != 0u) ? 1u : 0u);
}


/***********************************************************************
* FUNCTION    : DescMgr_CanDiagDtcRecordOpVoltEnableCheck
* DESCRIPTION : 
* INPUTS      : none
* RETURN      : none
***********************************************************************/
BOOLEAN DescMgr_CanDiagDtcRecordOpVoltEnableCheck(void)
{
    return (((dtc_record_enable_mask & DTC_RECORD_ENABLE_OP_VOLT) != 0u) ? 1u : 0u);
}

/**** $85 ****/

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagDtcControlSettingOn
* DESCRIPTION : TRUE:means control dtc setting is on,
*               FALSE:means control dtc setting is off.
* INPUTS      : none
* RETURN      : none
***********************************************************************/
void DescMgr_CanDiagDtcControlSettingOn(void)
{
    dtc_control_setting_on_off = TRUE;
}

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagDtcControlSettingOff
* DESCRIPTION : TRUE:means control dtc setting is on,
*               FALSE:means control dtc setting is off.
* INPUTS      : none
* RETURN      : none
***********************************************************************/
void DescMgr_CanDiagDtcControlSettingOff(void)
{
    dtc_control_setting_on_off = FALSE;
}

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagDtcControlSettingCheck
* DESCRIPTION : TRUE:means control dtc setting is on,
*               FALSE:means control dtc setting is off.
* INPUTS      : none
* RETURN      : none
***********************************************************************/
BOOLEAN DescMgr_CanDiagDtcControlSettingCheck(void)
{
    return dtc_control_setting_on_off;
}

/**** $14 ****/
/***********************************************************************
* FUNCTION    : DescMgr_CanDiagClearDtcDataStart
* DESCRIPTION : none
* INPUTS      : none
* RETURN      : none
***********************************************************************/
void DescMgr_CanDiagClearDtcDataStart(void)
{
    /* do not care about last time, just reset the whole clear things */
    dtc_status_svc14_clear_counter = DTC_STATUS_SVC14_CLEAR_START; /* clear begin */
    
    /* Adjust dtc record enable mask */
    DescMgr_CanDigDtcRecordEnableAdjust();
}

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagDtcStatusServiceClearAll
* DESCRIPTION : Do not clear all the dtc status at the same time, it would cost worse CPU/thread load,
*               So just separate the clear action, Called by desc_delay_service_task to do this job.
* INPUTS      : none
* RETURN      : none
***********************************************************************/
DESC_MGR_STAT BOOLEAN DescMgr_CanDiagDtcStatusServiceClearAll(void)
{
    BOOLEAN clear_finished = FALSE;

    if (dtc_status_svc14_clear_counter < DTC_CODE_MAX_NUM)
    {
		DescMgr_DtcStatusDataReport(dtc_status_svc14_clear_counter, DTC_TEST_CLEAR);

#ifdef DTC_AGING_COUNTER
		memset(dtc_aged_count, DTC_STATUS_AGING_CNT_RESET, DTC_CODE_MAX_NUM);
		memset(dtc_aging_count, DTC_STATUS_AGING_CNT_RESET, DTC_CODE_MAX_NUM);
		memset(dtc_pending_count, DTC_STATUS_AGING_CNT_RESET, DTC_CODE_MAX_NUM);	
		memset(dtc_occurance_count, DTC_STATUS_AGING_CNT_RESET, DTC_CODE_MAX_NUM);
#endif
        dtc_status_svc14_clear_counter++;
		clear_finished = FALSE;
    }
    else
    {
		/* Adjust dtc record enable mask */
        DescMgr_CanDigDtcRecordEnableAdjust();    
        dtc_status_svc14_clear_counter = DTC_STATUS_SVC14_CLEAR_FINISHED;  
        clear_finished = TRUE;
    }
    
    return clear_finished;
}

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagDtcStatusServiceClearFinished
* DESCRIPTION : none                        
* INPUTS      : none
* RETURN      : none
***********************************************************************/
DESC_MGR_STAT BOOLEAN DescMgr_CanDiagDtcStatusServiceClearFinished(void)
{
    return ((DTC_STATUS_SVC14_CLEAR_FINISHED == dtc_status_svc14_clear_counter) ? 1u : 0u);
}

/***********************************************************************
* FUNCTION    : DescMgr_CanDigDtcRecordEnableAdjust
* DESCRIPTION : none
* INPUTS      : none
* RETURN      : none
***********************************************************************/
DESC_MGR_STAT void DescMgr_CanDigDtcRecordEnableAdjust(void)
{
    /* diagnosis function start-up init delay according to vehicle manufacturer */
    if (CAN_DIAG_INIT_DELAY_TIMEOUT != can_diag_init_delay_timer)
    {
        /* all dtc record disable */
        dtc_record_enable_mask = DTC_RECORD_ENABLE_NONE;
        
        return;
    }
    else
    {
        /* Disable here, because we do not know the other situations, so let other judgement following to do this job. */
        dtc_record_enable_mask = DTC_RECORD_ENABLE_NONE;
    }

    /* dtc eeprom init delay */
    if (DTC_INIT_STEP_FINISHED != dtc_eeprom_init_step)
    {
        /* all dtc record disable */
        dtc_record_enable_mask = DTC_RECORD_ENABLE_NONE;
        
        return;    
    }
    else
    {
        /* Disable here, because we do not know the other situations, so let other judgement following to do this job. */
        dtc_record_enable_mask = DTC_RECORD_ENABLE_NONE;        
    }

    /* service $14 clear dtc eeprom init delay */
    if (DTC_STATUS_SVC14_CLEAR_FINISHED != dtc_status_svc14_clear_counter)
    {
        /* all dtc record disable */
        dtc_record_enable_mask = DTC_RECORD_ENABLE_NONE;
        
        return;    
    }
    else
    {
        /* Disable here, because we do not know the other situations, so let other judgement following to do this job. */
        dtc_record_enable_mask = DTC_RECORD_ENABLE_NONE;        
    }

    /* other delay according to vehicle manufacturer */
	// TODO:System power voltage status 9-16V.	
    if (1)  /*if power out range of [9-16v],stop candiag*/
    {
        dtc_record_enable_mask = (dtc_record_enable_mask | DTC_RECORD_ENALBE_BDY);//DTC_RECORD_ENABLE_OP_VOLT

        if (CAN_DIAG_BAT_VOLT_RECOVER_DELAY_TIMEOUT != can_diag_bat_volt_recover_delay_timer)
        {
            dtc_record_enable_mask = (dtc_record_enable_mask & (~DTC_RECORD_ENABLE_NM));
            dtc_record_enable_mask = (dtc_record_enable_mask & (~DTC_RECORD_ENALBE_BDY));

            return;
        }
        else
        {
            dtc_record_enable_mask = (dtc_record_enable_mask | DTC_RECORD_ENALBE_BDY);

            if (CAN_DIAG_NM_INIT_DELAY_TIMEOUT == can_diag_init_nm_delay_timer)
            {
                dtc_record_enable_mask = (dtc_record_enable_mask | DTC_RECORD_ENABLE_NM);
            }
            else
            {
                dtc_record_enable_mask = (dtc_record_enable_mask & (~DTC_RECORD_ENABLE_NM));
            }
        }
    }
    else
    {
        dtc_record_enable_mask = (dtc_record_enable_mask|DTC_RECORD_ENALBE_BDY);//DTC_RECORD_ENABLE_OP_VOLT
    }

}

/**** $19 ****/

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagGetDtcCount
* DESCRIPTION : none
* INPUTS      : INT8U status_mask
* RETURN      : none
***********************************************************************/
INT16U DescMgr_CanDiagGetDtcCount(INT8U status_mask)
{
    INT16U dtc_count = 0u;
    INT8U record_count = 0u;

    for (record_count=0u; record_count<DTC_CODE_MAX_NUM; record_count++)
    {
        if ((dtc_current_status_record[record_count].dtc_status.status_byte & status_mask) != 0u)
        {
            dtc_count++;
        }
    }
    
    return dtc_count;
}

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagGetDtcStatusByStatusMask
* DESCRIPTION : none
* INPUTS      : INT8U *p_dtc, INT8U status_mask
* RETURN      : none
***********************************************************************/
INT16U DescMgr_CanDiagGetDtcStatusByStatusMask(INT8U *p_dtc, INT8U status_mask)
{
    INT16U dtc_count = 0u;
    INT8U record_count = 0u;

    for (record_count=0u; record_count<DTC_CODE_MAX_NUM; record_count++)
    {
        if ((dtc_current_status_record[record_count].dtc_status.status_byte & status_mask) != 0u)
        {
            *p_dtc++ = dtc_code_data[record_count].dtc_bytes.dtc_high_byte;
            *p_dtc++ = dtc_code_data[record_count].dtc_bytes.dtc_middle_byte;
            *p_dtc++ = dtc_code_data[record_count].dtc_bytes.dtc_low_byte;
            *p_dtc++ = dtc_current_status_record[record_count].dtc_status.status_byte;
            
            dtc_count++;
        }
    }
    
    return dtc_count;
}

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagGetSupportedDtc
* DESCRIPTION : none
* INPUTS      : *p_dtc
* RETURN      : none
***********************************************************************/
INT16U DescMgr_CanDiagGetSupportedDtc(INT8U *p_dtc)
{
    INT16U dtc_count = 0u;
    INT8U record_count = 0u;

    for (record_count=0u; record_count<DTC_CODE_MAX_NUM; record_count++)
    {
        *p_dtc++ = dtc_code_data[record_count].dtc_bytes.dtc_high_byte;
        *p_dtc++ = dtc_code_data[record_count].dtc_bytes.dtc_middle_byte;
        *p_dtc++ = dtc_code_data[record_count].dtc_bytes.dtc_low_byte;
        *p_dtc++ = dtc_current_status_record[record_count].dtc_status.status_byte;
        
        dtc_count++;
    }
	
    return dtc_count;
}

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagGetSnapshotData
* DESCRIPTION : none
* INPUTS      : INT8U *res_data, INT8U *req_dtc
* RETURN      : none
***********************************************************************/
void DescMgr_CanDiagGetSnapshotData(INT8U *res_data, INT8U *req_dtc)
{
    INT8U record_count;
	
	for (record_count=0u; record_count<DTC_CODE_MAX_NUM; record_count++)   
    {
    	if ((dtc_code_data[record_count].dtc_bytes.dtc_high_byte == req_dtc[2]) 
		 && (dtc_code_data[record_count].dtc_bytes.dtc_middle_byte == req_dtc[3])
		 && (dtc_code_data[record_count].dtc_bytes.dtc_low_byte == req_dtc[4]))
        {
        	if ((dtc_current_status_record[record_count].dtc_status.status_byte & DTC_STATUS_AVAILABILITY_MASK) != 0)
        	{
        		DescMgr_ReturnSnapshotData(res_data, record_count);
			}
            else
            {
                DescMgr_ReturnSnapshotDataDefault(res_data, record_count);
            }
        }
    }
}

/***********************************************************************
* FUNCTION    : DescMgr_ReturnSnapshotData
* DESCRIPTION : none
* INPUTS      : INT8U *res_data, DTC_CODE_NUM_TYPE dtc_number
* RETURN      : none
***********************************************************************/
DESC_MGR_STAT void DescMgr_ReturnSnapshotData(INT8U *res_data, DTC_CODE_NUM_TYPE dtc_number)
{
    *(res_data+0) = dtc_code_data[dtc_number].dtc_bytes.dtc_high_byte;	          /* DTCHighByte */
    *(res_data+1) = dtc_code_data[dtc_number].dtc_bytes.dtc_middle_byte;          /* DTCMiddleByte */
    *(res_data+2) = dtc_code_data[dtc_number].dtc_bytes.dtc_low_byte;	          /* DTCLowByte */
    *(res_data+3) = dtc_current_status_record[dtc_number].dtc_status.status_byte; /* statusOfDTC */	
    *(res_data+4) = 0x01; /* DTC Snapshot Record Number */   
    *(res_data+5) = 0x07; /* DTC Snapshot Record Number Of Identifiers */

    *(res_data+6) = 0x19;
    *(res_data+7) = 0xA0;
    *(res_data+8) = dtc_snapshot_data_record[dtc_number].DcVoltage_High_Byte;
    *(res_data+9) = dtc_snapshot_data_record[dtc_number].DcVoltage_Low_Byte;

    *(res_data+10) = 0x19;
    *(res_data+11) = 0xA1;
    *(res_data+12) = dtc_snapshot_data_record[dtc_number].OutputCurrent_High_Byte;
    *(res_data+13) = dtc_snapshot_data_record[dtc_number].OutputCurrent_Low_Byte;

    *(res_data+14) = 0x19;
    *(res_data+15) = 0xA2;
    *(res_data+16) = dtc_snapshot_data_record[dtc_number].TargetFrequency_High_Byte;
    *(res_data+17) = dtc_snapshot_data_record[dtc_number].TargetFrequency_Low_Byte;	

    *(res_data+18) = 0x19;
    *(res_data+19) = 0xA3;
    *(res_data+20) = dtc_snapshot_data_record[dtc_number].ActualFrequency_High_Byte;
    *(res_data+21) = dtc_snapshot_data_record[dtc_number].ActualFrequency_Low_Byte;	

    *(res_data+22) = 0x19;
    *(res_data+23) = 0xA4;
    *(res_data+24) = dtc_snapshot_data_record[dtc_number].TargetTorque_High_Byte;
    *(res_data+25) = dtc_snapshot_data_record[dtc_number].TargetTorque_Low_Byte;

    *(res_data+26) = 0x19;
    *(res_data+27) = 0xA5;
    *(res_data+28) = dtc_snapshot_data_record[dtc_number].MotorTemperature;

    *(res_data+29) = 0x19;
    *(res_data+30) = 0xA6;
    *(res_data+31) = dtc_snapshot_data_record[dtc_number].EcuTemperature;	

    *(res_data+32) = 0x19;
    *(res_data+33) = 0xA7;
    *(res_data+34) = dtc_snapshot_data_record[dtc_number].ExtPowerVoltage_High_Byte;
    *(res_data+35) = dtc_snapshot_data_record[dtc_number].ExtPowerVoltage_Low_Byte;	
}

/***********************************************************************
* FUNCTION    : DescMgr_ReturnSnapshotDataDefault
* DESCRIPTION : none
* INPUTS      : INT8U *res_data, DTC_CODE_NUM_TYPE dtc_number
* RETURN      : none
***********************************************************************/
DESC_MGR_STAT void DescMgr_ReturnSnapshotDataDefault(INT8U *res_data, DTC_CODE_NUM_TYPE dtc_number)
{
    *(res_data+0) = dtc_code_data[dtc_number].dtc_bytes.dtc_high_byte;	          /* DTCHighByte */
    *(res_data+1) = dtc_code_data[dtc_number].dtc_bytes.dtc_middle_byte;          /* DTCMiddleByte */
    *(res_data+2) = dtc_code_data[dtc_number].dtc_bytes.dtc_low_byte;	          /* DTCLowByte */
    *(res_data+3) = dtc_current_status_record[dtc_number].dtc_status.status_byte; /* statusOfDTC */	
    *(res_data+4) = 0x01; /* DTC Snapshot Record Number */   
    *(res_data+5) = 0x07; /* DTC Snapshot Record Number Of Identifiers */

    *(res_data+6) = 0x19;
    *(res_data+7) = 0xA0;
    *(res_data+8) = 0x00;
    *(res_data+9) = 0x00;

    *(res_data+10) = 0x19;
    *(res_data+11) = 0xA1;
    *(res_data+12) = 0x00;
    *(res_data+13) = 0x00;

    *(res_data+14) = 0x19;
    *(res_data+15) = 0xA2;
    *(res_data+16) = 0x00;
    *(res_data+17) = 0x00;	

    *(res_data+18) = 0x19;
    *(res_data+19) = 0xA3;
    *(res_data+20) = 0x00;
    *(res_data+21) = 0x00;	

    *(res_data+22) = 0x19;
    *(res_data+23) = 0xA4;
    *(res_data+24) = 0x00;
    *(res_data+25) = 0x00;

    *(res_data+26) = 0x19;
    *(res_data+27) = 0xA5;
    *(res_data+28) = 0x00;

    *(res_data+29) = 0x19;
    *(res_data+30) = 0xA6;
    *(res_data+31) = 0x00;	

    *(res_data+32) = 0x19;
    *(res_data+33) = 0xA7;
    *(res_data+34) = 0x00;
    *(res_data+35) = 0x00;	
}

/***********************************************************************
* FUNCTION    : DescMgr_CanDiagGetExtendData
* DESCRIPTION : none
* INPUTS      : *p_dtc
* RETURN      : none
***********************************************************************/
void DescMgr_CanDiagGetExtendData(INT8U *res_data, INT8U *req_dtc)
{
    INT8U record_count;
	
	for (record_count=0u; record_count<DTC_CODE_MAX_NUM; record_count++)   //added by uidq2008
    {
    	if ((dtc_code_data[record_count].dtc_bytes.dtc_high_byte == req_dtc[2]) 
		&&  (dtc_code_data[record_count].dtc_bytes.dtc_middle_byte == req_dtc[3])
		&&  (dtc_code_data[record_count].dtc_bytes.dtc_low_byte == req_dtc[4]))
        {
        	if ((dtc_current_status_record[record_count].dtc_status.status_byte & DTC_STATUS_AVAILABILITY_MASK) != 0)
        	{
        		DescMgr_ReturnExtendData(res_data, record_count, req_dtc[5]);
			}
            else
            {
                DescMgr_ReturnExtendDataDefault(res_data, record_count, req_dtc[5]);
            }
        }
    }
}

/***********************************************************************
* FUNCTION    : DescMgr_ReturnExtendData
* DESCRIPTION : none
* INPUTS      : *p_dtc
* RETURN      : none
***********************************************************************/
DESC_MGR_STAT void DescMgr_ReturnExtendData(INT8U *res_data, DTC_CODE_NUM_TYPE dtc_number, INT8U RecordNum)
{
    *(res_data+0) = dtc_code_data[dtc_number].dtc_bytes.dtc_high_byte;	          /* DTCHighByte */
    *(res_data+1) = dtc_code_data[dtc_number].dtc_bytes.dtc_middle_byte;	      /* DTCMiddleByte */
    *(res_data+2) = dtc_code_data[dtc_number].dtc_bytes.dtc_low_byte;	          /* DTCLowByte */
    *(res_data+3) = dtc_current_status_record[dtc_number].dtc_status.status_byte; /* statusOfDTC */
	
    switch (RecordNum)    
    {
        case 0x01: /*Fault occurrence counter */
            *(res_data+4) = 0x01;
            *(res_data+5) = dtc_occurance_count[dtc_number]; 
            break;
			
        case 0x02: /*Fault pending counter  */
            *(res_data+4) = 0x02;
            *(res_data+5) = dtc_pending_count[dtc_number];	
            break;
			
        case 0x03: /* DTC Aging counter */
            *(res_data+4) = 0x03;
            *(res_data+5) = dtc_aging_count[dtc_number];	
            break;
			
        case 0x04: /* DTC Aged counter */
            *(res_data+4) = 0x04;
            *(res_data+5) = dtc_aged_count[dtc_number];	
            break;
			
        case 0xFF:
			*(res_data+4) = 0x01;
			*(res_data+5) = dtc_occurance_count[dtc_number];	
			*(res_data+6) = 0x02;
			*(res_data+7) = dtc_pending_count[dtc_number];	
			*(res_data+8) = 0x03;
			*(res_data+9) = dtc_aging_count[dtc_number];	
			*(res_data+10) = 0x04;
			*(res_data+11) = dtc_aged_count[dtc_number];	
            break;
			
        default:
            break;
    }
}

/***********************************************************************
* FUNCTION    : DescMgr_ReturnExtendDataDefault
* DESCRIPTION : none
* INPUTS      : *p_dtc
* RETURN      : none
***********************************************************************/
DESC_MGR_STAT void DescMgr_ReturnExtendDataDefault(INT8U *res_data, DTC_CODE_NUM_TYPE dtc_number, INT8U RecordNum)
{
    *(res_data+0) = dtc_code_data[dtc_number].dtc_bytes.dtc_high_byte;	          /* DTCHighByte */
    *(res_data+1) = dtc_code_data[dtc_number].dtc_bytes.dtc_middle_byte;	      /* DTCMiddleByte */
    *(res_data+2) = dtc_code_data[dtc_number].dtc_bytes.dtc_low_byte;	          /* DTCLowByte */
    *(res_data+3) = dtc_current_status_record[dtc_number].dtc_status.status_byte; /* statusOfDTC */
	
    switch (RecordNum)    
    {
        case 0x01: /* Fault occurrence counter */
            *(res_data+4) = 0x01;
            *(res_data+5) = 0x00;	
            break;
			
        case 0x02: /* Fault pending counter  */
            *(res_data+4) = 0x02;
            *(res_data+5) = 0x00;	
            break;
			
        case 0x03: /* DTC Aging counter */
            *(res_data+4) = 0x03;
            *(res_data+5) = 0x00;	
            break;
			
        case 0x04: /* DTC Aged counter */
            *(res_data+4) = 0x04;
            *(res_data+5) = 0x00;	
            break;
			
        case 0xFF:
			*(res_data+4) = 0x01;
			*(res_data+5) = 0x00;	
			*(res_data+6) = 0x02;
			*(res_data+7) = 0x00;	
			*(res_data+8) = 0x03;
			*(res_data+9) = 0x00;
			*(res_data+10) = 0x04;
			*(res_data+11) = 0x00;	
            break;
			
        default:
            break;
    }
}

static INT8U uVoltageLowTimeoutCnt = 0u;
static INT8U uVoltageLowRecoverCnt = 0u;
static BOOLEAN bVehiclePowerVoltageLow = FALSE;
/*****************************************************************************************
*  Name        : CanDiagApp_DTC_VehiclePowerVoltageLow
*  Description : vehicle power voltage low
                 operate condition: 1)ECU power on 2)KL30 node: battery connect 3)KL15 node: IGN ON
                 DTC set condition: power voltage lower than 9V continue 1s
                 DTC resume condition: power voltage higher than 9.5V continue 1s
*  Parameter   : none
*  Returns     : none 
*****************************************************************************************/
void CanDiagApp_DTC_VehiclePowerVoltageLow(void)
{
	static INT16U uVehicleVoltage = 0u;
	const T_ADC_APP *ptr = AdcApp_SampleData();

	if (FALSE == bVehiclePowerVoltageLow)
	{
		if (ptr->vehiclePowerVoltage < POWER_9V_VOLTAGE)
		{
			uVoltageLowRecoverCnt = 0u;

			uVoltageLowTimeoutCnt++;
			if (uVoltageLowTimeoutCnt >= DELAY_1s)		
			{
				uVoltageLowTimeoutCnt = DELAY_1s;
				bVehiclePowerVoltageLow = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
	            {
					DescMgr_DtcStatusDataReport(DTC_VEHICLE_POWER_VOLTAGE_LOW, DTC_TEST_FAILED);
				}		
			}			
		}
		else
		{
			uVoltageLowTimeoutCnt = 0u;
			uVoltageLowRecoverCnt = 0u;
			bVehiclePowerVoltageLow = FALSE;
		}
	}
	else
	{
		if (ptr->vehiclePowerVoltage >= POWER_9_5V_VOLTAGE)
		{		
			uVoltageLowTimeoutCnt = 0u;

			uVoltageLowRecoverCnt++;
			if (uVoltageLowRecoverCnt >= DELAY_1s)
	        {
				uVoltageLowRecoverCnt = DELAY_1s;
				bVehiclePowerVoltageLow = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
	            {
					DescMgr_DtcStatusDataReport(DTC_VEHICLE_POWER_VOLTAGE_LOW, DTC_TEST_PASSED);
	            }
	        }			
		}
		else
		{
			uVoltageLowTimeoutCnt = 0u;
			uVoltageLowRecoverCnt = 0u;
			bVehiclePowerVoltageLow = TRUE;
		}
	}
}

void CanDescDtc_TaskEntry(void)
{ 
    /* Diagnosis init, dtc record enable delay */
    if(can_diag_init_delay_timer > CAN_DIAG_INIT_DELAY_TIMEOUT)
    {
        can_diag_init_delay_timer--;

        if(can_diag_init_delay_timer == CAN_DIAG_INIT_DELAY_TIMEOUT)
        {
            /* Adjust dtc record enable mask */
            DescMgr_CanDigDtcRecordEnableAdjust();
        }
    }

    DescMgr_CanDiagDtcEepromHandle();

#if 0	
    can_desc_delay_service_monitor();
    Get_Software_Version_Status();
    EEP_Write_Status_Monitor();
    EEP_Status_Monitor();
#endif

	CanDiagApp_DTC_VehiclePowerVoltageLow();
}

/* _END_OF_DESC_DTC_MGR_ */
