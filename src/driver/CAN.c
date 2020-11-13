//****************************************************************************
// @Module        MultiCAN Module (CAN)
// @Filename      CAN.c
// @Project       P300.dav
//----------------------------------------------------------------------------
// @Controller    Infineon XC2365B-40F80
//
// @Compiler      Keil
//
// @Codegenerator 2.0
//
// @Description   This file contains functions that use the CAN module.
//
//----------------------------------------------------------------------------
// @Date          2020/5/11 17:54:11
//
//****************************************************************************



// USER CODE BEGIN (CAN_General,1)

// USER CODE END



//****************************************************************************
// @Project Includes
//****************************************************************************

#include "MAIN.h"

// USER CODE BEGIN (CAN_General,2)

#include "can.h"
#include "can_if.h"
#include "can_com.h"
// USER CODE END


//****************************************************************************
// @Macros
//****************************************************************************

// USER CODE BEGIN (CAN_General,3)

// USER CODE END


//****************************************************************************
// @Defines
//****************************************************************************

// USER CODE BEGIN (CAN_General,4)

// USER CODE END


//****************************************************************************
// @Typedefs
//****************************************************************************

// USER CODE BEGIN (CAN_General,5)

// USER CODE END


//****************************************************************************
// @Imported Global Variables
//****************************************************************************

// USER CODE BEGIN (CAN_General,6)

// USER CODE END


//****************************************************************************
// @Global Variables
//****************************************************************************

static ubyte DataPointer_0[8];
static ulong MessageId_0;
static ubyte DataPointer_32[8];
static ulong MessageId_32;


// USER CODE BEGIN (CAN_General,7)

// USER CODE END


//****************************************************************************
// @External Prototypes
//****************************************************************************

// USER CODE BEGIN (CAN_General,8)

// USER CODE END


//****************************************************************************
// @Prototypes Of Local Functions
//****************************************************************************

// USER CODE BEGIN (CAN_General,9)

// USER CODE END


//****************************************************************************
// @Function      void CAN_vInit(void) 
//
//----------------------------------------------------------------------------
// @Description   This is the initialization function of the CAN function 
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
// @Date          2020/5/11
//
//****************************************************************************

// USER CODE BEGIN (Init,1)

// USER CODE END

