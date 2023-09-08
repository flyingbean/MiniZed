#include "xplatform_to_app.h"

#define MAX_NUM_OF_I2C_BUS (3)

static unsigned num_bus = 0;
static u8 last_i2c_addr[MAX_NUM_OF_I2C_BUS] = {0};
static int i2c_bus[MAX_NUM_OF_I2C_BUS] = {0};

static void setup_i2c_bus(int axi_address, u8 i2c_addr);
static int find_address_index(int axi_address);

int i2c_open(const char * i2c_dev)
{
	int file;
	file = open(i2c_dev, O_RDWR);
	if (file < 0) {
	    /* ERROR HANDLING: you can check errno to see what went wrong */
	    perror("Failed to open the i2c bus");
	    exit(1);
	}
	if (num_bus == MAX_NUM_OF_I2C_BUS) {
		close(file);
		printf("max i2c bus number exceeded!\n");
		exit(-1);
	}
	i2c_bus[num_bus++] = file;
	return (file);
}

int i2c_close(int file) {
	return (close(file));
}

s16 XIic_Send(int axi_address, u8 i2c_addr, const u8 * write_bytes, s16 num_bytes, u8 xii_null )
{
	s16 num_wrote;
	int r;
	r = find_address_index(axi_address);
	if (i2c_addr != last_i2c_addr[r]) {
		setup_i2c_bus(axi_address, i2c_addr);
	}
	num_wrote = write(axi_address,write_bytes,num_bytes);
	if (num_wrote != num_bytes) {
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus. %d\n", num_wrote);
	}
	return (num_wrote);
}

s16 XIic_Recv(int axi_address, u8 i2c_addr, u8 * read_bytes, s16 num_bytes, u8 xii_null)
{
	s16 num_read;
	int r;
	r = find_address_index(axi_address);
	if (i2c_addr != last_i2c_addr[r]) {
		setup_i2c_bus(axi_address, i2c_addr);
	}
	// Using I2C Read
	num_read = read(axi_address,read_bytes,num_bytes);
	if (num_read != num_bytes) {
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to read from the i2c bus. %d\n", num_read);
	}
	return (num_read);
}

static int find_address_index(int axi_address)
{
	int i;
	for(i=0; i<MAX_NUM_OF_I2C_BUS; i++) {
		if (i2c_bus[i]  == axi_address)
			return i;
	}

	printf("i2c bus address not in list!\n");
	exit (1);

	return -1;
}

static void setup_i2c_bus(int axi_address, u8 i2c_addr)
{
	int r;
	if (ioctl(axi_address,I2C_SLAVE,i2c_addr) < 0) {
		printf("Failed to acquire i2c bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}
	r = find_address_index(axi_address);
	if (r >= 0) {
		last_i2c_addr[r] = i2c_addr;
	}

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
int CRC8(char* data){
	u32 div, poly;
	int i;
	div = 256*data[0]+data[1];
	poly = CRC_POLY;

	for(i=0;i<8;i++){
		if( (1<<(15-i))&div ){
			div ^= (poly<<(7-i));
		}
	}

	if( (div&(0xFF))==0x00 ){
		return TRUE;
	}else{
		return FALSE;
	}
}

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
int CRC16(char* data){
	u32 div, poly;
	int i;
	div = 256*256*data[0]+256*data[1]+data[2];
	poly = CRC_POLY;

	for(i=0;i<16;i++){
		if( (1<<(23-i))&div ){
			div ^= (poly<<(15-i));
		}
	}

	if( (div&(0xFF))==0x00 ){
		return TRUE;
	}else{
		return FALSE;
	}
}
