#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "I2CWrapper.h"
#include "mpu6050.h"

int MPU6050_Test_I2C(int handle)
{
    unsigned char Data = 0x00;
    Data = I2CWrapperReadByte(handle, MPU6050_RA_WHO_AM_I);
 
    if(Data == 0x68)
    {
      printf("I2C Read Test Passed\r\n");       
      return(1);   
   }
     
    printf("ERROR: I2C Read Test Failed, returned=%X Stopping\r\n",Data);
 
    return(0);
}


void Setup_MPU6050(int handle)
{
   int loop;
   unsigned char TheReg;
  
// I need code space. Just create a table with register to clear
const unsigned char MPU6050RegTable[]= {
    MPU6050_RA_FF_THR,    		//Freefall threshold of |0mg|  LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_FF_THR, 0x00);
    MPU6050_RA_FF_DUR,    		//Freefall duration limit of 0   LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_FF_DUR, 0x00);
    MPU6050_RA_MOT_THR,		//Motion threshold of 0mg     LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_MOT_THR, 0x00);
    MPU6050_RA_MOT_DUR,    		//Motion duration of 0s    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_MOT_DUR, 0x00);
    MPU6050_RA_ZRMOT_THR,    	//Zero motion threshold    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_ZRMOT_THR, 0x00);
    MPU6050_RA_ZRMOT_DUR,    	//Zero motion duration threshold    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_ZRMOT_DUR, 0x00);
    MPU6050_RA_FIFO_EN,    		//Disable sensor output to FIFO buffer    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_FIFO_EN, 0x00);
    MPU6050_RA_I2C_MST_CTRL,    //AUX I2C setup    //Sets AUX I2C to single master control, plus other config    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_MST_CTRL, 0x00);
    MPU6050_RA_I2C_SLV0_ADDR,  //Setup AUX I2C slaves    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_ADDR, 0x00);
    MPU6050_RA_I2C_SLV0_REG,   	//    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_REG, 0x00);
    MPU6050_RA_I2C_SLV0_CTRL,  	//    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_CTRL, 0x00);
    MPU6050_RA_I2C_SLV1_ADDR, // LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_ADDR, 0x00);
    MPU6050_RA_I2C_SLV1_REG,   //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_REG, 0x00);
    MPU6050_RA_I2C_SLV1_CTRL,  //LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_CTRL, 0x00);
    MPU6050_RA_I2C_SLV2_ADDR,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_ADDR, 0x00);
    MPU6050_RA_I2C_SLV2_REG,    //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_REG, 0x00);
    MPU6050_RA_I2C_SLV2_CTRL,   //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_CTRL, 0x00);
    MPU6050_RA_I2C_SLV3_ADDR,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_ADDR, 0x00);
    MPU6050_RA_I2C_SLV3_REG,    //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_REG, 0x00);
    MPU6050_RA_I2C_SLV3_CTRL,   //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_CTRL, 0x00);
    MPU6050_RA_I2C_SLV4_ADDR,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_ADDR, 0x00);
    MPU6050_RA_I2C_SLV4_REG,   //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_REG, 0x00);
    MPU6050_RA_I2C_SLV4_DO,     //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_DO, 0x00);
    MPU6050_RA_I2C_SLV4_CTRL,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_CTRL, 0x00);
    MPU6050_RA_I2C_SLV4_DI,      //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_DI, 0x00);
    MPU6050_RA_INT_PIN_CFG,     //MPU6050_RA_I2C_MST_STATUS //Read-only    //Setup INT pin and AUX I2C pass through    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x00);
    MPU6050_RA_INT_ENABLE,    //Enable data ready interrupt      LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_INT_ENABLE, 0x00);
    MPU6050_RA_I2C_SLV0_DO,  //Slave out, dont care    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_DO, 0x00);
    MPU6050_RA_I2C_SLV1_DO,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_DO, 0x00);
    MPU6050_RA_I2C_SLV2_DO,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_DO, 0x00);
    MPU6050_RA_I2C_SLV3_DO,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_DO, 0x00);
    MPU6050_RA_I2C_MST_DELAY_CTRL, //More slave config      LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_MST_DELAY_CTRL, 0x00);
    MPU6050_RA_SIGNAL_PATH_RESET,  //Reset sensor signal paths    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_SIGNAL_PATH_RESET, 0x00);
    MPU6050_RA_MOT_DETECT_CTRL,     //Motion detection control    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_MOT_DETECT_CTRL, 0x00);
    MPU6050_RA_USER_CTRL,                 //Disables FIFO, AUX I2C, FIFO and I2C reset bits to 0    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_USER_CTRL, 0x00);
    MPU6050_RA_CONFIG,                       //Disable FSync, 256Hz DLPF    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_CONFIG, 0x00);
    MPU6050_RA_FF_THR,				   //Freefall threshold of |0mg|    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_FF_THR, 0x00);
    MPU6050_RA_FF_DUR,			       //Freefall duration limit of 0    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_FF_DUR, 0x00);
    MPU6050_RA_MOT_THR,                 //Motion threshold of 0mg    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_MOT_THR, 0x00);
    MPU6050_RA_MOT_DUR,			    //Motion duration of 0s    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_MOT_DUR, 0x00);
    MPU6050_RA_ZRMOT_THR,	    //Zero motion threshold    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_ZRMOT_THR, 0x00);
    MPU6050_RA_ZRMOT_DUR,      //Zero motion duration threshold    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_ZRMOT_DUR, 0x00);
    MPU6050_RA_FIFO_EN,		    //Disable sensor output to FIFO buffer    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_FIFO_EN, 0x00);
    MPU6050_RA_I2C_MST_CTRL,      //AUX I2C setup    //Sets AUX I2C to single master control, plus other config    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_MST_CTRL, 0x00);
    MPU6050_RA_I2C_SLV0_ADDR,    //Setup AUX I2C slaves    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_ADDR, 0x00);
    MPU6050_RA_I2C_SLV0_REG,    //LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_REG, 0x00);
    MPU6050_RA_I2C_SLV0_CTRL,   //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_CTRL, 0x00);
    MPU6050_RA_I2C_SLV1_ADDR,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_ADDR, 0x00);
    MPU6050_RA_I2C_SLV1_REG,    //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_REG, 0x00);
    MPU6050_RA_I2C_SLV1_CTRL,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_CTRL, 0x00);
    MPU6050_RA_I2C_SLV2_ADDR,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_ADDR, 0x00);
    MPU6050_RA_I2C_SLV2_REG,   //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_REG, 0x00);
    MPU6050_RA_I2C_SLV2_CTRL,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_CTRL, 0x00);
    MPU6050_RA_I2C_SLV3_ADDR,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_ADDR, 0x00);
    MPU6050_RA_I2C_SLV3_REG,   //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_REG, 0x00);
    MPU6050_RA_I2C_SLV3_CTRL, //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_CTRL, 0x00);
    MPU6050_RA_I2C_SLV4_ADDR, //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_ADDR, 0x00);
    MPU6050_RA_I2C_SLV4_REG,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_REG, 0x00);
    MPU6050_RA_I2C_SLV4_DO,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_DO, 0x00);
    MPU6050_RA_I2C_SLV4_CTRL, //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_CTRL, 0x00);
    MPU6050_RA_I2C_SLV4_DI,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_DI, 0x00);
    MPU6050_RA_I2C_SLV0_DO,  //    //Slave out, dont care    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_DO, 0x00);
    MPU6050_RA_I2C_SLV1_DO,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_DO, 0x00);
    MPU6050_RA_I2C_SLV2_DO,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_DO, 0x00);
    MPU6050_RA_I2C_SLV3_DO,  //    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_DO, 0x00);
    MPU6050_RA_I2C_MST_DELAY_CTRL,  //More slave config    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_MST_DELAY_CTRL, 0x00);
    MPU6050_RA_SIGNAL_PATH_RESET,      //Reset sensor signal paths    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_SIGNAL_PATH_RESET, 0x00);
    MPU6050_RA_MOT_DETECT_CTRL,    //Motion detection control    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_MOT_DETECT_CTRL, 0x00);
    MPU6050_RA_USER_CTRL,    //Disables FIFO, AUX I2C, FIFO and I2C reset bits to 0    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_USER_CTRL, 0x00);
    MPU6050_RA_INT_PIN_CFG,    //MPU6050_RA_I2C_MST_STATUS //Read-only    //Setup INT pin and AUX I2C pass through    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x00);
    MPU6050_RA_INT_ENABLE,    //Enable data ready interrupt    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_INT_ENABLE, 0x00);
    MPU6050_RA_FIFO_R_W,       // LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_FIFO_R_W, 0x00);
    0xff
};   