void CAN_vInit(void)
{
	volatile unsigned int uwTemp;

	INT8U i = 0;
	INT16U objectIndex = 0;
	
	INT8U can0SvcObjectNumber = 0;
	INT8U can0SendObjectNumber = 0;
	INT8U can1SvcObjectNumber = 0;
	INT8U can1SendObjectNumber = 0;
	
	const CanSvcInst *ptr_can0SvcInst = CanCom_ApplCan0SvcInstAccess();
	const CanSendInst *ptr_can0SendInst = CanCom_ApplCan0SendInstAccess();
  	const CanSvcInst *ptr_can1SvcInst = CanCom_ApplCan1SvcInstAccess();
	const CanSendInst *ptr_can1SendInst = CanCom_ApplCan1SendInstAccess();
	
	can0SvcObjectNumber = kCan0SvcInstNumItems > 16 ? 16 : kCan0SvcInstNumItems;
	can0SendObjectNumber = kCan0SendInstNumItems > 16 ? 16 : kCan0SendInstNumItems;
	can1SvcObjectNumber = kCan1SvcInstNumItems > 16 ? 16 : kCan1SvcInstNumItems;
	can1SendObjectNumber = kCan1SendInstNumItems > 16 ? 16 : kCan1SendInstNumItems;
	
  // USER CODE BEGIN (Init,2)

  // USER CODE END

  ///  -----------------------------------------------------------------------
  ///  Configuration of Kernel State Configuration Register:
  ///  -----------------------------------------------------------------------
  ///  - Enable the CAN module(MODEN)
  ///  - Enable Bit Protection for MODEN

    MCAN_KSCCFG  =  0x0003;      // load Kernel State Configuration Register

    uwTemp       =  MCAN_KSCCFG; // dummy read to avoid pipeline effects

  ///  -----------------------------------------------------------------------
  ///  Configuration of the Module Clock:
  ///  -----------------------------------------------------------------------
  ///  - the CAN module clock = 40.00 MHz
  ///  - Normal divider mode selected

    CAN_FDRL     =  0x43FE;      // load Fractional Divider Register

    uwTemp       =  CAN_FDRL;    // dummy read to avoid pipeline effects

  ///  -----------------------------------------------------------------------
  ///  Panel Control
  ///  -----------------------------------------------------------------------
  ///  - wait until Panel has finished the initialisation

    while(CAN_PANCTRL & CAN_PANCTR_BUSY){ // wait until Panel has finished 
                                          // the initialisation
      }                         

  ///  -----------------------------------------------------------------------
  ///  Configuration of CAN Node 0:
  ///  -----------------------------------------------------------------------

  ///  General Configuration of the Node 0:
  ///  - set INIT and CCE
  ///  - enable interrupt generation upon a change of BOFF or EWARN or upon a 
  ///    set of LLE, LOE or INIT
  ///  - alert interrupt node pointer: MultiCAN SRN 2

    CAN_NCR0     =  0x0049;      // load NODE 0 control register[15-0]

  ///  - load NODE 0 interrupt pointer register

    CAN_NIPR0    =  0x0002;      // load NIPR0_LECINP, ALINP, CFCINP and TRINP

  ///  Configuration of the used CAN Port Pins:
  ///  - Loop-back mode is disabled
  ///  - P2.0 is used for CAN0 Receive input(RXDC0C)
  ///  - P2.1 is used for CAN0 Transmit output(TXDC0C)

  P2_IOCR01 = 0x0090;    //set direction register
    CAN_NPCR0    =  0x0002;      // load node0 port control register


  ///  Configuration of the Node 0 Baud Rate:
  ///  - required baud rate = 500.000 kbaud
  ///  - real baud rate     = 500.000 kbaud
  ///  - sample point       = 60.00 %
  ///  - there are 5 time quanta before sample point
  ///  - there are 4 time quanta after sample point
  ///  - the (re)synchronization jump width is 2 time quanta

    CAN_NBTR0L   =  0x3447;      // load NBTR0_DIV8, TSEG2, TSEG1, SJW and BRP

  ///  Configuration of the Node 0 Error Counter:
  ///  - the error warning threshold value (warning level) is 96

    CAN_NECNT0H = 0x0060;        // load NECNT0_EWRNLVL register
    CAN_NECNT0L = 0x0000;       

  ///  Configuration of the Frame Counter:
  ///  - Frame Counter Mode: the counter is incremented upon the reception 
  ///    and transmission of frames
  ///  - frame counter: 0x0000

    CAN_NFCR0H = 0x0000;         // load NFCR0_CFCOV, CFCIE, CFMOD, CFSEL
    CAN_NFCR0L = 0x0000;         // load NFCR0_CFC

  ///  -----------------------------------------------------------------------
  ///  Configuration of CAN Node 1:
  ///  -----------------------------------------------------------------------

  ///  General Configuration of the Node 1:
  ///  - set INIT and CCE
  ///  - enable interrupt generation upon a change of BOFF or EWARN or upon a 
  ///    set of LLE, LOE or INIT
  ///  - alert interrupt node pointer: MultiCAN SRN 3

    CAN_NCR1     =  0x0049;      // load NODE 1 control register[15-0]

  ///  - load NODE 1 interrupt pointer register

    CAN_NIPR1    =  0x0003;      // load NIPR1_LECINP, ALINP, CFCINP and TRINP

  ///  Configuration of the used CAN Port Pins:
  ///  - Loop-back mode is disabled
  ///  - P2.4 is used for CAN1 Receive input(RXDC1A)
  ///  - P2.2 is used for CAN1 Transmit output(TXDC1B)

  P2_IOCR02 = 0x0090;    //set direction register
    CAN_NPCR1    =  0x0000;      // load node1 port control register


  ///  Configuration of the Node 1 Baud Rate:
  ///  - required baud rate = 500.000 kbaud
  ///  - real baud rate     = 500.000 kbaud
  ///  - sample point       = 60.00 %
  ///  - there are 5 time quanta before sample point
  ///  - there are 4 time quanta after sample point
  ///  - the (re)synchronization jump width is 2 time quanta

    CAN_NBTR1L   =  0x3440 | (4000 / Can1BaudRate - 1);      // load NBTR1_DIV8, TSEG2, TSEG1, SJW and BRP

  ///  Configuration of the Node 1 Error Counter:
  ///  - the error warning threshold value (warning level) is 96

    CAN_NECNT1H = 0x0060;        // load NECNT1_EWRNLVL register
    CAN_NECNT1L = 0x0000;       

  ///  Configuration of the Frame Counter:
  ///  - Frame Counter Mode: the counter is incremented upon the reception 
  ///    and transmission of frames
  ///  - frame counter: 0x0000

    CAN_NFCR1H = 0x0000;         // load NFCR1_CFCOV, CFCIE, CFMOD, CFSEL
    CAN_NFCR1L = 0x0000;         // load NFCR1_CFC

  ///  -----------------------------------------------------------------------
  ///  Configuration of CAN Node 2:
  ///  -----------------------------------------------------------------------

  ///  General Configuration of the Node 2:
  ///  - set INIT and CCE

    CAN_NCR2     =  0x0041;      // load NODE 2 control register[15-0]

  ///  -----------------------------------------------------------------------
  ///  Configuration of the CAN Message Object List Structure:
  ///  -----------------------------------------------------------------------

  ///  Allocate MOs for list 1:

    SetListCommand(0x0100,0x0002);  // MO0 for list 1 (Node 0)

    SetListCommand(0x0101,0x0002);  // MO1 for list 1 (Node 0)

    SetListCommand(0x0102,0x0002);  // MO2 for list 1 (Node 0)

    SetListCommand(0x0103,0x0002);  // MO3 for list 1 (Node 0)

    SetListCommand(0x0104,0x0002);  // MO4 for list 1 (Node 0)

    SetListCommand(0x0105,0x0002);  // MO5 for list 1 (Node 0)

    SetListCommand(0x0106,0x0002);  // MO6 for list 1 (Node 0)

    SetListCommand(0x0107,0x0002);  // MO7 for list 1 (Node 0)

    SetListCommand(0x0108,0x0002);  // MO8 for list 1 (Node 0)

    SetListCommand(0x0109,0x0002);  // MO9 for list 1 (Node 0)

    SetListCommand(0x010A,0x0002);  // MO10 for list 1 (Node 0)

    SetListCommand(0x010B,0x0002);  // MO11 for list 1 (Node 0)

    SetListCommand(0x010C,0x0002);  // MO12 for list 1 (Node 0)

    SetListCommand(0x010D,0x0002);  // MO13 for list 1 (Node 0)

    SetListCommand(0x010E,0x0002);  // MO14 for list 1 (Node 0)

    SetListCommand(0x010F,0x0002);  // MO15 for list 1 (Node 0)

    SetListCommand(0x0110,0x0002);  // MO16 for list 1 (Node 0)

    SetListCommand(0x0111,0x0002);  // MO17 for list 1 (Node 0)

    SetListCommand(0x0112,0x0002);  // MO18 for list 1 (Node 0)

    SetListCommand(0x0113,0x0002);  // MO19 for list 1 (Node 0)

    SetListCommand(0x0114,0x0002);  // MO20 for list 1 (Node 0)

    SetListCommand(0x0115,0x0002);  // MO21 for list 1 (Node 0)

    SetListCommand(0x0116,0x0002);  // MO22 for list 1 (Node 0)

    SetListCommand(0x0117,0x0002);  // MO23 for list 1 (Node 0)

    SetListCommand(0x0118,0x0002);  // MO24 for list 1 (Node 0)

    SetListCommand(0x0119,0x0002);  // MO25 for list 1 (Node 0)

    SetListCommand(0x011A,0x0002);  // MO26 for list 1 (Node 0)

    SetListCommand(0x011B,0x0002);  // MO27 for list 1 (Node 0)

    SetListCommand(0x011C,0x0002);  // MO28 for list 1 (Node 0)

    SetListCommand(0x011D,0x0002);  // MO29 for list 1 (Node 0)

    SetListCommand(0x011E,0x0002);  // MO30 for list 1 (Node 0)

    SetListCommand(0x011F,0x0002);  // MO31 for list 1 (Node 0)

  ///  Allocate MOs for list 2:

    SetListCommand(0x0220,0x0002);  // MO32 for list 2 (Node 1)

    SetListCommand(0x0221,0x0002);  // MO33 for list 2 (Node 1)

    SetListCommand(0x0222,0x0002);  // MO34 for list 2 (Node 1)

    SetListCommand(0x0223,0x0002);  // MO35 for list 2 (Node 1)

    SetListCommand(0x0224,0x0002);  // MO36 for list 2 (Node 1)

    SetListCommand(0x0225,0x0002);  // MO37 for list 2 (Node 1)

    SetListCommand(0x0226,0x0002);  // MO38 for list 2 (Node 1)

    SetListCommand(0x0227,0x0002);  // MO39 for list 2 (Node 1)

    SetListCommand(0x0228,0x0002);  // MO40 for list 2 (Node 1)

    SetListCommand(0x0229,0x0002);  // MO41 for list 2 (Node 1)

    SetListCommand(0x022A,0x0002);  // MO42 for list 2 (Node 1)

    SetListCommand(0x022B,0x0002);  // MO43 for list 2 (Node 1)

    SetListCommand(0x022C,0x0002);  // MO44 for list 2 (Node 1)

    SetListCommand(0x022D,0x0002);  // MO45 for list 2 (Node 1)

    SetListCommand(0x022E,0x0002);  // MO46 for list 2 (Node 1)

    SetListCommand(0x022F,0x0002);  // MO47 for list 2 (Node 1)

    SetListCommand(0x0230,0x0002);  // MO48 for list 2 (Node 1)

    SetListCommand(0x0231,0x0002);  // MO49 for list 2 (Node 1)

    SetListCommand(0x0232,0x0002);  // MO50 for list 2 (Node 1)

    SetListCommand(0x0233,0x0002);  // MO51 for list 2 (Node 1)

    SetListCommand(0x0234,0x0002);  // MO52 for list 2 (Node 1)

    SetListCommand(0x0235,0x0002);  // MO53 for list 2 (Node 1)

    SetListCommand(0x0236,0x0002);  // MO54 for list 2 (Node 1)

    SetListCommand(0x0237,0x0002);  // MO55 for list 2 (Node 1)

    SetListCommand(0x0238,0x0002);  // MO56 for list 2 (Node 1)

    SetListCommand(0x0239,0x0002);  // MO57 for list 2 (Node 1)

    SetListCommand(0x023A,0x0002);  // MO58 for list 2 (Node 1)

    SetListCommand(0x023B,0x0002);  // MO59 for list 2 (Node 1)

    SetListCommand(0x023C,0x0002);  // MO60 for list 2 (Node 1)

    SetListCommand(0x023D,0x0002);  // MO61 for list 2 (Node 1)

    SetListCommand(0x023E,0x0002);  // MO62 for list 2 (Node 1)

    SetListCommand(0x023F,0x0002);  // MO63 for list 2 (Node 1)

  ///  -----------------------------------------------------------------------
  ///  Configuration of the CAN Message Objects 0 - 63:
  ///  -----------------------------------------------------------------------
	
	/* object 2 is used in the bootloader for sending. Write 1 to clear the register first */
	CAN_MOCTR2H = 0xffff;        // load MO0 control register high
	CAN_MOCTR2L = 0xffff;        // load MO0 control register low
		
//------------------------------------------------------------------------------------------------------
// 		@Function : CAN0 receive 
//------------------------------------------------------------------------------------------------------	
	for(i = 0; i < can0SvcObjectNumber; i++)
	{
		objectIndex = ptr_can0SvcInst[i].canObjIndex;
		
		if((INT32U)0 != (ptr_can0SvcInst[i].canId))
		{
			///  General Configuration of the Message Object 0 :
			///  - message object 0 is valid
			///  - message object is used as receive object
			///  - this message object is assigned to list 1 (node 0)
		/*
			CAN_MOCTR0H = 0x00A0;        // load MO0 control register high
			CAN_MOCTR0L = 0x0000;        // load MO0 control register low
		*/
			(*((uword volatile far *) (0x20101E | (objectIndex * 0x20)))) = 0x00A0;        // load MO0 control register high
			(*((uword volatile far *) (0x20101C | (objectIndex * 0x20)))) = 0x0000;        // load MO0 control register low

			///  Configuration of Message Object 0 Arbitration:
			///  - priority class 2; transmit acceptance filtering is based on the CAN 
			///    identifier
			///  - standard 11-bit identifier
			///  - identifier 11-bit:      0x000
		/*
			CAN_MOAR0H = 0x8000; 		 // load MO0 arbitration register high
			CAN_MOAR0L = 0x0000;         // load MO0 arbitration register low
		*/
			if(STANDARD_ID == ptr_can0SvcInst[i].canIdType)
			{
				(*((uword volatile far *) (0x20101A | (objectIndex * 0x20)))) = 0x8000 | (((INT16U)ptr_can0SvcInst[i].canId & 0x7FF) << 2); // load MO0 arbitration register high
				(*((uword volatile far *) (0x201018 | (objectIndex * 0x20)))) = 0x0000;         // load MO0 arbitration register low
			}
			else
			{
				(*((uword volatile far *) (0x20101A | (objectIndex * 0x20)))) = 0xA000 | (INT16U)((ptr_can0SvcInst[i].canId >> 16) & 0x1FFF); // load MO0 arbitration register high
				(*((uword volatile far *) (0x201018 | (objectIndex * 0x20)))) = (INT16U)(ptr_can0SvcInst[i].canId);         // load MO0 arbitration register low
			}


			///  Configuration of Message Object 0 acceptance mask:
			///  - only accept receive frames with matching IDE bit
			///  - acceptance mask 11-bit: 0x7FF
		/*
			CAN_MOAMR0H = 0x3FFF;        // load MO0 acceptance mask register high
			CAN_MOAMR0L = 0xFFFF;        // load MO0 acceptance mask register low
		*/
			if(15 != objectIndex)
			{
				(*((uword volatile far *) (0x20100E | (objectIndex * 0x20)))) = 0x3FFF;        // load MO0 acceptance mask register high
				(*((uword volatile far *) (0x20100C | (objectIndex * 0x20)))) = 0xFFFF;        // load MO0 acceptance mask register low
			}
			else
			{
				(*((uword volatile far *) (0x20100E | (objectIndex * 0x20)))) = 0x2000;        // load MO0 acceptance mask register high
				(*((uword volatile far *) (0x20100C | (objectIndex * 0x20)))) = 0x0000;        // load MO0 acceptance mask register low
			}

			
			///  Configuration of Message Object 0 interrupt pointer:
			///  - use message pending register 0 bit position 0
			///  - receive interrupt node pointer: MultiCAN SRN 0
		/*
			CAN_MOIPR0H = 0x0000;        // load MO0 interrupt pointer register high
			CAN_MOIPR0L = 0x0000;        // load MO0 interrupt pointer register low
		*/
			(*((uword volatile far *) (0x20100A | (objectIndex * 0x20)))) = 0x0000;        // load MO0 interrupt pointer register high
			(*((uword volatile far *) (0x201008 | (objectIndex * 0x20)))) = 0x0000 | (objectIndex << 8);        // load MO0 interrupt pointer register low

			///  Configuration of Message Object 0 FIFO/Gateway pointer:
		/*
			CAN_MOFGPR0H = 0x0000;       // load MO0 FIFO/gateway pointer register high
			CAN_MOFGPR0L = 0x0000;       // load MO0 FIFO/gateway pointer register low
		*/
			(*((uword volatile far *) (0x201006 | (objectIndex * 0x20)))) = 0x0000;       // load MO0 FIFO/gateway pointer register high
			(*((uword volatile far *) (0x201004 | (objectIndex * 0x20)))) = 0x0000;       // load MO0 FIFO/gateway pointer register low


			///  Configuration of Message Object 0 Function control:
			///  - this object is a STANDARD MESSAGE OBJECT
			///  - 8 valid data bytes
			///  - enable receive interrupt; bit RXPND is set after successful 
			///    reception of a frame
		/*
			CAN_MOFCR0H = 0x0001 | ((ptr_can0SvcInst[0].msgLen << 8) & 0x0F00); // load MO0 function control register high
			CAN_MOFCR0L = 0x0000;        // load MO0 function control register low
		*/
			(*((uword volatile far *) (0x201002 | (objectIndex * 0x20)))) = 0x0001 | ((ptr_can0SvcInst[i].msgLen << 8) & 0x0F00); // load MO0 function control register high
			(*((uword volatile far *) (0x201000 | (objectIndex * 0x20)))) = 0x0000;        // load MO0 function control register low
		}
	}	
	
	

//------------------------------------------------------------------------------------------------------
// 		@Function : CAN0 send 
//------------------------------------------------------------------------------------------------------
	for(i = 0; i < can0SendObjectNumber; i++)
	{
		objectIndex = ptr_can0SendInst[i].canObjIndex;
		
		if((INT32U)0 != (ptr_can0SendInst[i].canId))
		{
			(*((uword volatile far *) (0x20101E | (objectIndex * 0x20)))) = 0x0EA8;        // load MO16 control register high
			(*((uword volatile far *) (0x20101C | (objectIndex * 0x20)))) = 0x0000;        // load MO16 control register low
		
			if(STANDARD_ID == ptr_can0SendInst[i].canIdType)
			{
				(*((uword volatile far *) (0x20101A | (objectIndex * 0x20)))) = 0x8000 | (((INT16U)ptr_can0SendInst[i].canId & 0x7FF) << 2); // load MO16 arbitration register high
				(*((uword volatile far *) (0x201018 | (objectIndex * 0x20)))) = 0x0000;         // load MO16 arbitration register low
			}
			else
			{
				(*((uword volatile far *) (0x20101A | (objectIndex * 0x20)))) = 0xA000 | (INT16U)((ptr_can0SendInst[i].canId >> 16) & 0x1FFF); // load MO16 arbitration register high
				(*((uword volatile far *) (0x201018 | (objectIndex * 0x20)))) = (INT16U)(ptr_can0SendInst[i].canId);         // load MO16 arbitration register low
			}
			
			(*((uword volatile far *) (0x201016 | (objectIndex * 0x20)))) = 0x0000;     // load MO16 Data Bytes(DB7 & DB6)
			(*((uword volatile far *) (0x201014 | (objectIndex * 0x20)))) = 0x0000;     // load MO16 Data Bytes(DB5 & DB4)
			(*((uword volatile far *) (0x201012 | (objectIndex * 0x20)))) = 0x0000;     // load MO16 Data Bytes(DB3 & DB2)
			(*((uword volatile far *) (0x201010 | (objectIndex * 0x20)))) = 0x0000;     // load MO16 Data Bytes(DB1 & DB0)
							
			(*((uword volatile far *) (0x20100E | (objectIndex * 0x20)))) = 0x3FFF;        // load MO16 acceptance mask register high
			(*((uword volatile far *) (0x20100C | (objectIndex * 0x20)))) = 0xFFFF;        // load MO16 acceptance mask register low
			
			(*((uword volatile far *) (0x20100A | (objectIndex * 0x20)))) = 0x0000;        // load MO16 interrupt pointer register high
			(*((uword volatile far *) (0x201008 | (objectIndex * 0x20)))) = 0x0000 | (objectIndex << 8);        // load MO16 interrupt pointer register low

			(*((uword volatile far *) (0x201006 | (objectIndex * 0x20)))) = 0x0000;       // load MO16 FIFO/gateway pointer register high
			(*((uword volatile far *) (0x201004 | (objectIndex * 0x20)))) = 0x0000;       // load MO16 FIFO/gateway pointer register low
		
	#if 1
			if(16 == objectIndex)
			{
				(*((uword volatile far *) (0x201002 | (objectIndex * 0x20)))) = 0x0002 | ((ptr_can0SendInst[i].msgLen << 8) & 0x0F00); // load MO16 function control register high
				(*((uword volatile far *) (0x201000 | (objectIndex * 0x20)))) = 0x0000;        // load MO16 function control register low
			}
			else
	#endif
			{
				(*((uword volatile far *) (0x201002 | (objectIndex * 0x20)))) = 0x0000 | ((ptr_can0SendInst[i].msgLen << 8) & 0x0F00); // load MO16 function control register high
				(*((uword volatile far *) (0x201000 | (objectIndex * 0x20)))) = 0x0000;        // load MO16 function control register low
			}
		}
	}

	

//------------------------------------------------------------------------------------------------------
// 		@Function : CAN1 receive 
//------------------------------------------------------------------------------------------------------
	for(i = 0; i < can1SvcObjectNumber; i++)
	{
		objectIndex = ptr_can1SvcInst[i].canObjIndex;
		
		if((INT32U)0 != (ptr_can1SvcInst[i].canId))
		{
			(*((uword volatile far *) (0x20101E | (objectIndex * 0x20)))) = 0x00A0;        // load MO0 control register high
			(*((uword volatile far *) (0x20101C | (objectIndex * 0x20)))) = 0x0000;        // load MO0 control register low

			if(STANDARD_ID == ptr_can1SvcInst[i].canIdType)
			{
				(*((uword volatile far *) (0x20101A | (objectIndex * 0x20)))) = 0x8000 | (((INT16U)ptr_can1SvcInst[i].canId & 0x7FF) << 2); // load MO0 arbitration register high
				(*((uword volatile far *) (0x201018 | (objectIndex * 0x20)))) = 0x0000;         // load MO0 arbitration register low
			}
			else
			{
				(*((uword volatile far *) (0x20101A | (objectIndex * 0x20)))) = 0xA000 | (INT16U)((ptr_can1SvcInst[i].canId >> 16) & 0x1FFF); // load MO0 arbitration register high
				(*((uword volatile far *) (0x201018 | (objectIndex * 0x20)))) = (INT16U)(ptr_can1SvcInst[i].canId);         // load MO0 arbitration register low
			}

			if(47 != objectIndex)
			{
				(*((uword volatile far *) (0x20100E | (objectIndex * 0x20)))) = 0x3FFF;        // load MO0 acceptance mask register high
				(*((uword volatile far *) (0x20100C | (objectIndex * 0x20)))) = 0xFFFF;        // load MO0 acceptance mask register low
			}
			else
			{
				(*((uword volatile far *) (0x20100E | (objectIndex * 0x20)))) = 0x2000;        // load MO0 acceptance mask register high
				(*((uword volatile far *) (0x20100C | (objectIndex * 0x20)))) = 0x0000;        // load MO0 acceptance mask register low
			}

			(*((uword volatile far *) (0x20100A | (objectIndex * 0x20)))) = 0x0000;        // load MO0 interrupt pointer register high
			(*((uword volatile far *) (0x201008 | (objectIndex * 0x20)))) = 0x0001 | (objectIndex << 8);        // load MO0 interrupt pointer register low

			(*((uword volatile far *) (0x201006 | (objectIndex * 0x20)))) = 0x0000;       // load MO0 FIFO/gateway pointer register high
			(*((uword volatile far *) (0x201004 | (objectIndex * 0x20)))) = 0x0000;       // load MO0 FIFO/gateway pointer register low

			(*((uword volatile far *) (0x201002 | (objectIndex * 0x20)))) = 0x0001 | ((ptr_can1SvcInst[i].msgLen << 8) & 0x0F00); // load MO0 function control register high
			(*((uword volatile far *) (0x201000 | (objectIndex * 0x20)))) = 0x0000;        // load MO0 function control register low
		}
	}

	

//------------------------------------------------------------------------------------------------------
// 		@Function : CAN1 send 
//------------------------------------------------------------------------------------------------------
	for(i = 0; i < can1SendObjectNumber; i++)
	{
		objectIndex = ptr_can1SendInst[i].canObjIndex;
		
		if((INT32U)0 != (ptr_can1SendInst[i].canId))
		{
			(*((uword volatile far *) (0x20101E | (objectIndex * 0x20)))) = 0x0EA8;        // load MO16 control register high
			(*((uword volatile far *) (0x20101C | (objectIndex * 0x20)))) = 0x0000;        // load MO16 control register low

			if(STANDARD_ID == ptr_can1SendInst[i].canIdType)
			{
				(*((uword volatile far *) (0x20101A | (objectIndex * 0x20)))) = 0x8000 | (((INT16U)ptr_can1SendInst[i].canId & 0x7FF) << 2); // load MO16 arbitration register high
				(*((uword volatile far *) (0x201018 | (objectIndex * 0x20)))) = 0x0000;         // load MO16 arbitration register low
			}
			else
			{
				(*((uword volatile far *) (0x20101A | (objectIndex * 0x20)))) = 0xA000 | (INT16U)((ptr_can1SendInst[i].canId >> 16) & 0x1FFF); // load MO16 arbitration register high
				(*((uword volatile far *) (0x201018 | (objectIndex * 0x20)))) = (INT16U)(ptr_can1SendInst[i].canId);         // load MO16 arbitration register low
			}

			(*((uword volatile far *) (0x201016 | (objectIndex * 0x20)))) = 0x0000;     // load MO16 Data Bytes(DB7 & DB6)
			(*((uword volatile far *) (0x201014 | (objectIndex * 0x20)))) = 0x0000;     // load MO16 Data Bytes(DB5 & DB4)
			(*((uword volatile far *) (0x201012 | (objectIndex * 0x20)))) = 0x0000;     // load MO16 Data Bytes(DB3 & DB2)
			(*((uword volatile far *) (0x201010 | (objectIndex * 0x20)))) = 0x0000;     // load MO16 Data Bytes(DB1 & DB0)	 

			(*((uword volatile far *) (0x20100E | (objectIndex * 0x20)))) = 0x3FFF;        // load MO16 acceptance mask register high
			(*((uword volatile far *) (0x20100C | (objectIndex * 0x20)))) = 0xFFFF;        // load MO16 acceptance mask register low
			
			if(48 == objectIndex)
			{
				(*((uword volatile far *) (0x20100A | (objectIndex * 0x20)))) = 0x0000;        // load MO16 interrupt pointer register high
				(*((uword volatile far *) (0x201008 | (objectIndex * 0x20)))) = 0x0010 | (objectIndex << 8);        // load MO16 interrupt pointer register low
			}
			else
			{
				(*((uword volatile far *) (0x20100A | (objectIndex * 0x20)))) = 0x0000;        // load MO16 interrupt pointer register high
				(*((uword volatile far *) (0x201008 | (objectIndex * 0x20)))) = 0x0000 | (objectIndex << 8);        // load MO16 interrupt pointer register low
			}

			(*((uword volatile far *) (0x201006 | (objectIndex * 0x20)))) = 0x0000;       // load MO16 FIFO/gateway pointer register high
			(*((uword volatile far *) (0x201004 | (objectIndex * 0x20)))) = 0x0000;       // load MO16 FIFO/gateway pointer register low

	#if 1
			if(48 == objectIndex)
			{
				(*((uword volatile far *) (0x201002 | (objectIndex * 0x20)))) = 0x0002 | ((ptr_can1SendInst[i].msgLen << 8) & 0x0F00); // load MO16 function control register high
				(*((uword volatile far *) (0x201000 | (objectIndex * 0x20)))) = 0x0000;        // load MO16 function control register low
			}
			else
	#endif
			{
				(*((uword volatile far *) (0x201002 | (objectIndex * 0x20)))) = 0x0000 | ((ptr_can1SendInst[i].msgLen << 8) & 0x0F00); // load MO16 function control register high
				(*((uword volatile far *) (0x201000 | (objectIndex * 0x20)))) = 0x0000;        // load MO16 function control register low
			}
		}	
	}



  ///  -----------------------------------------------------------------------
  ///  Configuration of Service Request Nodes 0 - 15:
  ///  -----------------------------------------------------------------------
  ///  SRN0 service request node configuration:
  ///  - SRN0 interrupt priority level (ILVL) = 4
  ///  - SRN0 interrupt group level (GLVL) = 0
  ///  - SRN0 group priority extension (GPX) = 0

  CAN_0IC        =  0x0050;     

  ///  SRN1 service request node configuration:
  ///  - SRN1 interrupt priority level (ILVL) = 4
  ///  - SRN1 interrupt group level (GLVL) = 1
  ///  - SRN1 group priority extension (GPX) = 0

  CAN_1IC        =  0x0051;     

  ///  SRN2 service request node configuration:
  ///  - SRN2 interrupt priority level (ILVL) = 3
  ///  - SRN2 interrupt group level (GLVL) = 2
  ///  - SRN2 group priority extension (GPX) = 0

  CAN_2IC        =  0x004E;     

  ///  SRN3 service request node configuration:
  ///  - SRN3 interrupt priority level (ILVL) = 3
  ///  - SRN3 interrupt group level (GLVL) = 1
  ///  - SRN3 group priority extension (GPX) = 0

  CAN_3IC        =  0x004D;     


  //   -----------------------------------------------------------------------
  //   Start the CAN Nodes:
  //   -----------------------------------------------------------------------

  ///  - ------------- CAN_NCR0 ----------------------------------------------

    CAN_NCR0 &= ~ (uword) 0x0041; // reset INIT and CCE
  ///  - ------------- CAN_NCR1 ----------------------------------------------

    CAN_NCR1 &= ~ (uword) 0x0041; // reset INIT and CCE



  // USER CODE BEGIN (Init,3)

  // USER CODE END

} //  End of function CAN_vInit

