#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "I2CWrapper.h"
#include "mpu6050.h"

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

int main(void)
{
   int i2c_handle;

   const BUS = 1;
   int I2C_Current_Slave_Adress=0x69;

    i2c_handle = I2CWrapperOpen(BUS,I2C_Current_Slave_Adress);
	if(i2c_handle <0) return -1;


    if(! MPU6050_Test_I2C(i2c_handle))
       {
         printf("Unable to detect MPU6050\n");
       }
       else
      {
           
           Setup_MPU6050(i2c_handle);
           usleep(10000); // wait a little
           Get_Accel_Values(i2c_handle,&Data);
           printf("Gx=%3.1f Gy=%3.1f Gz=%3.1f  Temp=%3.1f\n",\
           //acceleration are in 20G scale
           (float) Data.Gx * 20.0 / 32767,
           (float) Data.Gy * 20.0 / 32767,
           (float) Data.Gz * 20.0 / 32767,
           // temperature are 
           //Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53

           (float)  Data.Temperature / 340.0 + 36.53);
      }

   close(i2c_handle);
return 0;
}
