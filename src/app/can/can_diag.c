//****************************************************************************
// @Module        Project Settings
// @Filename      can_diag.c
// @Project       P300.dav
//----------------------------------------------------------------------------
// @Controller    Infineon XC2365B-40F80
//
// @Compiler      Keil
//
// @Codegenerator 2.0
//
// @Description   This file contains the project initialization function.
//
//----------------------------------------------------------------------------
// @Date          2020-03-04 15:47:42
//
//****************************************************************************

// USER CODE BEGIN (MAIN_General,1)

// USER CODE END



//****************************************************************************
// @Project Includes
//****************************************************************************

// USER CODE BEGIN (MAIN_General,2)

#include <string.h>

#include "can.h"
#include "can_com.h"
#include "can_diag.h"
#include "desc_mgr.h"
#include "Nm.h"

#define CAN_DIAG_GLOBALS

// USER CODE END


//****************************************************************************
// @Macros
//****************************************************************************

// USER CODE BEGIN (MAIN_General,3)

// USER CODE END


//****************************************************************************
// @Defines
//****************************************************************************

// USER CODE BEGIN (MAIN_General,4)

// USER CODE END


//****************************************************************************
// @Typedefs
//****************************************************************************

// USER CODE BEGIN (MAIN_General,5)

// USER CODE END


//****************************************************************************
// @Imported Global Variables
//****************************************************************************

// USER CODE BEGIN (MAIN_General,6)

// USER CODE END


//****************************************************************************
// @Global Variables
//****************************************************************************

// USER CODE BEGIN (MAIN_General,7)

INT16U  uCanVoltageValue;

CAN_DIAG_STAT CAN_DTC_ERR t_canDtcErr = {0u};

// USER CODE END


//****************************************************************************
// @External Prototypes
//****************************************************************************

// USER CODE BEGIN (MAIN_General,8)

// USER CODE END


//****************************************************************************
// @Prototypes Of Local Functions
//****************************************************************************

// USER CODE BEGIN (MAIN_General,9)

// USER CODE END


//****************************************************************************
// @Function      void CanDiag_DTC_VCU_0x118MsgRollingCount()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x118 message rolling counter.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x118MsgRollingTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x118MsgRollingRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_VCU_0x118MsgRollingCount(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	CAN_DIAG_STAT INT8U uPreRollingValue = 0u;
    CAN_DIAG_STAT INT8U uRollingContinuity = 0u;
	CAN_DIAG_STAT BOOLEAN bFristMsgFlag = FALSE;

  	INT8U uCurRollingValue = (INT8U)((rcvObject[7] & 0xF0) >> 4u);	
	
  	if (FALSE == bFristMsgFlag)
  	{
		bFristMsgFlag = TRUE;
		uRollingContinuity = 0u;
		uPreRollingValue = uCurRollingValue;

		return FALSE;
  	}
	else
	{
		uPreRollingValue++;
		if (15u == uPreRollingValue)
		{
			uPreRollingValue = 0u;
		}

		if (uPreRollingValue == uCurRollingValue)
		{
			uRollingContinuity++;
		}
		else
		{
			bFristMsgFlag = FALSE;
			uRollingContinuity = 0u;
		}

		if (FALSE == t_canDtcErr.b0x118MsgRollingCntErr)
		{
			if (uRollingContinuity < 10u)
			{
				u0x118MsgRollingRecoverCnt = 0u;

				u0x118MsgRollingTimeoutCnt++;
				if (u0x118MsgRollingTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
				{
					u0x118MsgRollingTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
					t_canDtcErr.b0x118MsgRollingCntErr = TRUE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x118_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
					}
				}
			}
			else
			{
				u0x118MsgRollingTimeoutCnt = 0u;
				u0x118MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x118MsgRollingCntErr = FALSE;
			}
		}
		else
		{
			if (uRollingContinuity >= 10u)
			{
				u0x118MsgRollingTimeoutCnt = 0u;

				u0x118MsgRollingRecoverCnt++;
				if (u0x118MsgRollingRecoverCnt >= MESSAGE_RECOVER_CYCLE)
				{
					u0x118MsgRollingRecoverCnt = MESSAGE_RECOVER_CYCLE;
					t_canDtcErr.b0x118MsgRollingCntErr = FALSE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x118_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
					}
				}
			}
			else
			{
				u0x118MsgRollingTimeoutCnt = 0u;
				u0x118MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x118MsgRollingCntErr = TRUE;				
			}
		}

		if (uRollingContinuity >= 10u)
		{
			uRollingContinuity = 10u;
			return TRUE;
		}
		else
		{
			return FALSE;	
		}
	}

  // USER CODE END

} //  End of function CanDiag_DTC_VCU_0x118MsgRollingCount

//****************************************************************************
// @Function      void CanDiag_DTC_VCU_0x119MsgRollingCount()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x119 message rolling counter.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x119MsgRollingTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x119MsgRollingRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_VCU_0x119MsgRollingCount(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	CAN_DIAG_STAT INT8U uPreRollingValue = 0u;
    CAN_DIAG_STAT INT8U uRollingContinuity = 0u;
	CAN_DIAG_STAT BOOLEAN bFristMsgFlag = FALSE;

  	INT8U uCurRollingValue = (INT8U)((rcvObject[7] & 0xF0) >> 4u);	
	
  	if (FALSE == bFristMsgFlag)
  	{
		bFristMsgFlag = TRUE;
		uRollingContinuity = 0u;
		uPreRollingValue = uCurRollingValue;

		return FALSE;
  	}
	else
	{
		uPreRollingValue++;
		if (15u == uPreRollingValue)
		{
			uPreRollingValue = 0u;
		}

		if (uPreRollingValue == uCurRollingValue)
		{
			uRollingContinuity++;
		}
		else
		{
			bFristMsgFlag = FALSE;
			uRollingContinuity = 0u;
		}

		if (FALSE == t_canDtcErr.b0x119MsgRollingCntErr)
		{
			if (uRollingContinuity < 10u)
			{
				u0x119MsgRollingRecoverCnt = 0u;

				u0x119MsgRollingTimeoutCnt++;
				if (u0x119MsgRollingTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
				{
					u0x119MsgRollingTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
					t_canDtcErr.b0x119MsgRollingCntErr = TRUE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x119_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
					}
				}
			}
			else
			{
				u0x119MsgRollingTimeoutCnt = 0u;
				u0x119MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x119MsgRollingCntErr = FALSE;
			}
		}
		else
		{
			if (uRollingContinuity >= 10u)
			{
				u0x119MsgRollingTimeoutCnt = 0u;

				u0x119MsgRollingRecoverCnt++;
				if (u0x119MsgRollingRecoverCnt >= MESSAGE_RECOVER_CYCLE)
				{
					u0x119MsgRollingRecoverCnt = MESSAGE_RECOVER_CYCLE;
					t_canDtcErr.b0x119MsgRollingCntErr = FALSE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x119_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
					}
				}
			}
			else
			{
				u0x119MsgRollingTimeoutCnt = 0u;
				u0x119MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x119MsgRollingCntErr = TRUE;				
			}
		}

		if (uRollingContinuity >= 10u)
		{
			uRollingContinuity = 10u;
			return TRUE;
		}
		else
		{
			return FALSE;	
		}
	}

  // USER CODE END

} //  End of function CanDiag_DTC_VCU_0x119MsgRollingCount