//****************************************************************************
// @Function      void CAN_vTransmit(ubyte ubObjNr) 
//
//----------------------------------------------------------------------------
// @Description   This function triggers the CAN controller to send the 
//                selected message.
//                If the selected message object is a TRANSMIT OBJECT then 
//                this function triggers the sending of a data frame. If 
//                however the selected message object is a RECEIVE OBJECT 
//                this function triggers the sending of a remote frame.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    ubObjNr: 
//                Number of the message object (0-63)
//
//----------------------------------------------------------------------------
// @Date          2020/5/11
//
//****************************************************************************

// USER CODE BEGIN (Transmit,1)

// USER CODE END

void CAN_vTransmit(ubyte ubObjNr)
{

  CAN_HWOBJ[ubObjNr].uwMOCTRH = 0x0720;  // set TXRQ,TXEN0,TXEN1,MSGVAL

} //  End of function CAN_vTransmit

//****************************************************************************
// @Function      void CAN_vLoadData(ubyte ubObjNr, ubyte *ubpubData) 
//
//----------------------------------------------------------------------------
// @Description   If a hardware TRANSMIT OBJECT has to be loaded with data 
//                but not with a new identifier, this function may be used 
//                instead of the function CAN_vConfigMsgObj. The message 
//                object should be accessed by calling the function 
//                CAN_ubRequestMsgObj before calling this function. This 
//                prevents the CAN controller from working with invalid data.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    ubObjNr: 
//                Number of the message object to be configured (0-63)
// @Parameters    *ubpubData: 
//                Pointer on a data buffer
//
//----------------------------------------------------------------------------
// @Date          2020/5/11
//
//****************************************************************************

