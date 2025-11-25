/*
 * eDP_adapt.c
 *
 *  Created on: Jul 29, 2025
 *      Author: jakub
 */
#include "eDP_adapt.h"

uint32_t ADC_Vals[4];

USBPD_DATA * Port_Data;


void PUZ_USBPD_INIT(uint8_t numPorts){
	Port_Data = (USBPD_DATA*)malloc(sizeof(USBPD_DATA) * numPorts);

	Port_Data[0].boardVerion = ( (uint8_t)HAL_GPIO_ReadPin(VER0_GPIO_Port, VER0_Pin) | ((uint8_t)HAL_GPIO_ReadPin(VER1_GPIO_Port, VER1_Pin) << 1))^3;
	for(uint8_t i = 0; i < numPorts; i++){
		USBPD_VDM_UserInit(i);
	}

	USBPD_IDHeaderVDO_TypeDef ID_Header;
	ID_Header.b30.VID = 42069;
	ID_Header.b30.ConnectorType = 2;
	ID_Header.b30.ProductTypeDFP = 0;
	ID_Header.b30.ModalOperation = 1;
	ID_Header.b30.ProductTypeUFPorCP = 2;
	ID_Header.b30.USBDevCapability = 0;
	ID_Header.b30.USBDevCapability = 0;

	Port_Data[0].Identity.IDHeader = ID_Header;
	Port_Data[0].Identity.CertStatVDO.d32 = 0x00080085;
	Port_Data[0].Identity.ProductVDO.b.USBProductId = 0x0420;
	Port_Data[0].Identity.ProductVDO.b.bcdDevice = 0x0069;
	Port_Data[0].Identity.UFP_VDO.d32 = 0x62000012;
	Port_Data[0].Identity.AMA_VDO_Presence = 0;
	Port_Data[0].Identity.DFP_VDO_Presence = 0;
	Port_Data[0].Identity.UFP_VDO_Presence = 1;

	Port_Data[0].numSVIDs = 1;
	Port_Data[0].SVIDs[0] = 0xFF01;

	Port_Data[0].DP_MODE = 0x00280045;

	Port_Data[0].orientation = NONE;
	I2C_init();

	if(Port_Data[0].boardVerion != 0){
		INA234_READ();
		Port_Data[0].idleConsup = ADC_Vals[2];

		TIM2->CCR2 = TIM2->ARR / 10;
		HAL_GPIO_WritePin(BL_Enable_GPIO_Port, BL_Enable_Pin, GPIO_PIN_SET);
		osDelay(3);

		INA234_READ();
		Port_Data[0].backlightPwrScale =  (ADC_Vals[2] - Port_Data[0].idleConsup) * 10;
		HAL_GPIO_WritePin(BL_Enable_GPIO_Port, BL_Enable_Pin, GPIO_PIN_RESET);

	}

}

void PUZ_USBPD_StorePDOs(uint8_t PortNum, uint8_t *Ptr, uint32_t Size){
	if (Size <= (USBPD_MAX_NB_PDO * 4)){
	  uint8_t* rdo;
	  Port_Data[PortNum].numPDOs = (Size / 4);

	  for (uint32_t index = 0; index < (Size / 4); index++){
		rdo = (uint8_t*)&Port_Data[PortNum].PDOs[index];
		(void)memcpy(rdo, (Ptr + (index * 4u)), (4u * sizeof(uint8_t)));
	  }
	}
	return;

}

