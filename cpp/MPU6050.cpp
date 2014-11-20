#include  <iostream>
#include  <cmath>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
extern "C" {
#include "myGpuFFT_lib.h"
}
#include "MPU6050.h"


using namespace std;

#ifdef USE_TEMPERATURE
const  float MPU6050::TemperatureGain= 1.0 / 340.0;
const  float MPU6050::TemperatureOffset= 36.53;
#endif


MPU6050::MPU6050()
{
    this->Channel=1;
    this->Address= 0x68;
    setup();
}


MPU6050::MPU6050(int Channel,int Address)
{
   this->Channel= Channel;
   this->Address= Address;
   setup();
}


void MPU6050::setup(void)
{
 unsigned char TheReg;
 int loop;

  if(i2c.Open(Channel,Address)>=0)
    {
      setGResolution(8);
      // set gyro
      i2c.WriteByte(MPU6050_RA_GYRO_CONFIG, 0b00001000);

      // zero register

      for(loop=0;;loop++)
        {
          TheReg=ZeroRegister[loop];
          if(TheReg == 0xff)
            break;
          i2c.WriteByte(TheReg,0);
          usleep(1000);
        }

    //Sets clock source to gyro reference w/ PLL
    i2c.WriteByte(MPU6050_RA_PWR_MGMT_1, 0b00000010);
    //Controls frequency of wakeups in accel low power mode plus the sensor standby modes
    i2c.WriteByte(MPU6050_RA_PWR_MGMT_2, 0x00);

    //MPU6050_RA_WHO_AM_I             //Read-only, I2C address
    i2c.WriteByte(MPU6050_RA_INT_ENABLE, 0x01);
    FailedFlag=0;

     }
  else
    FailedFlag=1;

}


MPU6050::~MPU6050()
{
  i2c.Close();
}


void MPU6050::setGResolution(int value)
{
  int G;

  switch(value)
   {

    case 2 : G=0;break;
    case 4 : G=8;break;
    case 8 : G=16;break;
    default : G=24;break;
   }
   i2c.WriteByte(MPU6050_RA_ACCEL_CONFIG,G);
   AccelerationFactor = (float) value / 32768.0;
}

unsigned char  MPU6050::readStatus(void)
{
   return i2c.ReadByte(MPU6050_RA_INT_STATUS);
}




GForceStruct *  MPU6050::readData(GForceStruct * gdata, int Count)
{
     return gdata;
}

void MPU6050::setSampleRate(int value)
{
   unsigned char  SampleReg = (8000/ value) -1;
   i2c.WriteByte(MPU6050_RA_SMPLRT_DIV,SampleReg);

}


short MPU6050::readFifoCount(void)
{
   union bword {
    unsigned short word;
    unsigned char  byte[0];
   }invword;

  invword.word = i2c.ReadWord(MPU6050_RA_FIFO_COUNTH);

  // need to swap the bytes big endian to little one
  return ((unsigned short) invword.byte[0]* 256 + (unsigned short) invword.byte[1]);

}


int  MPU6050::readFifo(unsigned char * Fifo,int Count)
{
  return i2c.ReadBlock(MPU6050_RA_FIFO_R_W ,Count, Fifo);
}


void MPU6050::resetFifo(void)
{
  i2c.WriteByte(MPU6050_RA_USER_CTRL,0);
  usleep(500);
  i2c.WriteByte(MPU6050_RA_USER_CTRL,0x4);
  usleep(500);
  i2c.WriteByte(MPU6050_RA_USER_CTRL,0x40);
}


void MPU6050::enableFifo(bool flag)
{
 unsigned char Mask = 8;

#ifdef USE_TEMPERATURE
   Mask  |=  0x80;
#endif

#ifdef USE_GYRO

  Mask   |=  0x70;
#endif

  if(flag)
    i2c.WriteByte(MPU6050_RA_FIFO_EN,Mask);
  else
    i2c.WriteByte(MPU6050_RA_FIFO_EN,0);
};