// USER CODE BEGIN (LoadData,1)

// USER CODE END

void CAN_vLoadData(ubyte ubObjNr, ubyte *ubpubData)
{
  ubyte i;

  CAN_HWOBJ[ubObjNr].uwMOCTRH = 0x0008;       // set NEWDAT

  for(i = 0; i < (CAN_HWOBJ[ubObjNr].uwMOFCRH & 0x0F00) >> 8; i++)
  {
    CAN_HWOBJ[ubObjNr].ubData[i] = *(ubpubData++); //load MODATA
  }

  CAN_HWOBJ[ubObjNr].uwMOCTRH = 0x0020;       // set MSGVAL
  CAN_HWOBJ[ubObjNr].uwMOCTRL = 0x0040;       // reset RTSEL

} //  End of function CAN_vLoadData

//****************************************************************************
// @Function      void Can_Init(void) 
//
//----------------------------------------------------------------------------
// @Description   This is the initialization function of the CAN_LLD function 
//                library. It is assumed that the SFRs used by this library 
//                are in reset state. 
//                The Function CAN_vInit() is called to Initialize as per 
//                DAvE Configuration.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          2020/5/11
//
//****************************************************************************

// USER CODE BEGIN (LLDInit,1)

// USER CODE END

void Can_Init(void)
{

// USER CODE BEGIN (LLDInit,2)

// USER CODE END

  CAN_vInit(); // Call initialization function

// USER CODE BEGIN (LLDInit,3)

// USER CODE END

} //  End of function Can_Init