//****************************************************************************
// @Function      void CanDiag_DTC_VCU_0x126MsgRollingCount()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x126 message rolling counter.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x126MsgRollingTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x126MsgRollingRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_VCU_0x126MsgRollingCount(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	CAN_DIAG_STAT INT8U uPreRollingValue = 0u;
    CAN_DIAG_STAT INT8U uRollingContinuity = 0u;
	CAN_DIAG_STAT BOOLEAN bFristMsgFlag = FALSE;

  	INT8U uCurRollingValue = (INT8U)((rcvObject[7] & 0xF0) >> 4u);	
	
  	if (FALSE == bFristMsgFlag)
  	{
		bFristMsgFlag = TRUE;
		uRollingContinuity = 0u;
		uPreRollingValue = uCurRollingValue;

		return FALSE;
  	}
	else
	{
		uPreRollingValue++;
		if (15u == uPreRollingValue)
		{
			uPreRollingValue = 0u;
		}

		if (uPreRollingValue == uCurRollingValue)
		{
			uRollingContinuity++;
		}
		else
		{
			bFristMsgFlag = FALSE;
			uRollingContinuity = 0u;
		}

		if (FALSE == t_canDtcErr.b0x126MsgRollingCntErr)
		{
			if (uRollingContinuity < 10u)
			{
				u0x126MsgRollingRecoverCnt = 0u;

				u0x126MsgRollingTimeoutCnt++;
				if (u0x126MsgRollingTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
				{
					u0x126MsgRollingTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
					t_canDtcErr.b0x126MsgRollingCntErr = TRUE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x126_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
					}
				}
			}
			else
			{
				u0x126MsgRollingTimeoutCnt = 0u;
				u0x126MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x126MsgRollingCntErr = FALSE;
			}
		}
		else
		{
			if (uRollingContinuity >= 10u)
			{
				u0x126MsgRollingTimeoutCnt = 0u;

				u0x126MsgRollingRecoverCnt++;
				if (u0x126MsgRollingRecoverCnt >= MESSAGE_RECOVER_CYCLE)
				{
					u0x126MsgRollingRecoverCnt = MESSAGE_RECOVER_CYCLE;
					t_canDtcErr.b0x126MsgRollingCntErr = FALSE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x126_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
					}
				}
			}
			else
			{
				u0x126MsgRollingTimeoutCnt = 0u;
				u0x126MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x126MsgRollingCntErr = TRUE;				
			}
		}

		if (uRollingContinuity >= 10u)
		{
			uRollingContinuity = 10u;
			return TRUE;
		}
		else
		{
			return FALSE;	
		}
	}

  // USER CODE END

} //  End of function CanDiag_DTC_VCU_0x126MsgRollingCount

//****************************************************************************
// @Function      void CanDiag_DTC_MCU_0x130MsgRollingCount()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x130 message rolling counter.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x130MsgRollingTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x130MsgRollingRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_MCU_0x130MsgRollingCount(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	CAN_DIAG_STAT INT8U uPreRollingValue = 0u;
    CAN_DIAG_STAT INT8U uRollingContinuity = 0u;
	CAN_DIAG_STAT BOOLEAN bFristMsgFlag = FALSE;

  	INT8U uCurRollingValue = (INT8U)((rcvObject[7] & 0xF0) >> 4u);	
	
  	if (FALSE == bFristMsgFlag)
  	{
		bFristMsgFlag = TRUE;
		uRollingContinuity = 0u;
		uPreRollingValue = uCurRollingValue;

		return FALSE;
  	}
	else
	{
		uPreRollingValue++;
		if (15u == uPreRollingValue)
		{
			uPreRollingValue = 0u;
		}

		if (uPreRollingValue == uCurRollingValue)
		{
			uRollingContinuity++;
		}
		else
		{
			bFristMsgFlag = FALSE;
			uRollingContinuity = 0u;
		}

		if (FALSE == t_canDtcErr.b0x130MsgRollingCntErr)
		{
			if (uRollingContinuity < 10u)
			{
				u0x130MsgRollingRecoverCnt = 0u;

				u0x130MsgRollingTimeoutCnt++;
				if (u0x130MsgRollingTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
				{
					u0x130MsgRollingTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
					t_canDtcErr.b0x130MsgRollingCntErr = TRUE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x130_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
					}
				}
			}
			else
			{
				u0x130MsgRollingTimeoutCnt = 0u;
				u0x130MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x130MsgRollingCntErr = FALSE;
			}
		}
		else
		{
			if (uRollingContinuity >= 10u)
			{
				u0x130MsgRollingTimeoutCnt = 0u;

				u0x130MsgRollingRecoverCnt++;
				if (u0x130MsgRollingRecoverCnt >= MESSAGE_RECOVER_CYCLE)
				{
					u0x130MsgRollingRecoverCnt = MESSAGE_RECOVER_CYCLE;
					t_canDtcErr.b0x130MsgRollingCntErr = FALSE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x130_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
					}
				}
			}
			else
			{
				u0x130MsgRollingTimeoutCnt = 0u;
				u0x130MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x130MsgRollingCntErr = TRUE;				
			}
		}

		if (uRollingContinuity >= 10u)
		{
			uRollingContinuity = 10u;
			return TRUE;
		}
		else
		{
			return FALSE;	
		}
	}

  // USER CODE END

} //  End of function CanDiag_DTC_MCU_0x130MsgRollingCount

//****************************************************************************
// @Function      void CanDiag_DTC_MCU_0x132MsgRollingCount()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x132 message rolling counter.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x132MsgRollingTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x132MsgRollingRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_MCU_0x132MsgRollingCount(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	CAN_DIAG_STAT INT8U uPreRollingValue = 0u;
    CAN_DIAG_STAT INT8U uRollingContinuity = 0u;
	CAN_DIAG_STAT BOOLEAN bFristMsgFlag = FALSE;

  	INT8U uCurRollingValue = (INT8U)((rcvObject[7] & 0xF0) >> 4u);	
	
  	if (FALSE == bFristMsgFlag)
  	{
		bFristMsgFlag = TRUE;
		uRollingContinuity = 0u;
		uPreRollingValue = uCurRollingValue;

		return FALSE;
  	}
	else
	{
		uPreRollingValue++;
		if (15u == uPreRollingValue)
		{
			uPreRollingValue = 0u;
		}

		if (uPreRollingValue == uCurRollingValue)
		{
			uRollingContinuity++;
		}
		else
		{
			bFristMsgFlag = FALSE;
			uRollingContinuity = 0u;
		}

		if (FALSE == t_canDtcErr.b0x132MsgRollingCntErr)
		{
			if (uRollingContinuity < 10u)
			{
				u0x132MsgRollingRecoverCnt = 0u;

				u0x132MsgRollingTimeoutCnt++;
				if (u0x132MsgRollingTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
				{
					u0x132MsgRollingTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
					t_canDtcErr.b0x132MsgRollingCntErr = TRUE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x132_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
					}
				}
			}
			else
			{
				u0x132MsgRollingTimeoutCnt = 0u;
				u0x132MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x132MsgRollingCntErr = FALSE;
			}
		}
		else
		{
			if (uRollingContinuity >= 10u)
			{
				u0x132MsgRollingTimeoutCnt = 0u;

				u0x132MsgRollingRecoverCnt++;
				if (u0x132MsgRollingRecoverCnt >= MESSAGE_RECOVER_CYCLE)
				{
					u0x132MsgRollingRecoverCnt = MESSAGE_RECOVER_CYCLE;
					t_canDtcErr.b0x132MsgRollingCntErr = FALSE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x132_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
					}
				}
			}
			else
			{
				u0x132MsgRollingTimeoutCnt = 0u;
				u0x132MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x132MsgRollingCntErr = TRUE;				
			}
		}

		if (uRollingContinuity >= 10u)
		{
			uRollingContinuity = 10u;
			return TRUE;
		}
		else
		{
			return FALSE;	
		}
	}

  // USER CODE END

} //  End of function CanDiag_DTC_MCU_0x132MsgRollingCount