short  MPU6050::SWAPBYTE(short V)
{
// mpu6050 use big endian
// we need to swap the bytes
unsigned char temp;

union  Data {
  short s;
  unsigned char c[2];
} data;

  data.s = V;

  temp = data.c[0];
  data.c[0]= data.c[1];
  data.c[1]=temp;

  return data.s;

}

int main(void)
{
  int i,loop1;
  int Count;
  unsigned char  Status;

  bool ExitFlag= false;
  int MaxCount =   (32 /  sizeof(GForceStruct));

//  GForceStruct FTable[1024];
//  float InTable[1024];
//  float OutTable[1024];
//  int mbox_h =   OpenMyGpuFFT(10); // prepare gpu fft with 1024 data points



// use 256 points instead of 1024.   1/(1000ms * 256)= 3.9  times per seconds

  GForceStruct FTable[256];
  float InTable[256];
  float OutTable[256];

  int mbox_h =   OpenMyGpuFFT(8); // prepare gpu fft with 256  points

  /*
    // testing FFT routine
    // offset of 1 should be in OutTable[0];
    // Amplitude  of 1 should be in OutTable[5]

   float mpi = 2.0 * 3.141592654 / 1024.0;
   for(i=0;i<NumberOfDataPoint;i++)
    InTable[i]= 1.0 + sin(mpi * 5 * i);

   DoMyGpuFFT(InTable,OutTable,1);

   for(i=0;i<10;i++)
    cout << i << " : " << OutTable[i] << endl;

   CloseMyGpuFFT();

   return 0;


 */








   MPU6050 * mpu = new MPU6050();

   mpu->enableFifo(false);
   mpu->setSampleRate(1000);
   mpu->resetFifo();
   mpu->readStatus();
   mpu->enableFifo(true);


   cout << "Ready" << endl;

   for(loop1=0;;loop1++)
{
       Count=0;
        while(Count<NumberOfDataPoint)
         {



           Status = mpu->readStatus();

           if(Status & 0x10)
              {
                cout << "Overrun Error! Quitting Count=" << Count<< "Fifo count " <<  mpu->readFifoCount() <<endl;
                ExitFlag=true;
                break;
              }
           if(Status & 1)
             {
               int Dcount;

               int FifoCount = mpu-> readFifoCount();

               if(FifoCount==0) continue;
               FifoCount /= sizeof(GForceStruct);
               int TCount =  Count + FifoCount;
               if(TCount >NumberOfDataPoint)
                   Dcount = NumberOfDataPoint - Count;
                 else
                   Dcount = FifoCount;

               while(Dcount>0)
                {
                   int rec=Dcount;
                   if (Dcount > MaxCount)
                      rec= MaxCount;
                    int t = mpu->readFifo((unsigned char *) &FTable[Count],rec * sizeof(GForceStruct));
                   Dcount -= rec;
                   Count +=  rec;
                 }
              }
         }

         for(i=0;i<NumberOfDataPoint;i++)
           {
              float Gx,Gy,Gz,GT;

              Gx = mpu->AccelerationFactor * (mpu->SWAPBYTE(FTable[i].Gx));
              Gy = mpu->AccelerationFactor * (mpu->SWAPBYTE(FTable[i].Gy));
              Gz = mpu->AccelerationFactor * (mpu->SWAPBYTE(FTable[i].Gz));


              GT=  sqrt(Gx * Gx + Gy * Gy + Gz * Gz);
              InTable[i]=  GT;
//           cout << Gx << "\t" << Gy << "\t" << Gz << "\t" << GT << endl;
           }


         DoMyGpuFFT(InTable,OutTable,1);

          int idx;
          int MaxIdx;
          float MaxPeak=0;
          for(idx=1;idx<= (NumberOfDataPoint/2); idx ++)
             if(OutTable[idx] > MaxPeak)
               {
                 MaxPeak= OutTable[idx];
                MaxIdx=idx;
               }

        cout << "Peak at [" << MaxIdx << "] : " << MaxIdx * (1000.0/1024.0) << "Hz" ;
        cout << "Amplitude =" <<  OutTable[MaxIdx] << endl;




        if(ExitFlag) break;
      }

   CloseMyGpuFFT();
   delete mpu;
  return  0;
}