//****************************************************************************
// @Function      ubyte Can_SetControllerMode(ubyte Controller, 
//                Can_StateTransitionType Transition) 
//
//----------------------------------------------------------------------------
// @Description   This function performs software triggered state transition 
//                of the CAN Contrller state machine.
//
//----------------------------------------------------------------------------
// @Returnvalue   1 if transition success, else 0
//
//----------------------------------------------------------------------------
// @Parameters    Controller: 
//                CAN Controller for which the state shall be changed 
//                Transition. Number of node ( 0 - 3)
// @Parameters    Transition: 
//                Transition - Transition requested.The following definitions 
//                for TimerNr are available: CAN_T_START, CAN_T_STOP, 
//                CAN_T_SLEEP, CAN_T_WAKEUP
//
//----------------------------------------------------------------------------
// @Date          2020/5/11
//
//****************************************************************************

// USER CODE BEGIN (SetControllerMode,1)

// USER CODE END

ubyte Can_SetControllerMode(ubyte Controller, Can_StateTransitionType Transition)
{
ubyte retValue = 0;

      switch (Transition)
      {
        case CAN_T_START:

          /* Allow participation of the node/controller in the CAN traffic */
            CAN_HWNodes[Controller].uwNCR = (CAN_HWNodes[Controller].uwNCR & ~CAN_NCR_INIT);

          // USER CODE BEGIN (SetControllerMode,2)

          // USER CODE END
          retValue = 1 ;

          break ;

        case CAN_T_STOP:

           /* Terminate participation of the node/controller in the CAN traffic */
            CAN_HWNodes[Controller].uwNCR = (CAN_HWNodes[Controller].uwNCR | CAN_NCR_INIT);

          // USER CODE BEGIN (SetControllerMode,3)

          // USER CODE END

          retValue = 1 ;

          break ;

        case CAN_T_SLEEP:

          // USER CODE BEGIN (SetControllerMode,4)

          // USER CODE END

          break ;

        case CAN_T_WAKEUP:

          // USER CODE BEGIN (SetControllerMode,5)

          // USER CODE END
          break ;

        default:

          // USER CODE BEGIN (SetControllerMode,6)

          // USER CODE END
            break;

      }

    // USER CODE BEGIN (SetControllerMode,7)

    // USER CODE END

    return retValue ;

} //  End of function Can_SetControllerMode

