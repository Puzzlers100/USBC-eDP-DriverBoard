/*
 * i2c_comms.c
 *
 *  Created on: Aug 12, 2025
 *      Author: jakub
 */

#include "i2c_comms.h"

I2C_DATA I2C_dat = {
		0,
		0xAA,
		0x58,
		0x80,
		{
		  {0x0424, 0x2514,  0x0BB3},	//order VID, PID, DID
		  {0x98, 0x20, 0x01},			//Conf_bytes 1, 2, 3
		  0x10,
		  {0x00, 0x00},
		  {50, 250},
		  {50, 50},
		  50,
		  0x0409,
		  L"Puzzlers100",
		  L"Portable Monitor Embedded Hub",
		  L"80085",
		  0x00,
		  0x00,
		  0x00,
		  0x00,
		  0x0000
		}
	  };

void INA_CONF(){
	uint8_t conf [2] = {0x80, 0x00};
	HAL_I2C_Mem_Write(&hi2c2, I2C_dat.INA_addr, 0x00, 1, conf, 2, 35);
	uint8_t calib[2] = {0x0A, 0xC7};
	HAL_I2C_Mem_Write(&hi2c2, I2C_dat.INA_addr, 0x05, 1, calib, 2, 35);
}

void INA234_READ(){
	uint16_t conf;
	HAL_I2C_Mem_Read(&hi2c2, I2C_dat.INA_addr, 0x05, 1, (uint8_t*)&conf, 2, 35);
	uint16_t current;
	HAL_I2C_Mem_Read(&hi2c2, I2C_dat.INA_addr, 0x04, 1, (uint8_t*)&current, 2, 35);
	ADC_Vals[2] = (uint32_t)((abs(((int16_t)((current&(0xff)) << 8 | (current&(0xff00)) >> 8))>>4) * 30405) >> 14);
	uint16_t voltage;
	HAL_I2C_Mem_Read(&hi2c2, I2C_dat.INA_addr, 0x02, 1, (uint8_t*)&voltage, 2, 35);
	ADC_Vals[3] = ((((voltage&(0xff)) << 4 | (voltage&(0xff00)) >> 12)) * 25.6);
}


uint8_t set_Mux (uint8_t SET){
	HAL_I2C_Mem_Write(&hi2c2, I2C_dat.mux_addr, 0x02, 1, &SET, 1, 35);
	uint8_t test = 255;
	HAL_I2C_Mem_Read(&hi2c2, I2C_dat.mux_addr, 0x02, 1, &test, 1, 35);
	if(test != SET){
		return 1;
	}
	return 0;
}

void disable_USB_HUB(){
	//Doesn't work, add GPIO reset line
	uint8_t reset_hub [2] = {1, 2};
	HAL_I2C_Mem_Write(&hi2c2, I2C_dat.hub_addr, 0xFF, 1, reset_hub, 2, 35);
}

void enable_USB_HUB(){
	uint8_t enable_hub[2] = {1, 1};
	HAL_I2C_Mem_Write(&hi2c2, I2C_dat.hub_addr, 0xFF, 1, enable_hub, 2, 35);
}