//****************************************************************************
// @Function      void CanDiag_DTC_BMS_0x140MsgRollingCount()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x140 message rolling counter.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x140MsgRollingTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x140MsgRollingRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_BMS_0x140MsgRollingCount(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	CAN_DIAG_STAT INT8U uPreRollingValue = 0u;
    CAN_DIAG_STAT INT8U uRollingContinuity = 0u;
	CAN_DIAG_STAT BOOLEAN bFristMsgFlag = FALSE;

  	INT8U uCurRollingValue = (INT8U)((rcvObject[7] & 0xF0) >> 4u);	
	
  	if (FALSE == bFristMsgFlag)
  	{
		bFristMsgFlag = TRUE;
		uRollingContinuity = 0u;
		uPreRollingValue = uCurRollingValue;

		return FALSE;
  	}
	else
	{
		uPreRollingValue++;
		if (15u == uPreRollingValue)
		{
			uPreRollingValue = 0u;
		}

		if (uPreRollingValue == uCurRollingValue)
		{
			uRollingContinuity++;
		}
		else
		{
			bFristMsgFlag = FALSE;
			uRollingContinuity = 0u;
		}

		if (FALSE == t_canDtcErr.b0x140MsgRollingCntErr)
		{
			if (uRollingContinuity < 10u)
			{
				u0x140MsgRollingRecoverCnt = 0u;

				u0x140MsgRollingTimeoutCnt++;
				if (u0x140MsgRollingTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
				{
					u0x140MsgRollingTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
					t_canDtcErr.b0x140MsgRollingCntErr = TRUE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x140_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
					}
				}
			}
			else
			{
				u0x140MsgRollingTimeoutCnt = 0u;
				u0x140MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x140MsgRollingCntErr = FALSE;
			}
		}
		else
		{
			if (uRollingContinuity >= 10u)
			{
				u0x140MsgRollingTimeoutCnt = 0u;

				u0x140MsgRollingRecoverCnt++;
				if (u0x140MsgRollingRecoverCnt >= MESSAGE_RECOVER_CYCLE)
				{
					u0x140MsgRollingRecoverCnt = MESSAGE_RECOVER_CYCLE;
					t_canDtcErr.b0x140MsgRollingCntErr = FALSE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x140_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
					}
				}
			}
			else
			{
				u0x140MsgRollingTimeoutCnt = 0u;
				u0x140MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x140MsgRollingCntErr = TRUE;				
			}
		}

		if (uRollingContinuity >= 10u)
		{
			uRollingContinuity = 10u;
			return TRUE;
		}
		else
		{
			return FALSE;	
		}
	}

  // USER CODE END

} //  End of function CanDiag_DTC_BMS_0x140MsgRollingCount

//****************************************************************************
// @Function      void CanDiag_DTC_BMS_0x148MsgRollingCount()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x148 message rolling counter.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x148MsgRollingTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x148MsgRollingRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_BMS_0x148MsgRollingCount(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	CAN_DIAG_STAT INT8U uPreRollingValue = 0u;
    CAN_DIAG_STAT INT8U uRollingContinuity = 0u;
	CAN_DIAG_STAT BOOLEAN bFristMsgFlag = FALSE;

  	INT8U uCurRollingValue = (INT8U)((rcvObject[7] & 0xF0) >> 4u);	
	
  	if (FALSE == bFristMsgFlag)
  	{
		bFristMsgFlag = TRUE;
		uRollingContinuity = 0u;
		uPreRollingValue = uCurRollingValue;

		return FALSE;
  	}
	else
	{
		uPreRollingValue++;
		if (15u == uPreRollingValue)
		{
			uPreRollingValue = 0u;
		}

		if (uPreRollingValue == uCurRollingValue)
		{
			uRollingContinuity++;
		}
		else
		{
			bFristMsgFlag = FALSE;
			uRollingContinuity = 0u;
		}

		if (FALSE == t_canDtcErr.b0x148MsgRollingCntErr)
		{
			if (uRollingContinuity < 10u)
			{
				u0x148MsgRollingRecoverCnt = 0u;

				u0x148MsgRollingTimeoutCnt++;
				if (u0x148MsgRollingTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
				{
					u0x148MsgRollingTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
					t_canDtcErr.b0x148MsgRollingCntErr = TRUE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x148_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
					}
				}
			}
			else
			{
				u0x148MsgRollingTimeoutCnt = 0u;
				u0x148MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x148MsgRollingCntErr = FALSE;
			}
		}
		else
		{
			if (uRollingContinuity >= 10u)
			{
				u0x148MsgRollingTimeoutCnt = 0u;

				u0x148MsgRollingRecoverCnt++;
				if (u0x148MsgRollingRecoverCnt >= MESSAGE_RECOVER_CYCLE)
				{
					u0x148MsgRollingRecoverCnt = MESSAGE_RECOVER_CYCLE;
					t_canDtcErr.b0x148MsgRollingCntErr = FALSE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x148_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
					}
				}
			}
			else
			{
				u0x148MsgRollingTimeoutCnt = 0u;
				u0x148MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x148MsgRollingCntErr = TRUE;				
			}
		}

		if (uRollingContinuity >= 10u)
		{
			uRollingContinuity = 10u;
			return TRUE;
		}
		else
		{
			return FALSE;	
		}
	}

  // USER CODE END

} //  End of function CanDiag_DTC_BMS_0x148MsgRollingCount

