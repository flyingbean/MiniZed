#ifndef __XPLATFORM_TO_APP_
#define __XPLATFORM_TO_APP_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef __u8 u8;
typedef __u16 u16;
typedef __u32 u32;
typedef __s8 s8;
typedef __s16 s16;
typedef __s32 s32;
#define FALSE (0)
#define TRUE (1)
#define XIIC_STOP (0)

int i2c_open(const char * i2c_dev);
int i2c_close(int file);
s16 XIic_Send(int axi_address, u8 i2c_addr, const u8 * write_bytes, s16 num_bytes, u8 xii_null );
s16 XIic_Recv(int axi_address, u8 i2c_addr, u8 * read_bytes, s16 num_bytes, u8 xii_null);

int CRC8(char* data);
int CRC16(char* data);
#define CRC_POLY					(0x131)	// CRC Polynomial: X^8 + X^5 + X^4 + 1

#endif
