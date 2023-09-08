/*
 * tcs34725.c
 *
 *  Created on: Aug 12, 2014
 *      Author: JMBrinkhus
 */

//#include "xiic_l.h"
//#include "sleep.h"
//#include "math.h"
//#include "xparameters.h"
#include "xplatform_to_app.h"
#include "tcs34725.h"


// tcs34725 Global Variables
int 				tcs34725_axi_address;


/*********************************************************************
 *
 * Function:    tcs34725_init
 *
 * Description:	Initialize the axi address of the i2c core and the
 * 				internal resolution variable to t_14b_rh_12b to reflect
 * 				the sensor's initial resolution value on reset
 *
 * Parameters:	u32 axi_address -
 * 					Address of Xilinx AXI IIC Peripheral
 *
 * Returns:		void
 *
 *********************************************************************/
void tcs34725_init(const char * axi_address){
	u8 send_byte;
	tcs34725_axi_address = i2c_open(axi_address);
	printf("Pmod axi_address=0x%x\r\n", *axi_address);

	send_byte = XIIC_DGIER_OFFSET;

	XIic_Send(tcs34725_axi_address, TCS34725_I2C_ADDR, &send_byte, 1,   XIIC_STOP);

	return;
}


/*********************************************************************
 *
 * Function:    tcs34725_reset
 *
 * Description:	Send I2C Reset command to tcs34725 and wait 10ms
 *
 * Parameters:	none
 *
 * Returns:		Enumerated type tcs34725_status with possible values
 * 				tcs34725_status_ok or tcs34725_status_i2c_transfer_error
 *
 *********************************************************************/
tcs34725_status tcs34725_reset(void){

	char tx_buf[1];
	int byte_count=0;

	// Send I2C reset command
	tx_buf[0] = TCS34725_I2C_CMD_RESET;
	byte_count = XIic_Send(tcs34725_axi_address, TCS34725_I2C_ADDR, (u8*)tx_buf, 1, XIIC_STOP);

	if(byte_count!=1){
		return tcs34725_status_i2c_transfer_error;
	}

	// Wait 10ms
	usleep(10000);

	return tcs34725_status_ok;
}




/*********************************************************************
 *
 * Function:    CRC8
 *
 * Description:	Check 1 byte of data with 8 bits of CRC information.
 * 				Use polynomial X^8 + X^5 + X^4 + 1
 * 				For use with TSYS02D serial number read
 *
 * Parameters:	char* data -
 * 					2 byte character array. data[0] is the data byte
 * 					and data[1] contains the CRC information
 *
 * Returns:		TRUE if operation succeeded else FALSE
 *
 *********************************************************************/
/*
int CRC8(char* data){
	u32 div, poly;
	int i;
	div = 256*data[0]+data[1];
	poly = CRC_POLY;
	//printf("Divisor: 0x%X\n",(unsigned int)div);
	for(i=0;i<8;i++){
		if( (1<<(15-i))&div ){
			//printf("         0x%X\n         0x%X\n",(unsigned int)(poly<<(7-i)),(unsigned int)div);
			div ^= (poly<<(7-i));
		}
	}
	//printf("Result: 0x%X",(unsigned int)div);
	if( (div&(0xFF))==0x00 ){
		return TRUE;
	}else{
		return FALSE;
	}
}
*/

/*********************************************************************
 *
 * Function:    CRC16
 *
 * Description:	Check 2 bytes of data with 8 bits of CRC information.
 * 				Use polynomial X^8 + X^5 + X^4 + 1
 * 				For use with TSYS02D serial number read and checking
 * 				ADC results for several sensors
 *
 * Parameters:	char* data -
 * 					3 byte character array. data[0] is the most sig-
 * 					nificant byte, data[1] is the least significant
 * 					byte, and data[2] contains the CRC information
 *
 * Returns:		TRUE if operation succeeded else FALSE
 *
 *********************************************************************/
