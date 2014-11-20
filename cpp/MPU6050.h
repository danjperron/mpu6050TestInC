#include "MPU6050_Register.h"
#include "I2CWrapper.h"

//#define  USE_TEMPERATURE
//#define  USE_GYRO

typedef struct
{
  short Gx,Gy,Gz;
#ifdef USE_TEMPERATURE
  short Temperature;
#endif

#ifdef USE_GYRO
  short Gyrox,Gyroy,Gyroz;
#endif

}__attribute__((packed)) GForceStruct;




class MPU6050 {

public:

       I2CWrapper i2c;
       int FailedFlag;

       float Gx,Gy,Gz;
       int SampleRate;
#ifdef  USE_TEMPERATURE
       float Temperature;
static const  float TemperatureGain;
static const  float TemperatureOffset;
#endif

#ifdef USE_GYRO
      float Gyrox,Gyroy,Gyroz;
      float GyroFactor;
#endif

      float AccelerationFactor;
      int i2c_handle;

MPU6050();
MPU6050(int Channel,int Address);
~MPU6050();

void setup(void);

static const unsigned char  ZeroRegister[];

unsigned char readStatus(void);


GForceStruct *  readData(GForceStruct *, int Count);

void setGResolution(int value);
void setSampleRate(int value);
short readFifoCount(void);
int readFifo(unsigned char * Fifo,int Count);
void resetFifo(void);
void enableFifo(bool flag);
short SWAPBYTE(short value);


private:
  int Channel;
  int Address;

};
