/*******************************************************************************
 * Code Written by Jakub Nejedly
 *
 */

#ifndef __EDP_ADAPT_H
#define __EDP_ADAPT_H

#define V_SENSE_ADC 0
#define BL_SENSE_ADC 1

#define MUX_ADDR 0xAA

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
//#include "task.h"
#include "usbpd_def.h"
#include "usbpd_pwr_if.h"
#include "usbpd_vdm_user.h"
#include "usbpd_pwr_user.h"
#include "usbpd_dpm_core.h"
#include "stm32g0xx.h"
#include "main.h"
#include "cmsis_os2.h"
#include "i2c_comms.h"

extern uint32_t ADC_Vals[4];


extern TIM_HandleTypeDef htim2;

extern I2C_HandleTypeDef hi2c2;

typedef enum e_pdo_types{
	FIXED = 0,
	BATTERY = 1,
	VARIABLE = 2,
	AUGMENTED = 3
}PDO_Types;

typedef enum e_mux_opt{
	OPEN = 0,
	USB_N = 4,
	USB_F = 5,
	eDP_N = 6,
	eDP_F = 7
}mux_Options;

typedef enum e_plug_ort{
	NONE,
 	NORMAL,
	FLIPPED,
}plug_Ort;

typedef struct s_src_pdos{
	uint8_t boardVerion;
	uint8_t numPDOs;
	uint32_t PDOs[7];
	uint8_t mismatch;
	uint32_t maxCurrent;
	USBPD_DiscoveryIdentity_TypeDef Identity;
	uint16_t SVIDs[12];
	uint8_t numSVIDs;
	uint32_t DP_MODE;
	plug_Ort orientation;
} USBPD_DATA;

extern USBPD_DATA * Port_Data;

void PUZ_USBPD_SNK_Capability(uint8_t PortNum, uint32_t *PtrRequestData, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObjectType);

void PUZ_USBPD_INIT(uint8_t numPorts);

void PUZ_USBPD_StorePDOs(uint8_t PortNum, uint8_t *Ptr, uint32_t Size);

int PUZ_USBPD_VBUSVOLT(uint32_t Instance, uint32_t * pVoltage);

USBPD_StatusTypeDef PUZ_USBPD_DiscID(uint8_t PortNum, USBPD_DiscoveryIdentity_TypeDef *pIdentity);

USBPD_StatusTypeDef PUZ_USBPD_DiscSVIDs(uint8_t PortNum, uint16_t **p_SVID_Info, uint8_t *pNbSVID);

USBPD_StatusTypeDef PUZ_USBPD_DiscModes(uint8_t PortNum, uint16_t SVID, uint32_t **p_ModeTab, uint8_t *NumberOfMode);

USBPD_StatusTypeDef PUZ_USBPD_Specifics(uint8_t PortNum, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *pNbData, uint32_t *pVDO);

USBPD_StatusTypeDef PUZ_USBPD_ENTER( uint8_t PortNum, uint16_t SVID, uint32_t ModeIndex);

void PUZ_USBPD_PlugDet(uint8_t PortNum);

void BL_setPWM();

void initTask();

void mainLoop();

#endif
