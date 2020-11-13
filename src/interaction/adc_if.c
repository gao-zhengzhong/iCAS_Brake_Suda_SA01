/***********************************************************************
*   Copyright (C) JWD Automotive Co., Ltd.				               * 
*	All Rights Reserved.            					               *
*   Department : iCAS SW      									       *
*   AUTHOR	   :            										   *
************************************************************************
* Object        : 
* Module        : adc_if.c
* Instance      : 
* Description   : ADC sample interface
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

#include "XC23xxBREGS.h"

#include "adc_if.h"
#include "rte.h"
#include "ADC0.h"
#include "ADC1.h"

#define ADC_IF_GLOBALS


/**** Definition of variables ****/
ADC_IF_STAT T_ADC_IF tAdcIf = {0u};


/**** Declaration of functions ****/
ADC_IF_STAT void AdcIf_VehiclePowerVoltage(void);
ADC_IF_STAT void AdcIf_BrakeMotorVlinkSensorVoltage(void);
ADC_IF_STAT void AdcIf_BrakeMotorAPhaseSensorVoltage(void);
ADC_IF_STAT void AdcIf_BrakeMotorBPhaseSnesorVoltage(void);
ADC_IF_STAT void AdcIf_BrakeMainCylinderSensorVoltage(void);
ADC_IF_STAT void AdcIf_BrakePedalLocationSensorVoltage(void);
ADC_IF_STAT void AdcIf_BrakeMainCylinderPressureSensorVoltage(void);
ADC_IF_STAT void AdcIf_BrakePedalLocationSensor1(void);
ADC_IF_STAT void AdcIf_BrakePedalLocationSensor2(void);
ADC_IF_STAT void AdcIf_BrakeMainCylinderLocationSensor1(void);
ADC_IF_STAT void AdcIf_BrakeMainCylinderLocationSensor2(void);
ADC_IF_STAT void AdcIf_BrakeMotorCurrentSensor(void);
ADC_IF_STAT void AdcIf_EnvironmentTemperatureSensor(void);
ADC_IF_STAT void AdcIf_BrakeMainCylinderPressureSensor(void);
ADC_IF_STAT void AdcIf_ReserveSensor1(void);
ADC_IF_STAT void AdcIf_ReserveSensor2(void);
ADC_IF_STAT INT16U AdcIf_SignalSample(INT8U adcCore, INT8U adcChannel);


/***********************************************************************
*  Name        : AdcIf_Init
*  Description : After power on, initiliaze ADC sample variable
*  Parameter   : None
*  Returns     : None
***********************************************************************/
void AdcIf_Init(void)
{
	memset(&tAdcIf, 0, sizeof(tAdcIf));
	return;
}

/***********************************************************************
*  Name        : AdcIf_DeInit
*  Description : During running, need initilize ADC sample variable
*  Parameter   : None
*  Returns     : None
***********************************************************************/
void AdcIf_DeInit(void)
{
	return;
}