//****************************************************************************
// @Function      void CanDiag_DTC_BMS_0x155MsgRollingCount()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x155 message rolling counter.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x155MsgRollingTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x155MsgRollingRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_BMS_0x155MsgRollingCount(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	CAN_DIAG_STAT INT8U uPreRollingValue = 0u;
    CAN_DIAG_STAT INT8U uRollingContinuity = 0u;
	CAN_DIAG_STAT BOOLEAN bFristMsgFlag = FALSE;

  	INT8U uCurRollingValue = (INT8U)((rcvObject[7] & 0xF0) >> 4u);	
	
  	if (FALSE == bFristMsgFlag)
  	{
		bFristMsgFlag = TRUE;
		uRollingContinuity = 0u;
		uPreRollingValue = uCurRollingValue;

		return FALSE;
  	}
	else
	{
		uPreRollingValue++;
		if (15u == uPreRollingValue)
		{
			uPreRollingValue = 0u;
		}

		if (uPreRollingValue == uCurRollingValue)
		{
			uRollingContinuity++;
		}
		else
		{
			bFristMsgFlag = FALSE;
			uRollingContinuity = 0u;
		}

		if (FALSE == t_canDtcErr.b0x155MsgRollingCntErr)
		{
			if (uRollingContinuity < 10u)
			{
				u0x155MsgRollingRecoverCnt = 0u;

				u0x155MsgRollingTimeoutCnt++;
				if (u0x155MsgRollingTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
				{
					u0x155MsgRollingTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
					t_canDtcErr.b0x155MsgRollingCntErr = TRUE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x155_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
					}
				}
			}
			else
			{
				u0x155MsgRollingTimeoutCnt = 0u;
				u0x155MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x155MsgRollingCntErr = FALSE;
			}
		}
		else
		{
			if (uRollingContinuity >= 10u)
			{
				u0x155MsgRollingTimeoutCnt = 0u;

				u0x155MsgRollingRecoverCnt++;
				if (u0x155MsgRollingRecoverCnt >= MESSAGE_RECOVER_CYCLE)
				{
					u0x155MsgRollingRecoverCnt = MESSAGE_RECOVER_CYCLE;
					t_canDtcErr.b0x155MsgRollingCntErr = FALSE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x155_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
					}
				}
			}
			else
			{
				u0x155MsgRollingTimeoutCnt = 0u;
				u0x155MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x155MsgRollingCntErr = TRUE;				
			}
		}

		if (uRollingContinuity >= 10u)
		{
			uRollingContinuity = 10u;
			return TRUE;
		}
		else
		{
			return FALSE;	
		}
	}

  // USER CODE END

} //  End of function CanDiag_DTC_BMS_0x155MsgRollingCount

//****************************************************************************
// @Function      void CanDiag_DTC_VCU_0x16eMsgRollingCount()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x16E message rolling counter.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x16eMsgRollingTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x16eMsgRollingRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_VCU_0x16eMsgRollingCount(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	CAN_DIAG_STAT INT8U uPreRollingValue = 0u;
    CAN_DIAG_STAT INT8U uRollingContinuity = 0u;
	CAN_DIAG_STAT BOOLEAN bFristMsgFlag = FALSE;

  	INT8U uCurRollingValue = (INT8U)((rcvObject[7] & 0xF0) >> 4u);	
	
  	if (FALSE == bFristMsgFlag)
  	{
		bFristMsgFlag = TRUE;
		uRollingContinuity = 0u;
		uPreRollingValue = uCurRollingValue;

		return FALSE;
  	}
	else
	{
		uPreRollingValue++;
		if (15u == uPreRollingValue)
		{
			uPreRollingValue = 0u;
		}

		if (uPreRollingValue == uCurRollingValue)
		{
			uRollingContinuity++;
		}
		else
		{
			bFristMsgFlag = FALSE;
			uRollingContinuity = 0u;
		}

		if (FALSE == t_canDtcErr.b0x16eMsgRollingCntErr)
		{
			if (uRollingContinuity < 10u)
			{
				u0x16eMsgRollingRecoverCnt = 0u;

				u0x16eMsgRollingTimeoutCnt++;
				if (u0x16eMsgRollingTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
				{
					u0x16eMsgRollingTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
					t_canDtcErr.b0x16eMsgRollingCntErr = TRUE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x16E_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
					}
				}
			}
			else
			{
				u0x16eMsgRollingTimeoutCnt = 0u;
				u0x16eMsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x16eMsgRollingCntErr = FALSE;
			}
		}
		else
		{
			if (uRollingContinuity >= 10u)
			{
				u0x16eMsgRollingTimeoutCnt = 0u;

				u0x16eMsgRollingRecoverCnt++;
				if (u0x16eMsgRollingRecoverCnt >= MESSAGE_RECOVER_CYCLE)
				{
					u0x16eMsgRollingRecoverCnt = MESSAGE_RECOVER_CYCLE;
					t_canDtcErr.b0x16eMsgRollingCntErr = FALSE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x16E_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
					}
				}
			}
			else
			{
				u0x16eMsgRollingTimeoutCnt = 0u;
				u0x16eMsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x16eMsgRollingCntErr = TRUE;				
			}
		}

		if (uRollingContinuity >= 10u)
		{
			uRollingContinuity = 10u;
			return TRUE;
		}
		else
		{
			return FALSE;	
		}
	}

  // USER CODE END

} //  End of function CanDiag_DTC_VCU_0x16eMsgRollingCount

//****************************************************************************
// @Function      void CanDiag_DTC_VCU_0x16fMsgRollingCount()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x16F message rolling counter.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x16fMsgRollingTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x16fMsgRollingRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_VCU_0x16fMsgRollingCount(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	CAN_DIAG_STAT INT8U uPreRollingValue = 0u;
    CAN_DIAG_STAT INT8U uRollingContinuity = 0u;
	CAN_DIAG_STAT BOOLEAN bFristMsgFlag = FALSE;

  	INT8U uCurRollingValue = (INT8U)((rcvObject[7] & 0xF0) >> 4u);	
	
  	if (FALSE == bFristMsgFlag)
  	{
		bFristMsgFlag = TRUE;
		uRollingContinuity = 0u;
		uPreRollingValue = uCurRollingValue;

		return FALSE;
  	}
	else
	{
		uPreRollingValue++;
		if (15u == uPreRollingValue)
		{
			uPreRollingValue = 0u;
		}

		if (uPreRollingValue == uCurRollingValue)
		{
			uRollingContinuity++;
		}
		else
		{
			bFristMsgFlag = FALSE;
			uRollingContinuity = 0u;
		}

		if (FALSE == t_canDtcErr.b0x16fMsgRollingCntErr)
		{
			if (uRollingContinuity < 10u)
			{
				u0x16fMsgRollingRecoverCnt = 0u;

				u0x16fMsgRollingTimeoutCnt++;
				if (u0x16fMsgRollingTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
				{
					u0x16fMsgRollingTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
					t_canDtcErr.b0x16fMsgRollingCntErr = TRUE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x16F_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
					}
				}
			}
			else
			{
				u0x16fMsgRollingTimeoutCnt = 0u;
				u0x16fMsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x16fMsgRollingCntErr = FALSE;
			}
		}
		else
		{
			if (uRollingContinuity >= 10u)
			{
				u0x16fMsgRollingTimeoutCnt = 0u;

				u0x16fMsgRollingRecoverCnt++;
				if (u0x16fMsgRollingRecoverCnt >= MESSAGE_RECOVER_CYCLE)
				{
					u0x16fMsgRollingRecoverCnt = MESSAGE_RECOVER_CYCLE;
					t_canDtcErr.b0x16fMsgRollingCntErr = FALSE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x16F_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
					}
				}
			}
			else
			{
				u0x16fMsgRollingTimeoutCnt = 0u;
				u0x16fMsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x16fMsgRollingCntErr = TRUE;				
			}
		}

		if (uRollingContinuity >= 10u)
		{
			uRollingContinuity = 10u;
			return TRUE;
		}
		else
		{
			return FALSE;	
		}
	}

  // USER CODE END

} //  End of function CanDiag_DTC_VCU_0x16fMsgRollingCount

