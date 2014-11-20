//#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <string.h>
#include "I2CWrapper.h"



I2CWrapper::I2CWrapper()
  {
    handle = -1;
    FailedFlag=0;
  }


I2CWrapper::~I2CWrapper()
  {
    Close();
  }



void I2CWrapper::Close()
  {
    if(handle >=0)
       close(handle);
    handle =-1;
  }


////////////////////////////////////   I2CWrapper::Open
//
//    Open I2C I/O
//
//    Inputs,
//
//    BUS :  select  bus  (Rpi is 0 or 1)
//    SlaveAddress:  between 0x3 .. 0x77
//
//    Return,
//
//    IO handle for  open()
//


int I2CWrapper::Open(int BUS, int SlaveAddress)
    {

	  char I2C_dev[256];

	  if(BUS < 0) return -1;
	  if(BUS > 1) return -1;

	  sprintf(I2C_dev,"/dev/i2c-%d", BUS);

          Close(); // close handle if it is opened!

          handle = open(I2C_dev, O_RDWR);
	  if(handle < 0)
	   {
              FailedFlag=1;
	      return -1;
	   }
	  if(SetSlaveAddress(SlaveAddress) < 0)
	   {
              Close();
              return -2;
	   }
	   return handle;
	}



////////////////////////////////////   I2CWrapper:: SetSlaveAddress
//
//    Specify which Slave device you want to talk to
//
//     inputs,
//
//     SlaveAddress:
//
//    Return integer
//
//    0  ok
//    < 0 error
//

int I2CWrapper::SetSlaveAddress(int SlaveAddress)
    {
       if(SlaveAddress < 3) return -1;
       if(SlaveAddress > 0x77)return -1;

	   if (ioctl(handle, I2C_SLAVE, SlaveAddress) < 0)
	    {
               FailedFlag=1;
		return -1;
            }
        return SlaveAddress; // ok
    }

////////////////////////////////////   I2CWrapper::ReadBlock
//
//    Read N byte from the I2C (maximum of 31 bytes possible)
//
//     inputs,
//
//     cmd:  Specify which is the device command (more or less the device function or register)
//     size:     Number of bytes to read
//     array:    the pointer array
//
//    Return integer
//
//    number of byte reed.
//    < 0 error
//
// Be aware than SMBUS block handling for I2C is different
// blk.size has to be I2C_SMBUS_I2C_BLOCK_DATA
// i2cdata.block[0] hold the number of byte to be transfered. Maximum  is 31 bytes because cmd counts for one byte
// The data  start at i2cdata.block[1]

int I2CWrapper::ReadBlock(unsigned char cmd, unsigned char  size,  void * array)
{
 struct i2c_smbus_ioctl_data  blk;
 union i2c_smbus_data i2cdata;

  blk.read_write=1;
  blk.command=cmd;
  blk.size= I2C_SMBUS_I2C_BLOCK_DATA;
  blk.data=&i2cdata;
  i2cdata.block[0]=size;

  if(ioctl(handle,I2C_SMBUS,&blk)<0){
    FailedFlag = 1;
    return -1;
    }

 memcpy(array,&i2cdata.block[1],size);
 return   i2cdata.block[0];
}

////////////////////////////////////   I2CWrapper::ReadByte
//
//    Read 1 byte  from the I2C
//
//     inputs,
//
//     cmd:  Specify which is the device command (more or less the device function or register)
//
//    Return  byte data (in int),  if <0 error
//
//    Check I2CWrapper::FailedFlag for error
//
unsigned char  I2CWrapper::ReadByte(unsigned char cmd)
{
  struct i2c_smbus_ioctl_data  blk;
  union i2c_smbus_data i2cdata;

  blk.read_write=1;
  blk.command=cmd;
  blk.size=I2C_SMBUS_BYTE_DATA;
  blk.data=&i2cdata;

  if(ioctl(handle,I2C_SMBUS,&blk)<0){
    FailedFlag=1;
    }
  return   i2cdata.byte;
}


////////////////////////////////////   I2CWrapper::WriteByte
//
//    Write 1 byte  to the I2C device
//
//     inputs,
//
//     cmd:  Specify which is the device command (more or less the device function or register)
//     value:    byte value
//    Return   number of byte written if <0 error

int I2CWrapper::WriteByte(unsigned char cmd, unsigned char value)
{
 struct i2c_smbus_ioctl_data  blk;
 union i2c_smbus_data i2cdata;

 i2cdata.byte=value;
 blk.read_write=0;
 blk.command=cmd;
 blk.size=I2C_SMBUS_BYTE_DATA;
 blk.data= &i2cdata;

  if(ioctl(handle,I2C_SMBUS,&blk)<0){
    FailedFlag=1;
    return -1;
    }
 return 1;
}


////////////////////////////////////   I2CWrapper::ReadWord
//
//    Read 2 bytes  from the I2C
//
//     inputs,
//
//     cmd:  Specify which is the device command (more or less the device function or register)
//
//    Return  word data  in ( short long format), if < 0 error
//
//    Check I2CWrapperErrorFlag for error
//
unsigned short  I2CWrapper::ReadWord(unsigned char cmd)
{
 struct i2c_smbus_ioctl_data  blk;
 union i2c_smbus_data i2cdata;


 blk.read_write=1;
 blk.command=cmd;
 blk.size=I2C_SMBUS_WORD_DATA;
 blk.data=&i2cdata;

 if(ioctl(handle,I2C_SMBUS,&blk)<0){
    FailedFlag=1;
    }
 return  i2cdata.word;
}


////////////////////////////////////   I2CWrapper::WriteWord
//
//    Write 2 bytes  to the I2C device
//
//     inputs,
//
//     handle:   IO handle
//     cmd:  Specify which is the device command (more or less the device function or register)
//     value:    byte value
//    Return   number of byte written if <0 error
//
//    Check I2CWrapperErrorFlag for error
//
int I2CWrapper::WriteWord(unsigned char cmd, unsigned short value)
{
 struct i2c_smbus_ioctl_data  blk;
  union i2c_smbus_data i2cdata;

  i2cdata.word=value;
  blk.read_write=0;
  blk.command=cmd;
  blk.size=I2C_SMBUS_WORD_DATA;
  blk.data= &i2cdata;

  if(ioctl(handle,I2C_SMBUS,&blk)<0){
    FailedFlag=1;
    return -1;
    }
 return 2;
}