//****************************************************************************
// @Function      void Can_lInitializeController(const ubyte Controller, 
//                const uword Config) 
//
//----------------------------------------------------------------------------
// @Description   This function (re-)initialization of CAN Controller.
//                NOTE: The generated configurations are as per DAvE Node 
//                Configuration
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    Controller: 
//                CAN Controller to be initialized. Number of node ( 0 - 3)
// @Parameters    Config: 
//                Config - controller configuration.
//
//----------------------------------------------------------------------------
// @Date          2020/5/11
//
//****************************************************************************

// USER CODE BEGIN (lInitializeController,1)

// USER CODE END

void Can_lInitializeController(const ubyte Controller, const uword Config)
{

// USER CODE BEGIN (lInitializeController,2)

// USER CODE END

    CAN_HWNodes[Controller].uwNCR = (CAN_HWNodes[Controller].uwNCR | ( CAN_NCR_INIT | CAN_NCR_CCE ));

      /* Set Node Bit Timing values from configuration.
         Note: Masking is done to exclude loop back enable setting. */

    CAN_HWNodes[Controller].uwNBTRL = Config; // load NBTR0_DIV8, TSEG2, TSEG1, SJW and BRP

  switch (Controller)
  {
    case CAN_CONTROLLER0:


      CAN_HWNodes[Controller].uwNECNTH = 0x0060; // load NECNT0_EWRNLVL 
                                                 // register
      CAN_HWNodes[Controller].uwNECNTL = 0x0000;

      /* Disable Configuration Change - NBTR, NPCR, NECNT can only be read */
      CAN_HWNodes[Controller].uwNCR = ( CAN_HWNodes[Controller].uwNCR & ~CAN_NCR_CCE) ;

      // USER CODE BEGIN (lInitializeController,   3 )

      // USER CODE END

    break ;

    case CAN_CONTROLLER1:


      CAN_HWNodes[Controller].uwNECNTH = 0x0060; // load NECNT1_EWRNLVL 
                                                 // register
      CAN_HWNodes[Controller].uwNECNTL = 0x0000;

      /* Disable Configuration Change - NBTR, NPCR, NECNT can only be read */
      CAN_HWNodes[Controller].uwNCR = ( CAN_HWNodes[Controller].uwNCR & ~CAN_NCR_CCE) ;

      // USER CODE BEGIN (lInitializeController,   4 )

      // USER CODE END

    break ;

    default:

      // USER CODE BEGIN (lInitializeController,8)

      // USER CODE END

    break;
  }

  // USER CODE BEGIN (lInitializeController,9)

  // USER CODE END

} //  End of function Can_lInitializeController

//****************************************************************************
// @Function      void Can_EnableControllerInterrupts(ubyte Controller) 
//
//----------------------------------------------------------------------------
// @Description   This function enables CAN Node interrupts which are 
//                configured in DAvE.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    Controller: 
//                CAN Controller for which interrupts shall be enabled. 
//                Number of node ( 0 - 3)
//
//----------------------------------------------------------------------------
// @Date          2020/5/11
//
//****************************************************************************

// USER CODE BEGIN (EnableControllerInterrupts,1)

// USER CODE END

void Can_EnableControllerInterrupts(ubyte Controller)
{

  // USER CODE BEGIN (EnableControllerInterrupts,2)

  // USER CODE END

  switch (Controller)
  {
    case CAN_CONTROLLER0:

      // - load NODE 0 control register

      CAN_NCR0   =  0x0008;      // load NODE 0 control register

      // USER CODE BEGIN (EnableControllerInterrupts,   3 )

      // USER CODE END

    break ;

    case CAN_CONTROLLER1:

      // - load NODE 1 control register

      CAN_NCR1   =  0x0008;      // load NODE 1 control register

      // USER CODE BEGIN (EnableControllerInterrupts,   4 )

      // USER CODE END

    break ;

    default:

      // USER CODE BEGIN (EnableControllerInterrupts,8)

      // USER CODE END

    break;
  }

  // USER CODE BEGIN (EnableControllerInterrupts,9)

  // USER CODE END

} //  End of function Can_EnableControllerInterrupts

//****************************************************************************
// @Function      void Can_DisableControllerInterrupts(ubyte Controller) 
//
//----------------------------------------------------------------------------
// @Description   This function disables CAN Node interrupts which are 
//                configured in DAvE.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    Controller: 
//                CAN Controller for which interrupts shall be disabled. 
//                Number of node ( 0 - 3)
//
//----------------------------------------------------------------------------
// @Date          2020/5/11
//
//****************************************************************************

// USER CODE BEGIN (DisableControllerInterrupts,1)

// USER CODE END

void Can_DisableControllerInterrupts(ubyte Controller)
{

  // USER CODE BEGIN (DisableControllerInterrupts,2)

  // USER CODE END

  switch (Controller)
  {
    case CAN_CONTROLLER0:

      // - load NODE 0 control register

      CAN_NCR0  &= ~  0x0008;    // load NODE 0 control register

      // USER CODE BEGIN (DisableControllerInterrupts,   3 )

      // USER CODE END

    break ;

    case CAN_CONTROLLER1:

      // - load NODE 1 control register

      CAN_NCR1  &= ~  0x0008;    // load NODE 1 control register

      // USER CODE BEGIN (DisableControllerInterrupts,   4 )

      // USER CODE END

    break ;

    default:

      // USER CODE BEGIN (DisableControllerInterrupts,8)

      // USER CODE END

    break;
  }

  // USER CODE BEGIN (DisableControllerInterrupts,9)

  // USER CODE END

} //  End of function Can_DisableControllerInterrupts



//****************************************************************************
// @Macro         CAN_viSRN0() 
//
//----------------------------------------------------------------------------
// @Description   
//
//----------------------------------------------------------------------------
// @Returnvalue   
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          2020/5/11
//
//****************************************************************************

// USER CODE BEGIN (SRN0,1)

// USER CODE END