//****************************************************************************
// @Function      void CanDiag_DTC_GW_0x278MsgRollingCount()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x278 message rolling counter.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x278MsgRollingTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x278MsgRollingRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_GW_0x278MsgRollingCount(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	CAN_DIAG_STAT INT8U uPreRollingValue = 0u;
    CAN_DIAG_STAT INT8U uRollingContinuity = 0u;
	CAN_DIAG_STAT BOOLEAN bFristMsgFlag = FALSE;

  	INT8U uCurRollingValue = (INT8U)((rcvObject[6] & 0x0F) >> 4u);	
	
  	if (FALSE == bFristMsgFlag)
  	{
		bFristMsgFlag = TRUE;
		uRollingContinuity = 0u;
		uPreRollingValue = uCurRollingValue;

		return FALSE;
  	}
	else
	{
		uPreRollingValue++;
		if (15u == uPreRollingValue)
		{
			uPreRollingValue = 0u;
		}

		if (uPreRollingValue == uCurRollingValue)
		{
			uRollingContinuity++;
		}
		else
		{
			bFristMsgFlag = FALSE;
			uRollingContinuity = 0u;
		}

		if (FALSE == t_canDtcErr.b0x278MsgRollingCntErr)
		{
			if (uRollingContinuity < 10u)
			{
				u0x278MsgRollingRecoverCnt = 0u;

				u0x278MsgRollingTimeoutCnt++;
				if (u0x278MsgRollingTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
				{
					u0x278MsgRollingTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
					t_canDtcErr.b0x278MsgRollingCntErr = TRUE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x278_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
					}
				}
			}
			else
			{
				u0x278MsgRollingTimeoutCnt = 0u;
				u0x278MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x278MsgRollingCntErr = FALSE;
			}
		}
		else
		{
			if (uRollingContinuity >= 10u)
			{
				u0x278MsgRollingTimeoutCnt = 0u;

				u0x278MsgRollingRecoverCnt++;
				if (u0x278MsgRollingRecoverCnt >= MESSAGE_RECOVER_CYCLE)
				{
					u0x278MsgRollingRecoverCnt = MESSAGE_RECOVER_CYCLE;
					t_canDtcErr.b0x278MsgRollingCntErr = FALSE;

					if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
					{
						DescMgr_DtcStatusDataReport(DTC_CAN_0x278_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
					}
				}
			}
			else
			{
				u0x278MsgRollingTimeoutCnt = 0u;
				u0x278MsgRollingRecoverCnt = 0u;
				t_canDtcErr.b0x278MsgRollingCntErr = TRUE;				
			}
		}

		if (uRollingContinuity >= 10u)
		{
			uRollingContinuity = 10u;
			return TRUE;
		}
		else
		{
			return FALSE;	
		}
	}

  // USER CODE END

} //  End of function CanDiag_DTC_GW_0x278MsgRollingCount

//****************************************************************************
// @Function      void CanDiag_DTC_VCU_0x118MsgChecksum()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x118 message checksum.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x118MsgChecksumTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x118MsgChecksumRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_VCU_0x118MsgChecksum(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	INT8U i;
	INT8U uChecksumValue = 0u;

	for (i = 0u; i < 7; i++)
	{
		uChecksumValue ^= rcvObject[i]; 
	}

	if (FALSE == t_canDtcErr.b0x118MsgChecksumErr)
	{
		if (uChecksumValue != (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x118MsgChecksumRecoverCnt = 0u;

			u0x118MsgChecksumTimeoutCnt++;
			if (u0x118MsgChecksumTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				u0x118MsgChecksumTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.b0x118MsgChecksumErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x118_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
				}
			}
		}
		else
		{
			u0x118MsgChecksumTimeoutCnt = 0u;
			u0x118MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x118MsgChecksumErr = FALSE;
		}
	}
	else
	{
		if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x118MsgChecksumTimeoutCnt = 0u;

			u0x118MsgChecksumRecoverCnt++;
			if (u0x118MsgChecksumRecoverCnt >= MESSAGE_RECOVER_CYCLE)
			{
				u0x118MsgChecksumRecoverCnt = MESSAGE_RECOVER_CYCLE;
				t_canDtcErr.b0x118MsgChecksumErr = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x118_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
				}				
			}
		}
		else
		{
			u0x118MsgChecksumTimeoutCnt = 0u;
			u0x118MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x118MsgChecksumErr = TRUE;			
		}
	}

	if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

  // USER CODE END

} //  End of function CanDiag_DTC_VCU_0x118MsgChecksum

//****************************************************************************
// @Function      void CanDiag_DTC_VCU_0x119MsgChecksum()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x119 message checksum.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x119MsgChecksumTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x119MsgChecksumRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_VCU_0x119MsgChecksum(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	INT8U i;
	INT8U uChecksumValue = 0u;

	for (i = 0u; i < 7; i++)
	{
		uChecksumValue ^= rcvObject[i]; 
	}

	if (FALSE == t_canDtcErr.b0x119MsgChecksumErr)
	{
		if (uChecksumValue != (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x119MsgChecksumRecoverCnt = 0u;

			u0x119MsgChecksumTimeoutCnt++;
			if (u0x119MsgChecksumTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				u0x119MsgChecksumTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.b0x119MsgChecksumErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x119_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
				}
			}
		}
		else
		{
			u0x119MsgChecksumTimeoutCnt = 0u;
			u0x119MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x119MsgChecksumErr = FALSE;
		}
	}
	else
	{
		if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x119MsgChecksumTimeoutCnt = 0u;

			u0x119MsgChecksumRecoverCnt++;
			if (u0x119MsgChecksumRecoverCnt >= MESSAGE_RECOVER_CYCLE)
			{
				u0x119MsgChecksumRecoverCnt = MESSAGE_RECOVER_CYCLE;
				t_canDtcErr.b0x119MsgChecksumErr = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x119_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
				}				
			}
		}
		else
		{
			u0x119MsgChecksumTimeoutCnt = 0u;
			u0x119MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x119MsgChecksumErr = TRUE;			
		}
	}

	if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

  // USER CODE END

} //  End of function CanDiag_DTC_VCU_0x119MsgChecksum

//****************************************************************************
// @Function      void CanDiag_DTC_VCU_0x126MsgChecksum()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x126 message checksum.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x126MsgChecksumTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x126MsgChecksumRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_VCU_0x126MsgChecksum(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	INT8U i;
	INT8U uChecksumValue = 0u;

	for (i = 0u; i < 7; i++)
	{
		uChecksumValue ^= rcvObject[i]; 
	}

	if (FALSE == t_canDtcErr.b0x126MsgChecksumErr)
	{
		if (uChecksumValue != (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x126MsgChecksumRecoverCnt = 0u;

			u0x126MsgChecksumTimeoutCnt++;
			if (u0x126MsgChecksumTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				u0x126MsgChecksumTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.b0x126MsgChecksumErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x126_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
				}
			}
		}
		else
		{
			u0x126MsgChecksumTimeoutCnt = 0u;
			u0x126MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x126MsgChecksumErr = FALSE;
		}
	}
	else
	{
		if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x126MsgChecksumTimeoutCnt = 0u;

			u0x126MsgChecksumRecoverCnt++;
			if (u0x126MsgChecksumRecoverCnt >= MESSAGE_RECOVER_CYCLE)
			{
				u0x126MsgChecksumRecoverCnt = MESSAGE_RECOVER_CYCLE;
				t_canDtcErr.b0x126MsgChecksumErr = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x126_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
				}				
			}
		}
		else
		{
			u0x126MsgChecksumTimeoutCnt = 0u;
			u0x126MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x126MsgChecksumErr = TRUE;			
		}
	}

	if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

  // USER CODE END

} //  End of function CanDiag_DTC_VCU_0x126MsgChecksum