/***********************************************************************
*  Name        : AdcIf_Func
*  Description : ADC sample handle
*  Parameter   : None
*  Returns     : None
***********************************************************************/
void AdcIf_Func(void)
{
	if(FALSE == g_adcSimulateDataFlag)
	{
		AdcIf_VehiclePowerVoltage();
		AdcIf_BrakeMotorVlinkSensorVoltage();
		AdcIf_BrakeMotorAPhaseSensorVoltage();
		AdcIf_BrakeMotorBPhaseSnesorVoltage();
		AdcIf_BrakeMainCylinderSensorVoltage();
		AdcIf_BrakePedalLocationSensorVoltage();
		AdcIf_BrakeMainCylinderPressureSensorVoltage();
		AdcIf_BrakePedalLocationSensor1();
		AdcIf_BrakePedalLocationSensor2();
		AdcIf_BrakeMainCylinderLocationSensor1();
		AdcIf_BrakeMainCylinderLocationSensor2();
		AdcIf_BrakeMotorCurrentSensor();
		AdcIf_EnvironmentTemperatureSensor();
		AdcIf_BrakeMainCylinderPressureSensor();
	}
	else
	{
		switch(g_adcSimulateDataIndex)
		{
			case 0:
				tAdcIf.vehiclePowerVoltage_Adc = g_adcSimulateData;
				break;
			case 1:
				tAdcIf.brakeMotorVlinkSensorVoltage_Adc = g_adcSimulateData;
				break;
			case 2:
				tAdcIf.brakeMotorAPhaseSensorVoltage_Adc = g_adcSimulateData;
				break;
			case 3:
				tAdcIf.brakeMotorBPhaseSensorVoltage_Adc = g_adcSimulateData;
				break;
			case 4:
				tAdcIf.brakeMainCylinderSensorVoltage_Adc = g_adcSimulateData;
				break;
			case 5:
				tAdcIf.brakePedalLocationSensorVoltage_Adc = g_adcSimulateData;
				break;
			case 6:
				tAdcIf.brakeMainCylinderPressureSensorVoltage_Adc = g_adcSimulateData;
				break;
			case 7:
				tAdcIf.brakePedalLocationSensor1_Adc = g_adcSimulateData;
				break;
			case 8:
				tAdcIf.brakePedalLocationSensor2_Adc = g_adcSimulateData;	
				break;
			case 9:
				tAdcIf.brakeMainCylinderLocationSensor1_Adc = g_adcSimulateData;
				break;
			case 10:
				tAdcIf.brakeMainCylinderLocationSensor2_Adc = g_adcSimulateData;
				break;
			case 11:
				tAdcIf.brakeMotorCurrentSensor_Adc = g_adcSimulateData;
				break;
			case 12:
				tAdcIf.environmentTemperatureSensor_Adc = g_adcSimulateData;
				break;
			case 13:
				tAdcIf.brakeMainCylinderPressureSensor_Adc = g_adcSimulateData;
				break;
			
			default:
				break;
		}
	}
	
	return;
}


