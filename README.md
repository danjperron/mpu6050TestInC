mpu6050TestInC
==============

simple raspberry pi  program in C to test the mpu6050


 just do gcc -lm -o AccTest AccTest.c mpu6050.c I2CWrapper.c
 
Python Version
- MPU6050.py&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Interface class for the MPU6050.
- VibrationTest.py&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Application to detect peak in acceleration.
- GetFFTmpu6050.py&nbsp;&nbsp;&nbsp;FFT analysis using the build-in fifo buffer.

C++ Version.  GPU FFT calculating all axis and the absolute vector in parallel.
- I2CWrapper.cpp&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Interface to communicate with the I2C protocol.
- I2CWrapper.h&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;I2CWrapper class declaration.
- MPU6050.cpp&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MPU6050 class interface for the MPU6050.
- MPU6050.h&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MPU6050 class declaration.
- myGpuFFT_lib.c&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;My own static library to encapsulate the GPU FFT.
- myGpuFFT_lib.h&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;header declaration.
 
&nbsp;&nbsp;&nbsp;To compile just do ,<br>
&nbsp;&nbsp;&nbsp;cd cpp<br>
&nbsp;&nbsp;&nbsp;make clean; make all