void CAN_viSRN0(void) interrupt CAN_SRN0INT
{

  uword uwSRN0ObjHandler;
  ubyte ubMessagePendingIndex0 = 0u;

  // USER CODE BEGIN (SRN0,2)

  // USER CODE END

  CAN_MSIMASKH = 0x0001;          // set message index mask register
  CAN_MSIMASKL = 0xFFFF;

  while (CAN_MSID0 != 0x0020)
  {
	  ubMessagePendingIndex0 = (ubyte)CAN_MSID0;
	  
	  if(ubMessagePendingIndex0 <= 15)
	  {
		  uwSRN0ObjHandler = CAN_HWOBJ[ubMessagePendingIndex0].uwMOCTRL;

		  if(uwSRN0ObjHandler & MOSTAT_RXPND)          // if message object 0 receive interrupt
		  {
				// USER CODE BEGIN (SRN0_OBJ0,1)

				// USER CODE END

			if(uwSRN0ObjHandler & MOSTAT_NEWDAT)       // if NEWDAT is set
			{
				// USER CODE BEGIN (SRN0_OBJ0,2)

				// USER CODE END

			  if (uwSRN0ObjHandler & MOSTAT_MSGLST)    // if MSGLST is set
			  {
				// Indicates that the CAN controller has stored a new 
				// message into this object, while NEWDAT was still set,
				// ie. the previously stored message is lost.

				// USER CODE BEGIN (SRN0_OBJ0,3)

				// USER CODE END

			  }
			  else
			  {
				// The CAN controller has stored a new message into this object.

				  //----- get Identifier ------------------------------------------------
				  if(CAN_HWOBJ[ubMessagePendingIndex0].uwMOARH & 0x2000)    // if extended identifier
				  {
					//----- get Extended Identifier -------------------------------------
					  MessageId_0 = (((ulong)(CAN_HWOBJ[ubMessagePendingIndex0].uwMOARH & 0x1FFF))<<16) + CAN_HWOBJ[ubMessagePendingIndex0].uwMOARL;
				  }
				  else                                       // standard identifier
				  {
					//----- get Standard Identifier -------------------------------------
					  MessageId_0 = (ulong)(CAN_HWOBJ[ubMessagePendingIndex0].uwMOARH & 0x1FFF) >> 2;
				  }

				  DataPointer_0[0] = CAN_HWOBJ[ubMessagePendingIndex0].ubData[0];
				  DataPointer_0[1] = CAN_HWOBJ[ubMessagePendingIndex0].ubData[1];
				  DataPointer_0[2] = CAN_HWOBJ[ubMessagePendingIndex0].ubData[2];
				  DataPointer_0[3] = CAN_HWOBJ[ubMessagePendingIndex0].ubData[3];
				  DataPointer_0[4] = CAN_HWOBJ[ubMessagePendingIndex0].ubData[4];
				  DataPointer_0[5] = CAN_HWOBJ[ubMessagePendingIndex0].ubData[5];
				  DataPointer_0[6] = CAN_HWOBJ[ubMessagePendingIndex0].ubData[6];
				  DataPointer_0[7] = CAN_HWOBJ[ubMessagePendingIndex0].ubData[7];

			  CanIf_RxIndication0(ubMessagePendingIndex0, MessageId_0, ((CAN_HWOBJ[ubMessagePendingIndex0].uwMOFCRH & 0x0F00)>>8), DataPointer_0);


				// USER CODE BEGIN (SRN0_OBJ0,4)

				// USER CODE END
			  }

			}

			CAN_HWOBJ[ubMessagePendingIndex0].uwMOCTRL = MOSTAT_RST_MNR;            // reset MSGLST + NEWDAT + RXPND

		  }  // End of RXPND0


		  CAN_MSPND0H = ~(uword) 0x0000; // reset PND bit
		  CAN_MSPND0L = ~(uword) (0x0001 << ubMessagePendingIndex0);

		  
	  }
	  else if(16 == ubMessagePendingIndex0)
	  {
		  uwSRN0ObjHandler = CAN_HWOBJ[16].uwMOCTRL;

		  if(uwSRN0ObjHandler & MOSTAT_TXPND)                      // if message object 16 transmit interrupt
		  {

			// The transmission of the last message object
			// was successful.

			  CanIf_TxConfirmation(16);


				// USER CODE BEGIN (SRN0_OBJ16,6)

				// USER CODE END


			CAN_HWOBJ[16].uwMOCTRL = MOSTAT_RST_NT;               // reset NEWDAT + TXPND

		  }  // End of TXPND16

		  CAN_MSPND0H = ~(uword) 0x0001; // reset PND bit
		  CAN_MSPND0L = ~(uword) 0x0000;
	  }
	  


  }  // end of while CAN_MSID


} //  End of function CAN_viSRN0


//****************************************************************************
// @Macro         CAN_viSRN1() 
//
//----------------------------------------------------------------------------
// @Description   
//
//----------------------------------------------------------------------------
// @Returnvalue   
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          2020/5/11
//
//****************************************************************************

// USER CODE BEGIN (SRN1,1)

// USER CODE END

void CAN_viSRN1(void) interrupt CAN_SRN1INT
{

  uword uwSRN1ObjHandler;
  ubyte ubMessagePendingIndex1 = 0u;

  // USER CODE BEGIN (SRN1,2)

  // USER CODE END

  CAN_MSIMASKH = 0x0001;          // set message index mask register
  CAN_MSIMASKL = 0xFFFF;

  while (CAN_MSID1 != 0x0020)
  {
	  ubMessagePendingIndex1 = (ubyte)CAN_MSID1;
	  
	  if(ubMessagePendingIndex1 <= 15)
	  {
		  uwSRN1ObjHandler = CAN_HWOBJ[ubMessagePendingIndex1 + 32].uwMOCTRL;

		  if(uwSRN1ObjHandler & MOSTAT_RXPND)          // if message object 32 receive interrupt
		  {
				// USER CODE BEGIN (SRN1_OBJ32,1)

				// USER CODE END

			if(uwSRN1ObjHandler & MOSTAT_NEWDAT)       // if NEWDAT is set
			{
				// USER CODE BEGIN (SRN1_OBJ32,2)

				// USER CODE END

			  if (uwSRN1ObjHandler & MOSTAT_MSGLST)    // if MSGLST is set
			  {
				// Indicates that the CAN controller has stored a new 
				// message into this object, while NEWDAT was still set,
				// ie. the previously stored message is lost.

				// USER CODE BEGIN (SRN1_OBJ32,3)

				// USER CODE END

			  }
			  else
			  {
				// The CAN controller has stored a new message into this object.

				  //----- get Identifier ------------------------------------------------
				  if(CAN_HWOBJ[ubMessagePendingIndex1 + 32].uwMOARH & 0x2000)    // if extended identifier
				  {
					//----- get Extended Identifier -------------------------------------
					  MessageId_32 = (((ulong)(CAN_HWOBJ[ubMessagePendingIndex1 + 32].uwMOARH & 0x1FFF))<<16) + CAN_HWOBJ[ubMessagePendingIndex1 + 32].uwMOARL;
				  }
				  else                                       // standard identifier
				  {
					//----- get Standard Identifier -------------------------------------
					  MessageId_32 = (ulong)(CAN_HWOBJ[ubMessagePendingIndex1 + 32].uwMOARH & 0x1FFF) >> 2;
				  }

				  DataPointer_32[0] = CAN_HWOBJ[ubMessagePendingIndex1 + 32].ubData[0];
				  DataPointer_32[1] = CAN_HWOBJ[ubMessagePendingIndex1 + 32].ubData[1];
				  DataPointer_32[2] = CAN_HWOBJ[ubMessagePendingIndex1 + 32].ubData[2];
				  DataPointer_32[3] = CAN_HWOBJ[ubMessagePendingIndex1 + 32].ubData[3];
				  DataPointer_32[4] = CAN_HWOBJ[ubMessagePendingIndex1 + 32].ubData[4];
				  DataPointer_32[5] = CAN_HWOBJ[ubMessagePendingIndex1 + 32].ubData[5];
				  DataPointer_32[6] = CAN_HWOBJ[ubMessagePendingIndex1 + 32].ubData[6];
				  DataPointer_32[7] = CAN_HWOBJ[ubMessagePendingIndex1 + 32].ubData[7];

			  CanIf_RxIndication1(ubMessagePendingIndex1 + 32, MessageId_32, ((CAN_HWOBJ[ubMessagePendingIndex1 + 32].uwMOFCRH & 0x0F00)>>8), DataPointer_32);


				// USER CODE BEGIN (SRN1_OBJ32,4)

				// USER CODE END
			  }

			}

			CAN_HWOBJ[ubMessagePendingIndex1 + 32].uwMOCTRL = MOSTAT_RST_MNR;            // reset MSGLST + NEWDAT + RXPND

		  }  // End of RXPND32


		  CAN_MSPND1H = ~(uword) 0x0000; // reset PND bit
		  CAN_MSPND1L = ~(uword) (0x0001 << ubMessagePendingIndex1);
	  }
	  else if(16 == ubMessagePendingIndex1)
	  {
		  uwSRN1ObjHandler = CAN_HWOBJ[48].uwMOCTRL;

		  if(uwSRN1ObjHandler & MOSTAT_TXPND)                      // if message object 48 transmit interrupt
		  {

			// The transmission of the last message object
			// was successful.

			  CanIf_TxConfirmation(48);


				// USER CODE BEGIN (SRN1_OBJ48,6)

				// USER CODE END


			CAN_HWOBJ[48].uwMOCTRL = MOSTAT_RST_NT;               // reset NEWDAT + TXPND

		  }  // End of TXPND48

		  CAN_MSPND1H = ~(uword) 0x0001; // reset PND bit
		  CAN_MSPND1L = ~(uword) 0x0000;
	  }
  }  // end of while CAN_MSID

} //  End of function CAN_viSRN1


