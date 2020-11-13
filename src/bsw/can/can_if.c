//****************************************************************************
// @Module        Project Settings
// @Filename      can_if.c
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

#include "can.h"
#include "can_if.h"
#include "can_com.h"
#include "desc_ser.h"
#include "IO.h"
#include "MAIN.h"
#include "rte.h"

#define CAN_IF_GLOBALS

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
// @Function      void CanEnbale_v_g(voif) 
//
//----------------------------------------------------------------------------
// @Description   This function enable can transceiver.
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

// USER CODE END

void CanEnbale_v_g(void)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	IO_vSetPin(IO_P1_5); // Enable Can0 tracv enter normal mode (STB)
	IO_vSetPin(IO_P1_4); // Enable Can1 tracv enter normal mode (STB)
	
  // USER CODE END

} //  End of function CanEnbale_v_g

//****************************************************************************
// @Function      void CanSendMsg_v_g(ubyte ubObjNr, ubyte *ubpubData, ubyte  
//                Len) 
//
//----------------------------------------------------------------------------
// @Description   This function indicates the L-SDU has already been copied 
//                from the CAN hardware (or shadow) buffer to the destination 
//                bufferby the destination layer.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    Hrh: 
//                Hardware Object Index of the message object (0-63)
// @Parameters    Identifier: 
//                Message Object Identifier
// @Parameters    CanDlc: 
//                Message Object Data Length
// @Parameters    *CanSduPtr: 
//                Message Object Data Pointer
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

// USER CODE END

void CanSendMsg_v_g(ubyte ubObjNr, ubyte *ubpubData)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	CAN_vLoadData(ubObjNr,ubpubData);
	CAN_vTransmit(ubObjNr); 

  // USER CODE END

} //  End of function CanSendMsg_v_g

//****************************************************************************
// @Function      ubyte CanRecMsg_v_g(ubyte ubObjNr, ubyte *ubpubData, ubyte  
//                *Len) 
//
//----------------------------------------------------------------------------
// @Description   This function indicates the L-SDU has already been copied 
//                from the CAN hardware (or shadow) buffer to the destination 
//                bufferby the destination layer.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    Hrh: 
//                Hardware Object Index of the message object (0-63)
// @Parameters    Identifier: 
//                Message Object Identifier
// @Parameters    CanDlc: 
//                Message Object Data Length
// @Parameters    *CanSduPtr: 
//                Message Object Data Pointer
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

// USER CODE END

ubyte CanRecMsg_v_g(ubyte ubObjNr, ubyte *ubpubData, ubyte *Len)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	ubyte i;
  
	if (CAN_ubNewData(ubObjNr))
	{
		Len[0]= (ubyte)((CAN_HWOBJ[ubObjNr].uwMOFCRH & 0x0F00) >> 8); 
		
		for (i=0u; i<=Len[0]; i++)
		{
			ubpubData[i]=CAN_HWOBJ[ubObjNr].ubData[i];
		}

		CAN_HWOBJ[ubObjNr].uwMOCTRL = 0x0008;
		return 1;
	}
	
	return 0;

  // USER CODE END

} //  End of function CanRecMsg_v_g

//****************************************************************************
// @Function      void CanIf_RxIndication(ubyte Hrh, ulong Identifier, ubyte 
//                CanDlc, ubyte *CanSduPtr) 
//
//----------------------------------------------------------------------------
// @Description   This function indicates the L-SDU has already been copied 
//                from the CAN hardware (or shadow) buffer to the destination 
//                bufferby the destination layer.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    Hrh: 
//                Hardware Object Index of the message object (0-63)
// @Parameters    Identifier: 
//                Message Object Identifier
// @Parameters    CanDlc: 
//                Message Object Data Length
// @Parameters    *CanSduPtr: 
//                Message Object Data Pointer
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_RxIndication,1)

// USER CODE END

void CanIf_RxIndication0(ubyte Hrh, ulong Identifier, ubyte CanDlc, ubyte *CanSduPtr)
{

  // USER CODE BEGIN (If_RxIndication,2)

	ubyte i = 0u;
  	CanSvcInst *ptr_can0SvcInst = CanCom_ApplCan0SvcInstAccess();

	if (Hrh < 15)
	{
		if (CanDlc == ptr_can0SvcInst[Hrh].msgLen)
		{
			if (NULL != ptr_can0SvcInst[Hrh].mainHandler) 			
			{
				if((TRUE == descNmFrameRxEnable) && (TRUE == descApplFrameRxEnable))
				{
					CanCom_ApplCan0MsgResetTimeoutCnt(Hrh);
					ptr_can0SvcInst[Hrh].mainHandler(CanSduPtr);
				}
				else
				{
					if (FALSE == descNmFrameRxEnable)
					{
						if ((ptr_can0SvcInst[Hrh].canId & 0x600) != 0x600)
						{
							CanCom_ApplCan0MsgResetTimeoutCnt(Hrh);
							ptr_can0SvcInst[Hrh].mainHandler(CanSduPtr);
						}
					}
					
					if (FALSE == descApplFrameRxEnable)
					{
						if (((ptr_can0SvcInst[Hrh].canId & 0x600) == 0x600) || ((ptr_can0SvcInst[Hrh].canId & 0x700) == 0x700))
						{
							CanCom_ApplCan0MsgResetTimeoutCnt(Hrh);
							ptr_can0SvcInst[Hrh].mainHandler(CanSduPtr);
						}
					}
				}
			}
		}
	}
	else if(15 == Hrh)
	{
		for (i = 15u; i < kCan0SvcInstNumItems; i++)
		{
			if (Identifier == ptr_can0SvcInst[i].canId)
			{		
				if (CanDlc == ptr_can0SvcInst[i].msgLen)
				{
					if (NULL != ptr_can0SvcInst[i].mainHandler) 			
					{
						if (FALSE == descNmFrameRxEnable)
						{
							if ((ptr_can0SvcInst[i].canId & 0x600) != 0x600)
							{
								CanCom_ApplCan0MsgResetTimeoutCnt(i);
								ptr_can0SvcInst[i].mainHandler(CanSduPtr);
								
								break;
							}
						}
		
						if (FALSE == descApplFrameRxEnable)
						{
							if (((ptr_can0SvcInst[i].canId & 0x600) == 0x600) || ((ptr_can0SvcInst[i].canId & 0x700) == 0x700))
							{
								CanCom_ApplCan0MsgResetTimeoutCnt(i);
								ptr_can0SvcInst[i].mainHandler(CanSduPtr);
								
								break;
							}
						}
		
						CanCom_ApplCan0MsgResetTimeoutCnt(i);
						ptr_can0SvcInst[i].mainHandler(CanSduPtr);
						
					}	
				}
			}
		}
	}

  // USER CODE END

} //  End of function CanIf_RxIndication0


