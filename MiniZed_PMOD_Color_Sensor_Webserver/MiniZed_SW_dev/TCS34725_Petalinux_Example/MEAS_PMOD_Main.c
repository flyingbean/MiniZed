/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * MEAS_TCS34725_Main.c: Console Application for Testing the TCS34725
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <unistd.h>

#include "tcs34725.h"
#include "xplatform_to_app.h"




int test_tcs34725_sensor(const char * i2c_bus, int initOnly)
{

    char key_input;
    int i;
    tcs34725_status stat;





    // Set the AXI address of the IIC core
    if (*i2c_bus == '1')
    	 tcs34725_init("/dev/i2c-1");  // need to update for PMOD Color
    else if (*i2c_bus == '2')
    	 tcs34725_init("/dev/i2c-2");
    else {
    	printf("Error in I2C bus number valid inputs are: 1 or 2\n");
    	exit(-2);
    }

    // User of fcn can request only to init the I2C bus and bypass the options menu
    if (initOnly) {
        // Send the reset command to the TCS34725
        printf("\n");
        printf("Resetting TCS34725...\n");

        COLOR_SetENABLE(COLOR_REG_ENABLE_PON_MASK);
        usleep(2400);
       // COLOR_Interrupt_Disable();
        usleep(2400);
        COLOR_SetENABLE(COLOR_REG_ENABLE_PON_MASK | COLOR_REG_ENABLE_RGBC_INIT_MASK) ;
        usleep(2400);

        printf("COLOR_SetENABLE Complete. ");

        DemoRun();
    	return 0;
    }

    // Display the main menu

    // Infinite loop
 
    DemoRun();

    return 0;

}
void DemoRun() {
   u8 ID;
   COLOR_Data data;
   COLOR_Data byte;
  // CalibrationData calib;

   printf("Pmod COLOR Demo launched\r\n");
   ID = COLOR_GetID( );
   printf("Device ID = %02x\r\n", ID);

   data = COLOR_GetData();
   //calib = DemoInitCalibrationData(data);
   usleep(2400);
   int i;
   while(1) {
      data = COLOR_GetData();
   //   DemoCalibrate(data, &calib);
  //    data = DemoNormalizeToCalibration(data, calib);
      printf("r=%04x g=%04x b=%04x\n\r", data.r, data.g, data.b);
    //  byte = COLOR_ReadBye();
    //  printf("byte_r=%04x byte_g=%04x byte_b=%04x\n\r", byte.r, byte.g, byte.b);
      usleep(2400);
   }



    return;
}
