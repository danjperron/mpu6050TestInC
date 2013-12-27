#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "I2CWrapper.h"
#include "mpu6050.h"
#include  <math.h>
////////////////////////////////////////////
//
//    program to test mpu6050 accelerometer
//    on raspberry pi I2C bus
//    to compile
//    
//     gcc -o AccTest  AccTest.c mpu6050.c I2CWrapper.c
//
//
//   programmer : Daniel Perron
//   date       : Dec 26 , 2013
//

/*

The MIT License (MIT)

Copyright (c)  2013 Daniel Perron

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
*/


GForceStruct   Data;

int ExitFlag=0;

void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT\n");
   ExitFlag=1;
}


int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}






int main(void)
{
   int loop;
   int i2c_handle;
   float Gtotal;
   float GSumSquare;
   short GyroxOffset,GyroyOffset,GyrozOffset;
   long  Xsum,Ysum,Zsum;
   int CountSum;
   const float  AccFactor=16.0 /32768.0;
   const float  GyroFactor=500.0 / 32768.0;
   const BUS = 1;
   int I2C_Current_Slave_Adress=0x68;


    signal(SIGINT, sig_handler);



    i2c_handle = I2CWrapperOpen(BUS,I2C_Current_Slave_Adress);
	if(i2c_handle <0) return -1;


    if(! MPU6050_Test_I2C(i2c_handle))
       {
         printf("Unable to detect MPU6050\n");
       }
       else
      {

           Setup_MPU6050(i2c_handle);

           printf("Please be sure that the MPU6050 doesn't move and press space bar\n");
           fflush(stdout);

           while(!kbhit())
             Get_Accel_Values(i2c_handle,&Data);
           printf("calibrating Gyro..\n");
           fflush(stdout);

           Xsum=Ysum=Zsum=CountSum=0;
           for(loop=0;loop<200;loop++)
           {
             while(!GotInt_MPU6050(i2c_handle));
             Get_Accel_Values(i2c_handle,&Data);
             Xsum += Data.Gyrox;
             Ysum += Data.Gyroy;
             Zsum += Data.Gyroz;
             CountSum++;
            }
           GyroxOffset = Xsum / CountSum;
           GyroyOffset = Ysum / CountSum;
           GyrozOffset = Zsum / CountSum;

           while(!ExitFlag)
           {
           usleep(100000); // wait a little

           Get_Accel_Values(i2c_handle,&Data);
           GSumSquare = ((float) Data.Gx) * Data.Gx;
           GSumSquare += ((float) Data.Gy) * Data.Gy;
           GSumSquare += ((float) Data.Gz) * Data.Gz;
           Gtotal = sqrt(GSumSquare);

           printf("Accelerometer x=%+6.2f y=%+6.2f z=%+6.2f All=%5.2f Temp=%+4.1f Gyro x=%+7.2f y=%+7.2f z=%+7.2f\n",\
           //acceleration are in 20G scale
           AccFactor *  Data.Gx,
           AccFactor *  Data.Gy,
           AccFactor *  Data.Gz,
           AccFactor *  Gtotal,
           // temperature are
           //Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53

           (float)  Data.Temperature / 340.0 + 36.53,
           GyroFactor * (Data.Gyrox - GyroxOffset),
           GyroFactor * (Data.Gyroy - GyroyOffset),
           GyroFactor * (Data.Gyroz - GyrozOffset));
           fflush(stdout);
          }
      }

   close(i2c_handle);
   printf("Program close\n");
return 0;
}
