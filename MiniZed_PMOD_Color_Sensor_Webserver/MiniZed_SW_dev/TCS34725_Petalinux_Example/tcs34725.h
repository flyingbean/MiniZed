/*
 * tcs34725.h
 *
 *  Created on: Aug 26, 2023
 *  Author: Flyingbean
 *  Migrated from HTU21 driver developed by JMBrinkhus. The targeted part is TCS3475 on Zynq FPGA
 */

#include <stdio.h>
//#include "xiic_l.h"
#include "xplatform_to_app.h"
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#ifndef TCS34725_H_
#define TCS34725_H_

// TCS34725 Constants
#define TCS34725_I2C_ADDR				(0x29)	// 1000_000x
#define TCS34725_14B_CONV_DELAY_MS	    (50)	// Datasheet claims max conversion time of 50ms for 14 bit resolution
#define TCS34725_13B_CONV_DELAY_MS	    (25)	// Datasheet claims max conversion time of 25ms for 13 bit resolution
#define TCS34725_12B_CONV_DELAY_MS	    (13)	// Datasheet claims max conversion time of 13ms for 12 bit resolution
#define TCS34725_11B_CONV_DELAY_MS	     (7)		// Datasheet claims max conversion time of  7ms for 11 bit resolution
//#define CRC_POLY					(0x131)	// CRC Polynomial: X^8 + X^5 + X^4 + 1

// TCS34725 BIT MASKS
#define TCS34725_RESOLUTION_BIT7_MASK				(0x80)
#define TCS34725_RESOLUTION_BIT0_MASK				(0x01)
#define TCS34725_BATTERY_STATUS_MASK				(0x40)
#define TCS34725_HEATER_STATUS_MASK		     		(0x04)

// TCS34725 I2C Commands
#define TCS34725_I2C_CMD_RESET					(0xFE)
#define TCS34725_I2C_CMD_MEAS_TEMP_WITHOUT_HOLD	(0xF3)
#define TCS34725_I2C_CMD_MEAS_HUM_WITHOUT_HOLD	(0xF5)
#define TCS34725_I2C_CMD_WRITE_USER_REG			(0xE6)
#define TCS34725_I2C_CMD_READ_USER_REG			(0xE7)

// Typedef'ed Enumerations
typedef enum tcs34725_status{
	tcs34725_status_ok = 0,
	tcs34725_status_i2c_transfer_error = 1,
	tcs34725_status_crc_error = 2
}tcs34725_status;

#endif /* TCS34725_H_ */

// External variables
extern 	int 				tcs34725_axi_address;


//sc---
/**************************** Type Definitions ********************************/


typedef struct {
  int r, g, b, c;
} COLOR_Data;

typedef struct {
   COLOR_Data min, max;
} CalibrationData;

/************************** Register Definitions ******************************/

typedef enum {
   COLOR_RegENABLE  = 0x00,         // Enables states and interrupts
   COLOR_RegATIME   = 0x01,         // RGBC time

   COLOR_RegWTIME   = 0x03,         // Wait time
   COLOR_RegAILTL   = 0x04,         // Clear interrupt low threshold low byte
   COLOR_RegAILTH,                  // Clear interrupt low threshold high byte
   COLOR_RegAIHTL,                  // Clear interrupt high threshold low byte
   COLOR_RegAIHTH,                  // Clear interrupt high threshold high byte
   COLOR_RegPERS    = 0x0C,         // Interrupt persistence filter
   COLOR_RegCONFIG  = 0x0D,         // Configuration
   COLOR_RegCONTROL = 0x0F,         // Control
   COLOR_RegID      = 0x12,         // Device ID
   COLOR_RegSTATUS  = 0x13,         // Device status
   COLOR_RegCDATAL  = 0x14 ,        // Clear data low byte
   COLOR_RegCDATAH  = 0x15,         // Clear data high byte
   COLOR_RegRDATAL  = 0x16,         // Red data low byte
   COLOR_RegRDATAH  = 0x17,         // Red data high byte
   COLOR_RegGDATAL  = 0x18,         // Green data low byte
   COLOR_RegGDATAH  = 0x19,         // Green data high byte
   COLOR_RegBDATAL  = 0x1A,         // Blue data low byte
   COLOR_RegBDATAH  = 0x1B          // Blue data high byte
} COLOR_Reg;

#define COLOR_REG_ENABLE_PON_MASK 0x01
#define COLOR_REG_ENABLE_RGBC_INIT_MASK 0x02
#define COLOR_REG_ENABLE_AIEN 0x10
#define COLOR_REG_ENABLE_RD 0x20
#define COLOR_CMD_REPEAT_MASK (0x80)
#define COLOR_CMD_AUTO_INC_MASK (0xA0)
#define XIIC_DGIER_OFFSET	(0x1C)  /**< Global Interrupt Enable Register */
//---sc
// Function Declarations
void			tcs34725_init(const char * axi_address);
tcs34725_status	tcs34725_reset(void);

//sc---
void COLOR_SetENABLE(int bits);
u8 COLOR_GetID();
COLOR_Data COLOR_GetData();
COLOR_Data COLOR_ReadBye();



extern int CRC8(char*);
extern int CRC16(char*);
extern int test_tcs34725_sensor(const char * i2c_bus, int initOnly);