void convertor (USB_2514B_Conf * in, uint8_t * encoded){
	for(int i = 0; i < 3; i++){
		encoded[2*i] = (uint8_t)(in->IDs[i]&0xff);
		encoded[2*i+1] = (uint8_t)((in->IDs[i]>>8)&0xff);
		encoded[i+6] = in->config[i];
	}
	for(int i = 0; i < 2; i++){
		encoded[i+0x0A] = in->port_Disable[i];
		encoded[i+0x0C] = in->max_Power[i];
		encoded[i+0x0E] = in->hub_Current[i];
	}
	encoded[0x13] = 255;
	encoded[0x14] = 255;
	encoded[0x15] = 255;
	for(int i = 0; i < 31; i++){
		if(encoded[0x13] == 255){
			if(in->mfr_String[i] != 0){
				encoded[2 * i + 0x16] = (uint8_t)(in->mfr_String[i]&0xff);
				encoded[2 * i + 0x17] = (uint8_t)((in->mfr_String[i]>>8)&0xff);
			} else {
				encoded[0x13] = i;
			}
		}

		if(encoded[0x14] == 255){
			if(in->prd_String[i] != 0){
				encoded[2 * i + 0x54] = (uint8_t)(in->prd_String[i]&0xff);
				encoded[2 * i + 0x55] = (uint8_t)((in->prd_String[i]>>8)&0xff);
			} else {
				encoded[0x14] = i;
			}
		}

		if(encoded[0x15] == 255){
			if(in->ser_String[i] != 0){
				encoded[2 * i + 0x92] = (uint8_t)(in->ser_String[i]&0xff);
				encoded[2 * i + 0x93] = (uint8_t)((in->ser_String[i]>>8)&0xff);
			} else {
				encoded[0x15] = i;
			}
		}
	}
	if(encoded[0x13] > 31) encoded[0x13] = 31;
	if(encoded[0x14] > 31) encoded[0x14] = 31;
	if(encoded[0x15] > 31) encoded[0x15] = 31;

	encoded[0x11] = (uint8_t)(in->usb_Lang&0xff);
	encoded[0x12] = (uint8_t)((in->usb_Lang>>8)&0xff);

	encoded[0xFB] = (uint8_t)(in->prt_map&0xff);
	encoded[0xFC] = (uint8_t)((in->prt_map>>8)&0xff);

	encoded[0x09] = in->non_Rem_Dev;
	encoded[0x10] = in->pwr_On_Time;
	encoded[0xD0] = in->BC_EN;
	encoded[0xF6] = in->boots_Iin;
	encoded[0xF8] = in->boost_Iout;
	encoded[0xFA] = in->prt_invert;
}

void bulkTransfer(uint8_t * encoded, int blockSize, HAL_StatusTypeDef * errors){
      int numBlocks = 255/blockSize;
      int rem = 255 % blockSize;
	  uint8_t * tmp = (uint8_t*)malloc((1+numBlocks)*sizeof(uint8_t));
	  for(int i = 0; i < numBlocks; i++){
		  for(int j = 0; j < blockSize; j++){
			  tmp[j+1] = encoded[blockSize*i + j];
		  }
		  tmp[0] = blockSize;
		  errors[i] = HAL_I2C_Mem_Write(&hi2c2, 0x58, blockSize*i, 1, tmp, blockSize+1, 35);

	  }
	  if(rem){
		  for(int j = 0; j < rem; j++){
			  tmp[j+1] = encoded[255 - rem + j];
		  }
		  tmp[0] = rem;
		  errors[numBlocks] = HAL_I2C_Mem_Write(&hi2c2, 0x58, 255 - rem, 1, tmp, rem+1, 35);
	  }

	  free(tmp);
}

void config_USB_Hub(){
	uint8_t encode[256];
	HAL_StatusTypeDef err [256];
	convertor(&(I2C_dat.hub_conf), encode);
	bulkTransfer(encode, 32, err);
}


void I2C_Task(void * arg){
	uint32_t conf_time = osKernelGetTickCount();
	uint8_t hub_write = 0; //bit 0 is write protect, 1 is config, to is enable
	uint32_t boot_interval = 10 * osKernelGetTickFreq() * 0.01; //figure out the conversion from ticks to m
	while(1){
		uint32_t input = osThreadFlagsWait(0x7FFFFFFF, osFlagsWaitAny, osKernelGetTickFreq() * 0.01);
		if((input&0x8000000) == 0){
			if(input&0x00000008){
				set_Mux(1);
			} else if (input&0x00000004){
				set_Mux(6 | (input&0x00000001));
			} else if (input&0x00000002){
				set_Mux(4 | (input&0x00000001));
			}
			if((input&0x00000010) && (hub_write&0x01) == 0) hub_write |= 2;
			if((input&0x00000020) && (hub_write&0x01) == 0) hub_write |= 4;
			if(input&0x00000040){
				hub_write = 0;
				conf_time = osKernelGetTickCount();
				disable_USB_HUB();

			}
		}
		if(hub_write && (hub_write&0x01) != 1  && osKernelGetTickCount() >= conf_time + boot_interval){
			if(hub_write&0x02)	config_USB_Hub();
			if(hub_write&0x04){
				enable_USB_HUB();
				hub_write = 1;
			}
		}
		if(Port_Data[0].boardVerion != 0) INA234_READ();
	}
}

const osThreadAttr_t I2C_Task_attr = {
  .name = "I2C_Comms",
  .priority = (osPriority_t) osPriorityBelowNormal7,
  .stack_size = 128 * 16
};


void I2C_init(){
	I2C_dat.thread = osThreadNew(I2C_Task, NULL, &I2C_Task_attr);
	if(Port_Data[0].boardVerion != 0) INA_CONF();
}