void CanIf_RxIndication1(ubyte Hrh, ulong Identifier, ubyte CanDlc, ubyte *CanSduPtr)
{

  // USER CODE BEGIN (If_RxIndication,2)

	ubyte i = 0u;
  	CanSvcInst *ptr_can1SvcInst = CanCom_ApplCan1SvcInstAccess();

	if ((Hrh > 31) && (Hrh < 47))
	{
		if (CanDlc == ptr_can1SvcInst[Hrh - 32].msgLen)
		{
			if (NULL != ptr_can1SvcInst[Hrh - 32].mainHandler) 			
			{			
				CanCom_ApplCan1MsgResetTimeoutCnt(Hrh - 32);
				ptr_can1SvcInst[Hrh - 32].mainHandler(CanSduPtr);				
			}
		}
	}
	else if (47 == Hrh)
	{
		for (i = 15u; i < kCan1SvcInstNumItems; i++)
		{
			if (Identifier == ptr_can1SvcInst[i].canId)
			{		
				if (CanDlc == ptr_can1SvcInst[i].msgLen)
				{
					if (NULL != ptr_can1SvcInst[i].mainHandler) 			
					{			
						CanCom_ApplCan1MsgResetTimeoutCnt(i);
						ptr_can1SvcInst[i].mainHandler(CanSduPtr);						
					}
				}
			}
		}			
	}

  // USER CODE END

} //  End of function CanIf_RxIndication1


//****************************************************************************
// @Prototypes Of Local Functions
//****************************************************************************

// USER CODE BEGIN (MAIN_General,9)

// USER CODE END

//****************************************************************************
// @Function      void CanIf_TxConfirmation(ubyte SwPduHandle) 
//
//----------------------------------------------------------------------------
// @Description   This function Indicates a successful transmission.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    SwPduHandle: 
//                Softare Pdu Handle
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_TxConfirmation,1)

// USER CODE END

void CanIf_TxConfirmation(ubyte SwPduHandle)
{

  // USER CODE BEGIN (If_TxConfirmation,2)

	switch (SwPduHandle)
	{
		case 16:
		case 48:
		{
			if(TRUE == g_ecuResetFlag)
			{
				g_ecuResetFlag = FALSE;

				/* reset instruction */
				MAIN_vUnlockProtecReg();
				SCU_SWRSTCON &= 0x00FF;
				SCU_RSTCON0 |= 0xC000; //clearn SCU_RSTCON0.SW = 0x11
				SCU_SWRSTCON |= 0x0001; //SCU_SWRSTCON.SWBOOT = 1
				SCU_SWRSTCON |= 0x0002; //SCU_SWRSTCON.SWRSTREQ = 1;
				while(1);
			}
			
			break;
		}
			
		default:
			break;
	}

  // USER CODE END

} //  End of function CanIf_TxConfirmation

//****************************************************************************
// @Function      void CanIf_ControllerBusOff(ubyte Controller) 
//
//----------------------------------------------------------------------------
// @Description   This function Indicates that the controller went in bus-off 
//                mode.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    Controller: 
//                CAN Controller for which bus-off state is requested. Number 
//                of node ( 0 - 3)
//
//----------------------------------------------------------------------------
// @Date          2020-03-04
//
//****************************************************************************

// USER CODE BEGIN (If_ControllerBusOff,1)

// USER CODE END

void CanIf_ControllerBusOff(ubyte Controller)
{

  // USER CODE BEGIN (If_ControllerBusOff,2)

  // USER CODE END  

	switch (Controller)
	{
		case CAN_CONTROLLER0:

			// USER CODE BEGIN (If_ControllerBusOff,3)

			//Can_DisableControllerInterrupts(CAN_CONTROLLER0);

			Can_SetControllerMode(CAN_CONTROLLER0, CAN_T_START);	
			
			// USER CODE END
		
			break;

		case CAN_CONTROLLER1:

			// USER CODE BEGIN (If_ControllerBusOff,4)

			//Can_DisableControllerInterrupts(CAN_CONTROLLER1);

			Can_SetControllerMode(CAN_CONTROLLER1, CAN_T_START);			
			
			// USER CODE END
			
			break;

		default:
		
		// USER CODE BEGIN (If_ControllerBusOff,5)
		
		// USER CODE END
		
		  break;

	}

    // USER CODE BEGIN (SetControllerMode,7)

    // USER CODE END

} //  End of function CanIf_ControllerBusOff

