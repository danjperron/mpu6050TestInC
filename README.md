mpu6050TestInC
==============

simple raspberry pi  program in C to test the mpu6050


 just do gcc -lm -o AccTest AccTest.c mpu6050.c I2CWrapper.c
 
Python Version
-MPU6050.py  Interface class for the MPU6050.
-VibrationTest.py Application to detect peak in acceleration.
-GetFFTmpu6050.py FFT analysis using the build-in fifo buffer.

C++ Version

GPU FFT version calculating all axis and the absolute vector in parallel.
- I2CWrapper.cpp  Interface to communicate with the I2C protocol.
- I2CWrapper.h I2CWrapper class declaration.
- MPU6050.cpp MPU6050 class interface for the MPU6050.
- MPU6050.h   MPU6050 class declaration.
- myGpuFFT_lib.c My own static library to encapsulate the GPU FFT.
- myGpuFFT_lib.h header declaration.
 
To compile just do
cd cpp
make clean; make all

