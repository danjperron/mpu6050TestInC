
#include  <iostream>
#include <iomanip>
#include  <cmath>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
extern "C" {
#include "myGpuFFT_lib.h"
}
#include "MPU6050.h"

#include <time.h>
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

unsigned long  Microseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec*1000000 + ts.tv_nsec/1000;
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
   SampleRate = value;
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



// select number of data  point for FFT   it is 2 power N
// FFT_NPOINT_LOG 8 is 256 data points
// FFT_NPOINT_LOG 10 is 1024 data points
#define  FFT_NPOINT_LOG   8

// define 4 FFT to be use GT,Gx,Gy and Gz
#define  FFT_JOBS      4


  GForceStruct FTable[1 << FFT_NPOINT_LOG];

  float GtIn[1 << FFT_NPOINT_LOG];
  float GxIn[1 << FFT_NPOINT_LOG];
  float GyIn[1 << FFT_NPOINT_LOG];
  float GzIn[1 << FFT_NPOINT_LOG];

  float * InTable[FFT_JOBS] = {GtIn,GxIn,GyIn,GzIn};

  float GtOut[1 << FFT_NPOINT_LOG];
  float GxOut[1 << FFT_NPOINT_LOG];
  float GyOut[1 << FFT_NPOINT_LOG];
  float GzOut[1 << FFT_NPOINT_LOG];

  float * OutTable[FFT_JOBS] = {GtOut,GxOut,GyOut,GzOut};


  int mbox_h =   OpenMyGpuFFT(FFT_NPOINT_LOG,FFT_JOBS); // prepare gpu fft with (2 power FFT_NPOINT_LOG) data points


   MPU6050 * mpu = new MPU6050();

   mpu->enableFifo(false);
   mpu->setSampleRate(1000);
   mpu->resetFifo();
   mpu->readStatus();
   mpu->enableFifo(true);


   cout << "GPU FFT with " << NumberOfDataPoint << " data points ready!" << endl;

   for(loop1=0;loop1<10;loop1++)
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



         float *  GtPt= GtIn;
         float *  GxPt= GxIn;
         float *  GyPt= GyIn;
         float *  GzPt= GzIn;
         GForceStruct * TablePt= FTable;



         for(i=0;i<NumberOfDataPoint;i++)
           {
              float Gx,Gy,Gz;
              *(GxPt++)=Gx = mpu->AccelerationFactor * (mpu->SWAPBYTE(TablePt->Gx));
              *(GyPt++)=Gy = mpu->AccelerationFactor * (mpu->SWAPBYTE(TablePt->Gy));
              *(GzPt++)=Gz = mpu->AccelerationFactor * (mpu->SWAPBYTE(TablePt->Gz));
              *(GtPt++)= sqrt(Gx * Gx + Gy * Gy + Gz * Gz);
              TablePt++;
           }

         unsigned long start,middle,end;
         start= Microseconds();
         middle = Microseconds();

         // this is the FFT 
         DoMyGpuFFT(InTable,OutTable,0);

         end=Microseconds();

         int idx;
         int MaxIdx;
         float MaxPeak=0;

         GtPt= &GtOut[1];
         GxPt= &GxOut[1];
         GyPt= &GyOut[1];
         GzPt= &GzOut[1];


         for(idx=1;idx<= (NumberOfDataPoint/2); idx ++)
          {

           // we need to chack all axis in case we have a circular vibration
           // circular vibration won't be notice on absolute vector
             if(*GtPt > MaxPeak)
               {
                 MaxPeak= *GtPt;
                 MaxIdx=idx;
               }
             if(*GxPt > MaxPeak)
               {
                 MaxPeak= *GxPt;
                 MaxIdx=idx;
               }
             if(*GyPt > MaxPeak)
               {
                 MaxPeak= *GyPt;
                 MaxIdx=idx;
               }
             if(*GzPt > MaxPeak)
               {
                 MaxPeak= *GzPt;
                 MaxIdx=idx;
               }

             GtPt++;
             GxPt++;
             GyPt++;
             GzPt++;

          }

        float FundamentalFrequency =  (float) mpu->SampleRate / (float) NumberOfDataPoint;
        cout.precision(03);
        cout << "Peak at [" << std::setw(3) <<  MaxIdx << "] : ";
        cout.precision(1);
        cout << std::setw(5) << std::fixed   << MaxIdx * FundamentalFrequency << "Hz" ;
        cout.precision(4);
        cout << " Amplitude  GT=" <<  sqrt(GtOut[MaxIdx])/(NumberOfDataPoint/2.0);
        cout << " Gx= " << sqrt(GxOut[MaxIdx])/(NumberOfDataPoint/2.0);
        cout << " Gy= " << sqrt(GyOut[MaxIdx])/(NumberOfDataPoint/2.0);
        cout << " Gz= " << sqrt(GzOut[MaxIdx])/(NumberOfDataPoint/2.0);
        cout << "G Average Force = " << sqrt(GtOut[0])/NumberOfDataPoint;
//        cout << " Gx=" << sqrt(GxOut[0])/NumberOfDataPoint;
//        cout << " Gy=" << sqrt(GyOut[0])/NumberOfDataPoint;
//        cout << " Gz=" << sqrt(GzOut[0])/NumberOfDataPoint;

        cout << "G FFT exec times=" << (end - middle) -(middle - start) << " us" << endl;

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