//****************************************************************************
// @Macro         CAN_viSRN2() 
//
//----------------------------------------------------------------------------
// @Description   
//
//----------------------------------------------------------------------------
// @Returnvalue   
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          2020/5/15
//
//****************************************************************************

// USER CODE BEGIN (SRN2,1)

// USER CODE END

void CAN_viSRN2(void) interrupt CAN_SRN2INT
{

  uword uwSRN2NodeHandler;

  // USER CODE BEGIN (SRN2,2)

  // USER CODE END

  //   -----------------------------------------------------------------------
  //   Interrupts of CAN node 0
  //   -----------------------------------------------------------------------

  uwSRN2NodeHandler = CAN_NSR0;

  if (uwSRN2NodeHandler & 0x0020)               // if ALERT
  {
      if (uwSRN2NodeHandler & 0x0080)           // if BOFF
      {
        // Indicates when the CAN controller is in busoff state.

          CanIf_ControllerBusOff(0);

        // USER CODE BEGIN (SRN2_NODE0,1)

        // USER CODE END
      }

      if (uwSRN2NodeHandler & 0x0040)           // if EWRN
      {
        // One of the error counters REC or TEC reached the
        // warning limit of 96.

        // USER CODE BEGIN (SRN2_NODE0,2)

        // USER CODE END
      }

      if (uwSRN2NodeHandler & 0x0100)           // if LLE
      {
        // A list length error has been detected during message
        // acceptance filtering. The number of elements in the list
        // that belongs to this CAN node differs from the list SIZE 
        // given in the list termination pointer

        CAN_NSR0      = 0x023F;        // reset LLE

        // USER CODE BEGIN (SRN2_NODE0,3)

        // USER CODE END
      }

      if (uwSRN2NodeHandler & 0x0200)           // if LOE
      {
        // A list object error has been detected during message
        // acceptance filtering. A message object with wrong LIST 
        // index entry in the Message Object Control Register has 
        // been detected

        CAN_NSR0      = 0x013F;        // reset LOE

        // USER CODE BEGIN (SRN2_NODE0,4)

        // USER CODE END
      }

      if (CAN_NCR0 & 0x0001)           // if INIT
      {
        // Bit INIT has been set by the MultiCAN

        // USER CODE BEGIN (SRN2_NODE0,5)

        // USER CODE END
      }

      CAN_NSR0      = 0x031F;          // reset ALERT

      // USER CODE BEGIN (SRN2_NODE0,6)

      // USER CODE END
  }



  // USER CODE BEGIN (SRN2,15)

  // USER CODE END

} //  End of function CAN_viSRN2


//****************************************************************************
// @Macro         CAN_viSRN3() 
//
//----------------------------------------------------------------------------
// @Description   
//
//----------------------------------------------------------------------------
// @Returnvalue   
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//----------------------------------------------------------------------------
// @Date          2020/5/15
//
//****************************************************************************

// USER CODE BEGIN (SRN3,1)

// USER CODE END

void CAN_viSRN3(void) interrupt CAN_SRN3INT
{

  uword uwSRN3NodeHandler;

  // USER CODE BEGIN (SRN3,2)

  // USER CODE END

  //   -----------------------------------------------------------------------
  //   Interrupts of CAN node 1
  //   -----------------------------------------------------------------------

  uwSRN3NodeHandler = CAN_NSR1;

  if (uwSRN3NodeHandler & 0x0020)               // if ALERT
  {
      if (uwSRN3NodeHandler & 0x0080)           // if BOFF
      {
        // Indicates when the CAN controller is in busoff state.

          CanIf_ControllerBusOff(1);

        // USER CODE BEGIN (SRN3_NODE1,1)

        // USER CODE END
      }

      if (uwSRN3NodeHandler & 0x0040)           // if EWRN
      {
        // One of the error counters REC or TEC reached the
        // warning limit of 96.

        // USER CODE BEGIN (SRN3_NODE1,2)

        // USER CODE END
      }

      if (uwSRN3NodeHandler & 0x0100)           // if LLE
      {
        // A list length error has been detected during message
        // acceptance filtering. The number of elements in the list
        // that belongs to this CAN node differs from the list SIZE 
        // given in the list termination pointer

        CAN_NSR1      = 0x023F;        // reset LLE

        // USER CODE BEGIN (SRN3_NODE1,3)

        // USER CODE END
      }

      if (uwSRN3NodeHandler & 0x0200)           // if LOE
      {
        // A list object error has been detected during message
        // acceptance filtering. A message object with wrong LIST 
        // index entry in the Message Object Control Register has 
        // been detected

        CAN_NSR1      = 0x013F;        // reset LOE

        // USER CODE BEGIN (SRN3_NODE1,4)

        // USER CODE END
      }

      if (CAN_NCR1 & 0x0001)           // if INIT
      {
        // Bit INIT has been set by the MultiCAN

        // USER CODE BEGIN (SRN3_NODE1,5)

        // USER CODE END
      }

      CAN_NSR1      = 0x031F;          // reset ALERT

      // USER CODE BEGIN (SRN3_NODE1,6)

      // USER CODE END
  }



  // USER CODE BEGIN (SRN3,15)

  // USER CODE END

} //  End of function CAN_viSRN3



// USER CODE BEGIN (CAN_General,10)

// USER CODE END