/***********************************************************************
*  Name        : AdcIf_VehiclePowerVoltage
*  Description : Vehicle main power voltage ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_VehiclePowerVoltage(void)
{
	tAdcIf.vehiclePowerVoltage_Adc = AdcIf_SignalSample(ADC_CORE_VEHICLE_POWER_VOLTAGE, ADC_CHANNEL_VEHICLE_POWER_VOLTAGE);
	return;
}



/***********************************************************************
*  Name        : AdcIf_BrakeMotorVlinkSensorVoltage
*  Description : Brake motor vlink sensor power voltage ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakeMotorVlinkSensorVoltage(void)
{
	tAdcIf.brakeMotorVlinkSensorVoltage_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_MOTOR_VLINK_VOLTAGE, ADC_CHANNEL_BRAKE_MOTOR_VLINKE_VOLTAGE);	
	return;
}



/***********************************************************************
*  Name        : AdcIf_BrakeMotorAPhaseSensorVoltage
*  Description : Brake motor A phase sensor power voltage ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakeMotorAPhaseSensorVoltage(void)
{
	tAdcIf.brakeMotorAPhaseSensorVoltage_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_MOTOR_A_PHASE_VOLTAGE, ADC_CHANNEL_BRAKE_MOTOR_A_PHASE_VOLTAGE);
	return;
}


/***********************************************************************
*  Name        : AdcIf_BrakeMotorBPhaseSnesorVoltage
*  Description : Brake motor B phase sensor power voltage ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakeMotorBPhaseSnesorVoltage(void)
{
	tAdcIf.brakeMotorBPhaseSensorVoltage_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_MOTOR_B_PHASE_VOLTAGE, ADC_CHANNEL_BRAKE_MOTOR_B_PHASE_VOLTAGE);
	return;
}



/***********************************************************************
*  Name        : AdcIf_BrakeMainCylinderSensorVoltage
*  Description : Brake main cylinder sensor power voltage ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakeMainCylinderSensorVoltage(void)
{
	tAdcIf.brakeMainCylinderSensorVoltage_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_MAIN_CYLINDER_POWER_VOLTAGE, ADC_CHANNEL_BRAKE_MAIN_CYLINDER_POWER_VOLTAGE);
	return;
}



/***********************************************************************
*  Name        : AdcIf_BrakePedalLocationSensorVoltage
*  Description : Brake pedal location sensor power voltage ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakePedalLocationSensorVoltage(void)
{
	tAdcIf.brakePedalLocationSensorVoltage_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_PEDAL_LOCATION_POWER_VOLTAGE, ADC_CHANNEL_BRAKE_PEDAL_LOCATION_POWER_VOLTAGE);
	return;
}



/***********************************************************************
*  Name        : AdcIf_BrakeMainCylinderPressureSensorVoltage
*  Description : Brake main cylinder pressure sensor power voltage ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakeMainCylinderPressureSensorVoltage(void)
{
	tAdcIf.brakeMainCylinderPressureSensorVoltage_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_MAIN_CYLINDER_PRESSURE_POWER_VOLTAGE, ADC_CHANNEL_BRAKE_MAIN_CYLINDER_PRESSURE_POWER_VOLTAGE);
	return;
}


/***********************************************************************
*  Name        : AdcIf_BrakePedalLocationSensor1
*  Description : Brake pedal location sensor 1 ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakePedalLocationSensor1(void)
{
	tAdcIf.brakePedalLocationSensor1_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_PEDAL_LOCATION_1, ADC_CAHNNEL_BRAKE_PEDAL_LOCATION_1);
	return;
}


/***********************************************************************
*  Name        : AdcIf_BrakePedalLocationSensor2
*  Description : Brake pedal location sensor 2 ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakePedalLocationSensor2(void)
{
	tAdcIf.brakePedalLocationSensor2_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_PEDAL_LOCATION_2, ADC_CAHNNEL_BRAKE_PEDAL_LOCATION_2);
	return;
}


/***********************************************************************
*  Name        : AdcIf_BrakeMainCylinderLocationSensor1
*  Description : Brake main cylinder location sensor 1 ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakeMainCylinderLocationSensor1(void)
{
	tAdcIf.brakeMainCylinderLocationSensor1_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_MAIN_CYLINDER_LOCATION_1, ADC_CAHNNEL_BRAKE_MAIN_CYLINDER_LOCATION_1);
	return;
}


/***********************************************************************
*  Name        : AdcIf_BrakeMainCylinderLocationSensor2
*  Description : Brake main cylinder location sensor 2 ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakeMainCylinderLocationSensor2(void)
{
	tAdcIf.brakeMainCylinderLocationSensor2_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_MAIN_CYLINDER_LOCATION_2, ADC_CAHNNEL_BRAKE_MAIN_CYLINDER_LOCATION_2);
	return;
}


/***********************************************************************
*  Name        : AdcIf_BrakeMotorCurrentSensor
*  Description : Brake motor current ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakeMotorCurrentSensor(void)
{
	tAdcIf.brakeMotorCurrentSensor_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_MOTOR_CURRENT, ADC_CHANNEL_BRAKE_MOTOR_CURRENT);
	return;
}


/***********************************************************************
*  Name        : AdcIf_EnvironmentTemperatureSensor
*  Description : Environment temperature sensor ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_EnvironmentTemperatureSensor(void)
{
	tAdcIf.environmentTemperatureSensor_Adc = AdcIf_SignalSample(ADC_CORE_ENVIRONMENT_TEMPERATURE, ADC_CHANNEL_ENVIRONMENT_TEMPERATURE);
	return;
}


/***********************************************************************
*  Name        : AdcIf_BrakeMainCylinderPressureSensor
*  Description : Brake main cylinder pressure sensor ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_BrakeMainCylinderPressureSensor(void)
{
	tAdcIf.brakeMainCylinderPressureSensor_Adc = AdcIf_SignalSample(ADC_CORE_BRAKE_MAIN_CYLINDER_PRESSURE, ADC_CAHNNEL_BRAKE_MAIN_CYLINDER_PRESSURE);
	return;
}


/***********************************************************************
*  Name        : AdcIf_ReserveSensor1
*  Description : Reserve sensor 1 ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_ReserveSensor1(void)
{
	tAdcIf.reserveSensor1_Adc = AdcIf_SignalSample(ADC_CORE_RESERVE_1, ADC_CAHNNEL_RESERVE_1);
	return;
}


/***********************************************************************
*  Name        : AdcIf_ReserveSensor2
*  Description : Reserve sensor 2 ADC value
*  Parameter   : None
*  Returns     : None
***********************************************************************/
ADC_IF_STAT void AdcIf_ReserveSensor2(void)
{
	tAdcIf.reserveSensor2_Adc = AdcIf_SignalSample(ADC_CORE_RESERVE_2, ADC_CAHNNEL_RESERVE_2);
	return;
}