/*
int CRC16(char* data){
	u32 div, poly;
	int i;
	div = 256*256*data[0]+256*data[1]+data[2];
	poly = CRC_POLY;
	//printf("Divisor: 0x%X\n",(unsigned int)div);
	for(i=0;i<16;i++){
		if( (1<<(23-i))&div ){
			//printf("         0x%X\n         0x%X\n",(unsigned int)(poly<<(15-i)),(unsigned int)div);
			div ^= (poly<<(15-i));
		}
	}
	//printf("Result: 0x%X",(unsigned int)div);
	if( (div&(0xFF))==0x00 ){
		return TRUE;
	}else{
		return FALSE;
	}
}
*/
//sc---
/* ------------------------------------------------------------ */
/*    void COLOR_SetENABLE(PmodCOLOR *InstancePtr, u8 bits)
 **
 **   Parameters:
 **      InstancePtr: A PmodCOLOR object to use
 **      bits: The byte to write
 **
 **   Return Value:
 **      none
 **
 **   Errors:
 **      none
 **
 **   Description:
 **      Sets the contents of the PmodCOLOR's ENABLE register, used in the
 **      power up and initialization process
 */
void COLOR_SetENABLE( int bits) {
   u8 bytes[2] =
   {
	  COLOR_CMD_REPEAT_MASK | COLOR_RegENABLE,
      bits
   };

    XIic_Send(tcs34725_axi_address, TCS34725_I2C_ADDR, bytes, 2, XIIC_STOP);

}

u8 COLOR_GetID( ) {
   u8 bytes[1] = {0}, reg = COLOR_CMD_AUTO_INC_MASK | COLOR_RegID;

   XIic_Send(tcs34725_axi_address, TCS34725_I2C_ADDR, &reg, 1,XIIC_STOP);
   XIic_Recv(tcs34725_axi_address, TCS34725_I2C_ADDR, bytes, 1, XIIC_STOP);
   return bytes[0];
}

/* ------------------------------------------------------------ */
/*    COLOR_Data COLOR_GetData(PmodCOLOR *InstancePtr)
 **
 **   Parameters:
 **      InstancePtr: A PmodCOLOR object to use
 **
 **   Return Value:
 **      16 bit Clear/Red/Green/Blue color channel values
 **
 **   Errors:
 **      none
 **
 **   Description:
 **      Retrieves a data packet from the PmodCOLOR
 */
COLOR_Data COLOR_GetData( ) {
   u8 reg = {COLOR_CMD_AUTO_INC_MASK | COLOR_REG_ENABLE_RD |COLOR_RegCDATAL};


   COLOR_Data data;
   u8 buffer[8];
   XIic_Send(tcs34725_axi_address, TCS34725_I2C_ADDR, &reg, 1,   XIIC_STOP);
   XIic_Recv(tcs34725_axi_address, TCS34725_I2C_ADDR, buffer, 8,  XIIC_STOP);
   data.c = (buffer[1] << 8) | buffer[0];
   data.r = (buffer[3] << 8) | buffer[2];
   data.g = (buffer[5] << 8) | buffer[4];
   data.b = (buffer[7] << 8) | buffer[6];
   return data;
}


/******************************************************************************
function:   I2C read testing
******************************************************************************/
COLOR_Data COLOR_ReadBye( ) {
   u8 reg = {COLOR_CMD_AUTO_INC_MASK | COLOR_REG_ENABLE_RD |COLOR_RegCDATAL};

   COLOR_Data data;
   u8 buffer[2];
   XIic_Send(tcs34725_axi_address, TCS34725_I2C_ADDR, &reg, 1,   XIIC_STOP);
   XIic_Recv(tcs34725_axi_address, TCS34725_I2C_ADDR, buffer, 2,  XIIC_STOP);

   data.c = (buffer[1] << 8) | buffer[0];
   data.r = 0xDEADBEEF;
   data.g = 0xDEADBEEF;
   data.b = 0xDEADBEEF;
   return data;
}