const unsigned char  MPU6050::ZeroRegister[]= {
      MPU6050_RA_FF_THR, MPU6050_RA_FF_DUR, MPU6050_RA_MOT_THR, MPU6050_RA_MOT_DUR, MPU6050_RA_ZRMOT_THR,
      MPU6050_RA_ZRMOT_DUR, MPU6050_RA_FIFO_EN, MPU6050_RA_I2C_MST_CTRL, MPU6050_RA_I2C_SLV0_ADDR, MPU6050_RA_I2C_SLV0_REG,
      MPU6050_RA_I2C_SLV0_CTRL, MPU6050_RA_I2C_SLV1_ADDR, MPU6050_RA_I2C_SLV1_REG, MPU6050_RA_I2C_SLV1_CTRL,
      MPU6050_RA_I2C_SLV2_ADDR,	MPU6050_RA_I2C_SLV2_REG, MPU6050_RA_I2C_SLV2_CTRL, MPU6050_RA_I2C_SLV3_ADDR, MPU6050_RA_I2C_SLV3_REG,
      MPU6050_RA_I2C_SLV3_CTRL,	MPU6050_RA_I2C_SLV4_ADDR, MPU6050_RA_I2C_SLV4_REG, MPU6050_RA_I2C_SLV4_DO,
      MPU6050_RA_I2C_SLV4_CTRL,	MPU6050_RA_I2C_SLV4_DI, MPU6050_RA_INT_PIN_CFG, MPU6050_RA_INT_ENABLE, MPU6050_RA_I2C_SLV0_DO,
      MPU6050_RA_I2C_SLV1_DO, MPU6050_RA_I2C_SLV2_DO, MPU6050_RA_I2C_SLV3_DO, MPU6050_RA_I2C_MST_DELAY_CTRL,
      MPU6050_RA_SIGNAL_PATH_RESET, MPU6050_RA_MOT_DETECT_CTRL, MPU6050_RA_USER_CTRL, MPU6050_RA_CONFIG,
      MPU6050_RA_FF_THR, MPU6050_RA_FF_DUR, MPU6050_RA_MOT_THR, MPU6050_RA_MOT_DUR,
      MPU6050_RA_ZRMOT_THR, MPU6050_RA_ZRMOT_DUR, MPU6050_RA_FIFO_EN, MPU6050_RA_I2C_MST_CTRL,
      MPU6050_RA_I2C_SLV0_ADDR, MPU6050_RA_I2C_SLV0_REG, MPU6050_RA_I2C_SLV0_CTRL, MPU6050_RA_I2C_SLV1_ADDR,
      MPU6050_RA_I2C_SLV1_REG, MPU6050_RA_I2C_SLV1_CTRL, MPU6050_RA_I2C_SLV2_ADDR,
      MPU6050_RA_I2C_SLV2_REG, MPU6050_RA_I2C_SLV2_CTRL, MPU6050_RA_I2C_SLV3_ADDR,
      MPU6050_RA_I2C_SLV3_REG, MPU6050_RA_I2C_SLV3_CTRL, MPU6050_RA_I2C_SLV4_ADDR,
      MPU6050_RA_I2C_SLV4_REG, MPU6050_RA_I2C_SLV4_DO, MPU6050_RA_I2C_SLV4_CTRL,
      MPU6050_RA_I2C_SLV4_DI, MPU6050_RA_I2C_SLV0_DO, MPU6050_RA_I2C_SLV1_DO, MPU6050_RA_I2C_SLV2_DO,
      MPU6050_RA_I2C_SLV3_DO, MPU6050_RA_I2C_MST_DELAY_CTRL, MPU6050_RA_SIGNAL_PATH_RESET,
      MPU6050_RA_MOT_DETECT_CTRL, MPU6050_RA_USER_CTRL, MPU6050_RA_INT_PIN_CFG,
      MPU6050_RA_INT_ENABLE, MPU6050_RA_FIFO_R_W ,0xff};