void PUZ_USBPD_SNK_Capability(uint8_t PortNum, uint32_t *PtrRequestData, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObjectType){

	int PDO_Index = 0;
	PDO_Types PDO_type;
	int maxCurIndex;
	Port_Data[PortNum].maxCurrent= 0;
	for(PDO_Index = 0; PDO_Index < Port_Data[PortNum].numPDOs; PDO_Index++){
		uint32_t PDO_raw = Port_Data[PortNum].PDOs[PDO_Index];
		PDO_type = (PDO_Types)((PDO_raw & 0xC0000000) >> 30);

		int milivoltage_max = 0;
		int milivoltage_min = 0;
		int miliamperage = 0;

		switch(PDO_type){
				case FIXED:
					milivoltage_max = ((PDO_raw & 0x000FFC00) >> 10)*50;
					milivoltage_min = ((PDO_raw & 0x000FFC00) >> 10)*50;
					miliamperage = (PDO_raw & 0x000003FF)*10;
					break;
				case BATTERY:
					milivoltage_max = ((PDO_raw & 0x3FF00000) >> 10)*50;
					milivoltage_min = ((PDO_raw & 0x000FFC00) >> 10)*50;
					miliamperage = (PDO_raw & 0x000003FF)*10;
					break;
				case VARIABLE:
					milivoltage_max = ((PDO_raw & 0x3FF00000) >> 10)*50;
					milivoltage_min = ((PDO_raw & 0x000FFC00) >> 10)*50;
					miliamperage = ((PDO_raw & 0x000003FF)*250000)/milivoltage_max;
					break;
				case AUGMENTED:
					if((PDO_Types)((PDO_raw & 0x30000000) >> 28) == 0){
							milivoltage_max = ((PDO_raw & 0x01FE0000) >> 17)*100;
							milivoltage_min = ((PDO_raw & 0x0000FF00) >> 8)*100;
							miliamperage = (PDO_raw & 0x0000007F)*50;
					}
					break;
				default:
					break;
		}
		//checking if supply can provide 5V at 3A
		if((milivoltage_max >= 5000) && (milivoltage_min <= 5000) && ( miliamperage >= 3000)){
			Port_Data[PortNum].maxCurrent = miliamperage;
			Port_Data[PortNum].mismatch = 0;
			PDO_Index += 1;
			break;
		} else if((milivoltage_max >= 5000) && (milivoltage_min <= 5000) && ( miliamperage > Port_Data[PortNum].maxCurrent) ){
			Port_Data[PortNum].maxCurrent = miliamperage;
			maxCurIndex = PDO_Index+1;
		}
	}

	USBPD_SNKRDO_TypeDef rdo;
	rdo.d32 = 0;
	rdo.FixedVariableRDO.ObjectPosition = PDO_Index;
	rdo.FixedVariableRDO.NoUSBSuspend = 1;
	rdo.FixedVariableRDO.OperatingCurrentIn10mAunits = Port_Data[PortNum].maxCurrent/10;
	rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = Port_Data[PortNum].maxCurrent/10;
	rdo.FixedVariableRDO.CapabilityMismatch = 0;
	*PtrPowerObjectType = USBPD_CORE_PDO_TYPE_FIXED;

	if(Port_Data[PortNum].mismatch){
		rdo.FixedVariableRDO.ObjectPosition = maxCurIndex;
		rdo.FixedVariableRDO.ObjectPosition = 1;
		rdo.FixedVariableRDO.CapabilityMismatch = 1;
	} else {
		switch(PDO_type){
		case FIXED:
			break;
		case VARIABLE:
			*PtrPowerObjectType = USBPD_CORE_PDO_TYPE_VARIABLE;
			break;
		case BATTERY:
			rdo.BatteryRDO.OperatingPowerIn250mWunits = 60;
			rdo.BatteryRDO.MaxOperatingPowerIn250mWunits = 60;
			*PtrPowerObjectType = USBPD_CORE_PDO_TYPE_BATTERY;
			break;
		case AUGMENTED:
			rdo.d32 = 0;
			rdo.ProgRDO.ObjectPosition = PDO_Index;
			rdo.ProgRDO.NoUSBSuspend = 1;
			rdo.ProgRDO.OutputVoltageIn20mV = 250;
			rdo.ProgRDO.OperatingCurrentIn50mAunits = 60;
			*PtrPowerObjectType = USBPD_CORE_PDO_TYPE_APDO;
			break;
		default:
			rdo.FixedVariableRDO.ObjectPosition = 1;
			rdo.FixedVariableRDO.CapabilityMismatch = 1;
			break;

		}
	}

	if(Port_Data[PortNum].maxCurrent  > 1550) osThreadFlagsSet(I2C_dat.thread, 0x30);

	*PtrRequestData = rdo.d32;
}