/***********************************************************************
*  Name        : AdcIf_SignalSample
*  Description : signal ADC sample function
*  Parameter   : AdcCore:0-ADC0, 1-ADC1
*                adcChannel: ADC channel of ADC0/ADC1 core
*  Returns     : adcValue: ADC factual value
***********************************************************************/
ADC_IF_STAT INT16U AdcIf_SignalSample(INT8U adcCore, INT8U adcChannel)
{
	ADC_IF_STAT INT16U adcValue = 0u;
	
	switch (adcCore)
	{
		case CORE_ADC0:
		{
			ADC0_vStartSeq2ReqChNum(0,0,0,adcChannel);
			while(ADC0_uwBusy());
			switch(adcChannel)
			{
				case 0:
					adcValue = ADC0_uwGetResultData_RESULT_REG_0;
				break;
				
				case 2:
					adcValue = ADC0_uwGetResultData_RESULT_REG_1;
				break;
				
				case 3:
					adcValue = ADC0_uwGetResultData_RESULT_REG_2;
				break;
				
				case 4:
				case 5:
				case 8:
				case 13:
					adcValue = ADC0_uwGetResultData_RESULT_REG_3;
				break;
				
				case 9:
					adcValue = ADC0_uwGetResultData_RESULT_REG_4;
				break;
				
				case 10:
					adcValue = ADC0_uwGetResultData_RESULT_REG_5;
				break;
				
				case 11:
					adcValue = ADC0_uwGetResultData_RESULT_REG_6;
				break;
				
				case 15:
					adcValue = ADC0_uwGetResultData_RESULT_REG_7;
				break;
				
				default:
				break;
			}	
		}break;
		
		case CORE_ADC1:
		{
			ADC1_vStartSeq2ReqChNum(0,0,0,adcChannel);
			while(ADC1_uwBusy());
			switch(adcChannel)
			{
				case 0:
					adcValue = ADC1_uwGetResultData_RESULT_REG_0;
				break;
				
				case 2:
					adcValue = ADC1_uwGetResultData_RESULT_REG_1;
				break;
				
				case 4:
					adcValue = ADC1_uwGetResultData_RESULT_REG_3;
				break;
				
				default:
				break;
			}
		}break;

		default:
		break;
	}
	
	return adcValue;
}


/***********************************************************************
*  Name        : AdcIf_InterfaceData
*  Description : ADC sample module interface data
*  Parameter   : None
*  Returns     : ADC sample data structure access address
***********************************************************************/
const T_ADC_IF* AdcIf_InterfaceData(void)
{
	return (&tAdcIf);
}	

/* _END_OF_ADC_IF_ */