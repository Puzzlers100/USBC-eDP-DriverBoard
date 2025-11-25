/*
 * i2c_comms.h
 *
 *  Created on: Aug 12, 2025
 *      Author: Jakub Nejedly
 */

#ifndef INC_I2C_COMMS_H_
#define INC_I2C_COMMS_H_

#include "main.h"
#include "eDP_adapt.h"


typedef struct s_usb2514bConf{
	uint16_t 	IDs[3];				//order VID, PID, DID
	uint8_t		config[3]; 			//1, 2, 3
	uint8_t		non_Rem_Dev;
	uint8_t 	port_Disable[2]; 	//byte order Bus,Self
	uint8_t 	max_Power[2];
	uint8_t 	hub_Current[2];
	uint8_t		pwr_On_Time;
	uint16_t	usb_Lang;
	wchar_t		mfr_String[31];
	wchar_t		prd_String[31];
	wchar_t		ser_String[31];
	uint8_t		BC_EN;
	uint8_t		boots_Iin;
	uint8_t		boost_Iout;
	uint8_t		prt_invert;
	uint16_t	prt_map;
}USB_2514B_Conf;

typedef struct s_i2c_data{
	osThreadId_t 	thread;
	uint8_t		 	mux_addr;
	uint8_t		 	hub_addr;
	uint8_t			INA_addr;
	USB_2514B_Conf	hub_conf;
}I2C_DATA;

extern I2C_DATA I2C_dat;

//I2C Flag bits
/*
 * 0 - orientation bit - 0 is normal, 1 is flipped
 * 1 - enable SS
 * 2 - enable DP
 * 3 - disable mux
 * 4 - configure hub
 * 5 - enable hub
 * 6 - reset hub
 *
 */

void I2C_init();

#endif /* INC_I2C_COMMS_H_ */