//****************************************************************************
// @Function      void CanDiag_DTC_MCU_0x130MsgChecksum()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x130 message checksum.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x130MsgChecksumTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x130MsgChecksumRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_MCU_0x130MsgChecksum(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	INT8U i;
	INT8U uChecksumValue = 0u;

	for (i = 0u; i < 7; i++)
	{
		uChecksumValue ^= rcvObject[i]; 
	}

	if (FALSE == t_canDtcErr.b0x130MsgChecksumErr)
	{
		if (uChecksumValue != (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x130MsgChecksumRecoverCnt = 0u;

			u0x130MsgChecksumTimeoutCnt++;
			if (u0x130MsgChecksumTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				u0x130MsgChecksumTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.b0x130MsgChecksumErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x130_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
				}
			}
		}
		else
		{
			u0x130MsgChecksumTimeoutCnt = 0u;
			u0x130MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x130MsgChecksumErr = FALSE;
		}
	}
	else
	{
		if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x130MsgChecksumTimeoutCnt = 0u;

			u0x130MsgChecksumRecoverCnt++;
			if (u0x130MsgChecksumRecoverCnt >= MESSAGE_RECOVER_CYCLE)
			{
				u0x130MsgChecksumRecoverCnt = MESSAGE_RECOVER_CYCLE;
				t_canDtcErr.b0x130MsgChecksumErr = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x130_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
				}				
			}
		}
		else
		{
			u0x130MsgChecksumTimeoutCnt = 0u;
			u0x130MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x130MsgChecksumErr = TRUE;			
		}
	}

	if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

  // USER CODE END

} //  End of function CanDiag_DTC_MCU_0x130MsgChecksum

//****************************************************************************
// @Function      void CanDiag_DTC_MCU_0x132MsgChecksum()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x132 message checksum.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x132MsgChecksumTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x132MsgChecksumRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_MCU_0x132MsgChecksum(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	INT8U i;
	INT8U uChecksumValue = 0u;

	for (i = 0u; i < 7; i++)
	{
		uChecksumValue ^= rcvObject[i]; 
	}

	if (FALSE == t_canDtcErr.b0x132MsgChecksumErr)
	{
		if (uChecksumValue != (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x132MsgChecksumRecoverCnt = 0u;

			u0x132MsgChecksumTimeoutCnt++;
			if (u0x132MsgChecksumTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				u0x132MsgChecksumTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.b0x132MsgChecksumErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x132_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
				}
			}
		}
		else
		{
			u0x132MsgChecksumTimeoutCnt = 0u;
			u0x132MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x132MsgChecksumErr = FALSE;
		}
	}
	else
	{
		if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x132MsgChecksumTimeoutCnt = 0u;

			u0x132MsgChecksumRecoverCnt++;
			if (u0x132MsgChecksumRecoverCnt >= MESSAGE_RECOVER_CYCLE)
			{
				u0x132MsgChecksumRecoverCnt = MESSAGE_RECOVER_CYCLE;
				t_canDtcErr.b0x132MsgChecksumErr = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x132_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
				}				
			}
		}
		else
		{
			u0x132MsgChecksumTimeoutCnt = 0u;
			u0x132MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x132MsgChecksumErr = TRUE;			
		}
	}

	if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

  // USER CODE END

} //  End of function CanDiag_DTC_MCU_0x132MsgChecksum

//****************************************************************************
// @Function      void CanDiag_DTC_BMS_0x140MsgChecksum()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x140 message checksum.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x140MsgChecksumTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x140MsgChecksumRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_BMS_0x140MsgChecksum(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	INT8U i;
	INT8U uChecksumValue = 0u;

	for (i = 0u; i < 7; i++)
	{
		uChecksumValue ^= rcvObject[i]; 
	}

	if (FALSE == t_canDtcErr.b0x140MsgChecksumErr)
	{
		if (uChecksumValue != (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x140MsgChecksumRecoverCnt = 0u;

			u0x140MsgChecksumTimeoutCnt++;
			if (u0x140MsgChecksumTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				u0x140MsgChecksumTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.b0x140MsgChecksumErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x140_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
				}
			}
		}
		else
		{
			u0x140MsgChecksumTimeoutCnt = 0u;
			u0x140MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x140MsgChecksumErr = FALSE;
		}
	}
	else
	{
		if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x140MsgChecksumTimeoutCnt = 0u;

			u0x140MsgChecksumRecoverCnt++;
			if (u0x140MsgChecksumRecoverCnt >= MESSAGE_RECOVER_CYCLE)
			{
				u0x140MsgChecksumRecoverCnt = MESSAGE_RECOVER_CYCLE;
				t_canDtcErr.b0x140MsgChecksumErr = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x140_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
				}				
			}
		}
		else
		{
			u0x140MsgChecksumTimeoutCnt = 0u;
			u0x140MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x140MsgChecksumErr = TRUE;			
		}
	}

	if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

  // USER CODE END

} //  End of function CanDiag_DTC_BMS_0x140MsgChecksum

//****************************************************************************
// @Function      void CanDiag_DTC_BMS_0x148MsgChecksum()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x148 message checksum.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x148MsgChecksumTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x148MsgChecksumRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_BMS_0x148MsgChecksum(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	INT8U i;
	INT8U uChecksumValue = 0u;

	for (i = 0u; i < 7; i++)
	{
		uChecksumValue ^= rcvObject[i]; 
	}

	if (FALSE == t_canDtcErr.b0x148MsgChecksumErr)
	{
		if (uChecksumValue != (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x148MsgChecksumRecoverCnt = 0u;

			u0x148MsgChecksumTimeoutCnt++;
			if (u0x148MsgChecksumTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				u0x148MsgChecksumTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.b0x148MsgChecksumErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x148_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
				}
			}
		}
		else
		{
			u0x148MsgChecksumTimeoutCnt = 0u;
			u0x148MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x148MsgChecksumErr = FALSE;
		}
	}
	else
	{
		if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x148MsgChecksumTimeoutCnt = 0u;

			u0x148MsgChecksumRecoverCnt++;
			if (u0x148MsgChecksumRecoverCnt >= MESSAGE_RECOVER_CYCLE)
			{
				u0x148MsgChecksumRecoverCnt = MESSAGE_RECOVER_CYCLE;
				t_canDtcErr.b0x148MsgChecksumErr = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x148_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
				}				
			}
		}
		else
		{
			u0x148MsgChecksumTimeoutCnt = 0u;
			u0x148MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x148MsgChecksumErr = TRUE;			
		}
	}

	if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

  // USER CODE END

} //  End of function CanDiag_DTC_BMS_0x148MsgChecksum