int PUZ_USBPD_VBUSVOLT(uint32_t Instance, uint32_t * pVoltage){
	int32_t ret = BSP_ERROR_NONE;

	if ((Instance >= USBPD_PWR_INSTANCES_NBR) || (NULL == pVoltage))
	{
		ret = BSP_ERROR_WRONG_PARAM;
		*pVoltage = 0u;
	} else {
		uint32_t val = 0;
		val = __LL_ADC_CALC_DATA_TO_VOLTAGE( VDDA_APPLI, ADC_Vals[0], LL_ADC_RESOLUTION_12B);
		val *= 582;
		val /= 100;
		*pVoltage = val ;
	}
	return ret;
}

USBPD_StatusTypeDef PUZ_USBPD_DiscID(uint8_t PortNum, USBPD_DiscoveryIdentity_TypeDef *pIdentity){

	if(PortNum == 0){
		*pIdentity = Port_Data[0].Identity;
		return USBPD_ACK;
	}
	return USBPD_NAK;
}


USBPD_StatusTypeDef PUZ_USBPD_DiscSVIDs(uint8_t PortNum, uint16_t **p_SVID_Info, uint8_t *pNbSVID){
	if(PortNum == 0){
		*pNbSVID = Port_Data[0].numSVIDs;
		*p_SVID_Info = Port_Data[0].SVIDs;
		return USBPD_ACK;
	}
	return USBPD_NAK;
}


USBPD_StatusTypeDef PUZ_USBPD_DiscModes(uint8_t PortNum, uint16_t SVID, uint32_t **p_ModeTab, uint8_t *NumberOfMode){
	if(PortNum == 0 && SVID == 0xFF01){
		*NumberOfMode = 1;
		*p_ModeTab = &Port_Data[0].DP_MODE;
		return USBPD_ACK;
	}
	return USBPD_NAK;
}

USBPD_StatusTypeDef PUZ_USBPD_Specifics(uint8_t PortNum, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *pNbData, uint32_t *pVDO){
	if(PortNum == 0){
		switch(VDMCommand){
			default:
				return USBPD_NAK;
				break;
			case SVDM_SPECIFIC_1:
					*pNbData = 1;
					*pVDO = 0x0000009A;
					if(Port_Data[0].maxCurrent > 1200){
						*pVDO |= 0x0000000C;
						TIM2->CCR2 = 0;
						HAL_GPIO_WritePin(BL_Enable_GPIO_Port, BL_Enable_Pin, GPIO_PIN_SET);
					} else {
					}
				break;
			case SVDM_SPECIFIC_2:
				break;
		}
		return USBPD_ACK;
	}
	return USBPD_NAK;
}

USBPD_StatusTypeDef PUZ_USBPD_ENTER( uint8_t PortNum, uint16_t SVID, uint32_t ModeIndex){
	uint32_t SET = Port_Data[PortNum].orientation == FLIPPED ? 0x35 : ( Port_Data[PortNum].orientation == NORMAL ? 0x34 : 0x38);
	osThreadFlagsSet(I2C_dat.thread, SET);
	return USBPD_ACK;
}

void PUZ_USBPD_PlugDet(uint8_t PortNum){
	int CC = DPM_Params[PortNum].ActiveCCIs;
	Port_Data[PortNum].orientation = CC == 1 ? FLIPPED : (CC == 2 ? NORMAL : NONE);
	uint32_t SET = CC == 1 ? 0x03 : ( CC == 2 ? 0x02 : 0x08);
	osThreadFlagsSet(I2C_dat.thread, SET);
}

void initTask(){
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

void mainLoop(){
	TIM2->CCR2 = ADC_Vals[BL_SENSE_ADC]<<2;
	osDelay(1);
}