//    //Sets sample rate to 8000/1+7 = 1000Hz
//    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_SMPLRT_DIV, 0x07);
    //Sets sample rate to 8000/1+15 = 500Hz
    I2CWrapperWriteByte(handle,MPU6050_RA_SMPLRT_DIV, 15);
    //Disable gyro self tests, scale of 500 degrees/s
    I2CWrapperWriteByte(handle, MPU6050_RA_GYRO_CONFIG, 0b00001000);
    //Disable accel self tests, scale of +-16g, no DHPF
    I2CWrapperWriteByte(handle, MPU6050_RA_ACCEL_CONFIG, 0x18);

    loop=0;
   do
   {
       TheReg = MPU6050RegTable[loop++];
       if(TheReg==0xff) break;
       I2CWrapperWriteByte(handle,TheReg,0);
    }while(1);



    //Sets clock source to gyro reference w/ PLL
    I2CWrapperWriteByte(handle, MPU6050_RA_PWR_MGMT_1, 0b00000010);
    //Controls frequency of wakeups in accel low power mode plus the sensor standby modes
    I2CWrapperWriteByte(handle, MPU6050_RA_PWR_MGMT_2, 0x00);
 
    //MPU6050_RA_WHO_AM_I             //Read-only, I2C address
    I2CWrapperWriteByte(handle, MPU6050_RA_INT_ENABLE, 0x01);
  printf("\r\nMPU6050 Setup Complete\r\n");

}




void Get_Accel_Values(int handle, GForceStruct * GData)
{
char cval[14];
 I2CWrapperReadBlock(handle, MPU6050_RA_ACCEL_XOUT_H,14 ,  cval);
	GData->Gx = ((cval[0]<<8)|cval[1]);
	GData->Gy = ((cval[2]<<8)|cval[3]);
	GData->Gz = ((cval[4]<<8)|cval[5]);
	GData->Temperature = ((cval[6]<<8)|cval[7]);
	GData->Gyrox = ((cval[8]<<8)|cval[9]);
	GData->Gyroy = ((cval[10]<<8)|cval[11]);
	GData->Gyroz = ((cval[12]<<8)|cval[13]);
        
}




unsigned char GotInt_MPU6050(int handle)
{
  unsigned char uc_temp;

// Do we have a new data

	uc_temp= I2CWrapperReadByte(handle,MPU6050_RA_INT_STATUS);

  return ((uc_temp & 1) == 1 ? 1 : 0);
}	




