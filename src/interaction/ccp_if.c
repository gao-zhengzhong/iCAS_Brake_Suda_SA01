/*
 * ccp_interface.c
 *
 *  Created on: 2015-5-21
 *      Author: zhangguangyao
 */

// -----------------------------------------------------------------------------
// INCLUDE FILES
// -----------------------------------------------------------------------------
#include "MAIN.h"
#include "ccp_if.h"   // additional functions for CCP usage
#include "ccp.h"

#include "can_if.h"

// -----------------------------------------------------------------------------
//static ubyte RandomSeed[4] = {0};	//第一个字节是标志位
BYTE receive_buffer[8]; // receive buffer

// -----------------------------------------------------------------------------
// SENDING an CRM-DTO when receiving an CRO
// -----------------------------------------------------------------------------
BYTE ccpSend(BYTEPTR msg) // #define BYTEPTR unsigned char*
{
	// callup CAN-send调用CAN发送程序in“boot_can.c”
	
	CanSendMsg_v_g(25, msg);
	return 1;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CONVERT pointer
// -----------------------------------------------------------------------------
MTABYTEPTR ccpGetPointer(BYTE addr_ext, DWORD addr) // get Pointer into
{													// normal C
	addr = BSWAP_32(addr);
	return (MTABYTEPTR)addr;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CALCULATE new measuerments
// -----------------------------------------------------------------------------
void ccpUserBackground(void)
{
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Define
// -----------------------------------------------------------------------------
BYTE ccpDisableNormalOperation(MTABYTEPTR a, WORD s)
{
	disable_interrupt(); // disables all interrupts, e.g. TIMER_0
	return 1;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Disable/Enable Interrupt Functions
// -----------------------------------------------------------------------------
BYTE disable_interrupt_count = 0; // init counter

void disable_interrupt()
{
	CCP_ENABLE_INTERRUPT;
	disable_interrupt_count++; // incr. counter
}

void enable_interrupt()
{
	if (--disable_interrupt_count == 0)
	{ // if interrupts only one time disabled
		CCP_ENABLE_INTERRUPT;
		//_asm();// enable them
	}
}

// -----------------------------------------------------------------------------
void ccploop_v_g(INT8U* buffer)
{

	//OS_CPU_SR   cpu_sr;
    INT8U length;
	ccpBackground();                      // calculate checksum。计算校验和

	ccpCommand(buffer);         // call ccpCommand。调用命令处理程序
	
	CCP_DISABLE_INTERRUPT;

	ccpSendCallBack();

	CCP_ENABLE_INTERRUPT;

	ccpDaq(1);                            // transmit new values
}


/*!******************************************************************************************************
*	@fn	void   ccpGetSeed( CCP_BYTE resourceMask )
*	@brief	CCP获取Seed函数
*	@author Matt Zhang
*	@param[in]	resourceMask
*	@return	Seed
*	@note
*		- 2012-7-28, Matt Zhang, created acct RD-SW-120001-24 SeedKey交互文档
*********************************************************************************************************
*/
CCP_DWORD ccpGetSeed(CCP_BYTE resourceMask)
{

	return 0;
}
/*!******************************************************************************************************
*	@fn	void    ccpUnlock( CCP_BYTE *key )
*	@brief	CCP获取Key函数
*	@author Matt Zhang
*	@param[in]	key
*	@return
*	@note
*		- 2012-7-28, Matt Zhang, created acct RD-SW-120001-24 SeedKey交互文档
*********************************************************************************************************
*/
CCP_BYTE ccpUnlock(CCP_BYTE *key)
{

	return 0;
}