//****************************************************************************
// @Function      void CanDiag_DTC_BMS_0x155MsgChecksum()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x155 message checksum.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x155MsgChecksumTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x155MsgChecksumRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_BMS_0x155MsgChecksum(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	INT8U i;
	INT8U uChecksumValue = 0u;

	for (i = 0u; i < 7; i++)
	{
		uChecksumValue ^= rcvObject[i]; 
	}

	if (FALSE == t_canDtcErr.b0x155MsgChecksumErr)
	{
		if (uChecksumValue != (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x155MsgChecksumRecoverCnt = 0u;

			u0x155MsgChecksumTimeoutCnt++;
			if (u0x155MsgChecksumTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				u0x155MsgChecksumTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.b0x155MsgChecksumErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x155_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
				}
			}
		}
		else
		{
			u0x155MsgChecksumTimeoutCnt = 0u;
			u0x155MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x155MsgChecksumErr = FALSE;
		}
	}
	else
	{
		if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x155MsgChecksumTimeoutCnt = 0u;

			u0x155MsgChecksumRecoverCnt++;
			if (u0x155MsgChecksumRecoverCnt >= MESSAGE_RECOVER_CYCLE)
			{
				u0x155MsgChecksumRecoverCnt = MESSAGE_RECOVER_CYCLE;
				t_canDtcErr.b0x155MsgChecksumErr = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x155_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
				}				
			}
		}
		else
		{
			u0x155MsgChecksumTimeoutCnt = 0u;
			u0x155MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x155MsgChecksumErr = TRUE;			
		}
	}

	if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

  // USER CODE END

} //  End of function CanDiag_DTC_BMS_0x155MsgChecksum

//****************************************************************************
// @Function      void CanDiag_DTC_VCU_0x16eMsgChecksum()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x16E message checksum.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x16eMsgChecksumTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x16eMsgChecksumRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_VCU_0x16eMsgChecksum(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	INT8U i;
	INT8U uChecksumValue = 0u;

	for (i = 0u; i < 7; i++)
	{
		uChecksumValue ^= rcvObject[i]; 
	}

	if (FALSE == t_canDtcErr.b0x16eMsgChecksumErr)
	{
		if (uChecksumValue != (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x16eMsgChecksumRecoverCnt = 0u;

			u0x16eMsgChecksumTimeoutCnt++;
			if (u0x16eMsgChecksumTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				u0x16eMsgChecksumTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.b0x16eMsgChecksumErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x16E_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
				}
			}
		}
		else
		{
			u0x16eMsgChecksumTimeoutCnt = 0u;
			u0x16eMsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x16eMsgChecksumErr = FALSE;
		}
	}
	else
	{
		if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x16eMsgChecksumTimeoutCnt = 0u;

			u0x16eMsgChecksumRecoverCnt++;
			if (u0x16eMsgChecksumRecoverCnt >= MESSAGE_RECOVER_CYCLE)
			{
				u0x16eMsgChecksumRecoverCnt = MESSAGE_RECOVER_CYCLE;
				t_canDtcErr.b0x16eMsgChecksumErr = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x16E_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
				}				
			}
		}
		else
		{
			u0x16eMsgChecksumTimeoutCnt = 0u;
			u0x16eMsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x16eMsgChecksumErr = TRUE;			
		}
	}

	if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

  // USER CODE END

} //  End of function CanDiag_DTC_VCU_0x16eMsgChecksum

//****************************************************************************
// @Function      void CanDiag_DTC_VCU_0x16fMsgChecksum()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x16F message checksum.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x16fMsgChecksumTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x16fMsgChecksumRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_VCU_0x16fMsgChecksum(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	INT8U i;
	INT8U uChecksumValue = 0u;

	for (i = 0u; i < 7; i++)
	{
		uChecksumValue ^= rcvObject[i]; 
	}

	if (FALSE == t_canDtcErr.b0x16fMsgChecksumErr)
	{
		if (uChecksumValue != (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x16fMsgChecksumRecoverCnt = 0u;

			u0x16fMsgChecksumTimeoutCnt++;
			if (u0x16fMsgChecksumTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				u0x16fMsgChecksumTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.b0x16fMsgChecksumErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x16F_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
				}
			}
		}
		else
		{
			u0x16fMsgChecksumTimeoutCnt = 0u;
			u0x16fMsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x16fMsgChecksumErr = FALSE;
		}
	}
	else
	{
		if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
		{
			u0x16fMsgChecksumTimeoutCnt = 0u;

			u0x16fMsgChecksumRecoverCnt++;
			if (u0x16fMsgChecksumRecoverCnt >= MESSAGE_RECOVER_CYCLE)
			{
				u0x16fMsgChecksumRecoverCnt = MESSAGE_RECOVER_CYCLE;
				t_canDtcErr.b0x16fMsgChecksumErr = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x16F_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
				}				
			}
		}
		else
		{
			u0x16fMsgChecksumTimeoutCnt = 0u;
			u0x16fMsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x16fMsgChecksumErr = TRUE;			
		}
	}

	if (uChecksumValue == (INT8U)(rcvObject[7] & 0x0F))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

  // USER CODE END

} //  End of function CanDiag_DTC_VCU_0x16fMsgChecksum

//****************************************************************************
// @Function      void CanDiag_DTC_GW_0x278MsgChecksum()
//
//----------------------------------------------------------------------------
// @Description   This function check 0x278 message checksum.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U u0x278MsgChecksumTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U u0x278MsgChecksumRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_GW_0x278MsgChecksum(CanReceiveHandle* rcvObject)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	INT8U i;
	INT8U uChecksumValue = 0u;

	for (i = 0u; i < 7; i++)
	{
		uChecksumValue ^= rcvObject[i]; 
	}

	if (FALSE == t_canDtcErr.b0x278MsgChecksumErr)
	{
		if (uChecksumValue != (INT8U)(rcvObject[7]))
		{
			u0x278MsgChecksumRecoverCnt = 0u;

			u0x278MsgChecksumTimeoutCnt++;
			if (u0x278MsgChecksumTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				u0x278MsgChecksumTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.b0x278MsgChecksumErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x278_CHECKSUM_ROLLING_ERR, DTC_TEST_FAILED);
				}
			}
		}
		else
		{
			u0x278MsgChecksumTimeoutCnt = 0u;
			u0x278MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x278MsgChecksumErr = FALSE;
		}
	}
	else
	{
		if (uChecksumValue == (INT8U)(rcvObject[7]))
		{
			u0x278MsgChecksumTimeoutCnt = 0u;

			u0x278MsgChecksumRecoverCnt++;
			if (u0x278MsgChecksumRecoverCnt >= MESSAGE_RECOVER_CYCLE)
			{
				u0x278MsgChecksumRecoverCnt = MESSAGE_RECOVER_CYCLE;
				t_canDtcErr.b0x278MsgChecksumErr = FALSE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_CAN_0x278_CHECKSUM_ROLLING_ERR, DTC_TEST_PASSED);
				}				
			}
		}
		else
		{
			u0x278MsgChecksumTimeoutCnt = 0u;
			u0x278MsgChecksumRecoverCnt = 0u;
			t_canDtcErr.b0x278MsgChecksumErr = TRUE;			
		}
	}

	if (uChecksumValue == (INT8U)(rcvObject[7]))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

  // USER CODE END

} //  End of function CanDiag_DTC_GW_0x278MsgChecksum

//****************************************************************************
// @Function      void CanDiag_DTC_BusOff()
//
//----------------------------------------------------------------------------
// @Description   This function check can bus off.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U uCanBusOffTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U uCanBusOffRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_DTC_BusOff(void)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
	
	if (FALSE == t_canDtcErr.bCanBusOffErr)
	{
		uCanBusOffRecoverCnt = 0u;

		if (BUSOFF_SHUTDOWN_ERR == CanNm_GetBusoffStatus())
		{
			uCanBusOffTimeoutCnt++;
			if (uCanBusOffTimeoutCnt >= MESSAGE_TIMEOUT_CYCLE)
			{
				uCanBusOffTimeoutCnt = MESSAGE_TIMEOUT_CYCLE;
				t_canDtcErr.bCanBusOffErr = TRUE;

				if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
				{
					DescMgr_DtcStatusDataReport(DTC_VEHICLE_CAN_BUS_OFF, DTC_TEST_FAILED);
				}				
			}
		}
		else
		{
			uCanBusOffTimeoutCnt = 0u;
			uCanBusOffRecoverCnt = 0u;
			t_canDtcErr.bCanBusOffErr = FALSE;
		}
	}
	else
	{
		uCanBusOffTimeoutCnt = 0u;

		uCanBusOffRecoverCnt++;
		if (BUSOFF_NORMAL == CanNm_GetBusoffStatus())
		{
			uCanBusOffRecoverCnt = MESSAGE_RECOVER_CYCLE;
			t_canDtcErr.bCanBusOffErr = FALSE;

			if (TRUE == DescMgr_CanDiagDtcRecordBodyEnableCheck())
			{
				DescMgr_DtcStatusDataReport(DTC_VEHICLE_CAN_BUS_OFF, DTC_TEST_PASSED);
			}			
			
		}
		else
		{
			uCanBusOffTimeoutCnt = 0u;
			uCanBusOffRecoverCnt = 0u;
			t_canDtcErr.bCanBusOffErr = TRUE;			
		}
	}

	return t_canDtcErr.bCanBusOffErr;

  // USER CODE END

} //  End of function CanDiag_DTC_BusOff

//****************************************************************************
// @Function      void CanDiag_ComVoltageAbnormalStatus()
//
//----------------------------------------------------------------------------
// @Description   This function check can communication status.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT INT8U uCanVoltageAbnormalTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U uCanVoltageAbnormalRecoverCnt = 0u;

// USER CODE END

BOOLEAN CanDiag_ComVoltageAbnormalStatus(void)
{

  // USER CODE BEGIN (If_TxConfirmation,2)
  
	if (FALSE == t_canDtcErr.bCanVoltageErr)
	{
		uCanVoltageAbnormalRecoverCnt = 0u;

		if (uCanVoltageValue > 0u)
		{
			if ((uCanVoltageValue < CAN_VOLT_FALLING_LOW_THREHOLD) || (uCanVoltageValue > CAN_VOLT_RISING_HIGH_THREHOLD))
			{
				uCanVoltageAbnormalTimeoutCnt++;
				if (uCanVoltageAbnormalTimeoutCnt >= DIAG_ERR_TIMEOUT_CYCLE)
				{
					uCanVoltageAbnormalTimeoutCnt = DIAG_ERR_TIMEOUT_CYCLE;
					t_canDtcErr.bCanVoltageErr = TRUE;

					Can_DisableControllerInterrupts(CAN_CONTROLLER0);
					Can_DisableControllerInterrupts(CAN_CONTROLLER1);

					Can_SetControllerMode(CAN_CONTROLLER0, CAN_T_STOP);
					Can_SetControllerMode(CAN_CONTROLLER1, CAN_T_STOP);
				}
			}
			else
			{
				uCanVoltageAbnormalTimeoutCnt = 0u;
				uCanVoltageAbnormalRecoverCnt = 0u;	
				t_canDtcErr.bCanVoltageErr = FALSE;		
			}		
		}
	}
	else
	{
		uCanVoltageAbnormalTimeoutCnt = 0u;

		if (uCanVoltageValue > 0u)
		{
			if ((uCanVoltageValue > CAN_VOLT_FALLING_HIGH_THREHOLD) && (uCanVoltageValue < CAN_VOLT_RISING_LOW_THREHOLD))
			{
				uCanVoltageAbnormalRecoverCnt++;
				if (uCanVoltageAbnormalRecoverCnt >= DIAG_ERR_RECOVER_CYCLE)
				{
					uCanVoltageAbnormalRecoverCnt = DIAG_ERR_RECOVER_CYCLE;
					t_canDtcErr.bCanVoltageErr = FALSE;
			
					Can_SetControllerMode(CAN_CONTROLLER0, CAN_T_START);
					Can_SetControllerMode(CAN_CONTROLLER1, CAN_T_START);					
			
					Can_EnableControllerInterrupts(CAN_CONTROLLER0);
					Can_EnableControllerInterrupts(CAN_CONTROLLER1);			
				}
			}
			else
			{
				uCanVoltageAbnormalTimeoutCnt = 0u;
				uCanVoltageAbnormalRecoverCnt = 0u; 
				t_canDtcErr.bCanVoltageErr = TRUE;				
			}
		}
	}

  return t_canDtcErr.bCanVoltageErr;

  // USER CODE END

} //  End of function CanDiag_ComVoltageAbnormalStatus

//****************************************************************************
// @Function      void CanDiag_ComCircuitAbnormalStatus()
//
//----------------------------------------------------------------------------
// @Description   This function check can communication status.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None 
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

CAN_DIAG_STAT BOOLEAN bCan0GetErrorIncrement = 0u;
CAN_DIAG_STAT BOOLEAN bCan0GetErrorTransferDirection = 0u;

CAN_DIAG_STAT INT8U uCanCircuitAbnormalTimeoutCnt = 0u;
CAN_DIAG_STAT INT8U uCanCircuitAbnormalRecoverCnt = 0u;

// USER CODE END

BOOLEAN  CanDiag_ComCircuitAbnormalStatus(void)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	bCan0GetErrorIncrement = CAN_ubGetErrorIncrement(CAN_CONTROLLER0);
	bCan0GetErrorTransferDirection = CAN_ubGetErrorTransferDirection(CAN_CONTROLLER0);

	if (FALSE == t_canDtcErr.bCanCircuitErr)
	{
		uCanCircuitAbnormalRecoverCnt = 0u;

		if ((FALSE == bCan0GetErrorTransferDirection) && (FALSE == bCan0GetErrorIncrement))
		{
			uCanCircuitAbnormalTimeoutCnt++;
			if (uCanCircuitAbnormalTimeoutCnt >= DIAG_ERR_TIMEOUT_CYCLE)
			{
				uCanCircuitAbnormalTimeoutCnt = DIAG_ERR_TIMEOUT_CYCLE;
				t_canDtcErr.bCanCircuitErr = TRUE;

				Can_DisableControllerInterrupts(CAN_CONTROLLER0);
				Can_SetControllerMode(CAN_CONTROLLER0, CAN_T_STOP);
			}
		}
		else
		{
			uCanCircuitAbnormalTimeoutCnt = 0u;
			uCanCircuitAbnormalRecoverCnt = 0u;	
			t_canDtcErr.bCanCircuitErr = FALSE;		
		}
	}
	else
	{
		uCanCircuitAbnormalTimeoutCnt = 0u;

		uCanCircuitAbnormalRecoverCnt++;
		if (uCanCircuitAbnormalRecoverCnt >= DIAG_ERR_RECOVER_CYCLE)
		{
			uCanCircuitAbnormalRecoverCnt = DIAG_ERR_RECOVER_CYCLE;
			t_canDtcErr.bCanCircuitErr = FALSE;

			Can_SetControllerMode(CAN_CONTROLLER0, CAN_T_START);					
			Can_EnableControllerInterrupts(CAN_CONTROLLER0);			
		}
	}
	
  return t_canDtcErr.bCanCircuitErr;	

  // USER CODE END

} //  End of function CanDiag_ComCircuitAbnormalStatus


