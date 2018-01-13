#!/usr/bin/python

import MPU6050
import time

mpu6050 = MPU6050.MPU6050()

mpu6050.setup()
mpu6050.setSampleRate(50)
mpu6050.setGResolution(2)
i= mpu6050.readData()


try:
  c = 0
  while True:

      #wait for data available
      while (mpu6050.readStatus() & 1)==0:
          time.sleep(0.01)

      #read data
      i = mpu6050.readData()
      c = c + 1
      if c >= 50 :
         print('ACC x:{:6.3f} y:{:6.3f} z:{:6.3f}  GYRO x:{:.3f} y:{:.3f} z:{:.3f} Temp:{:.2f}'.format(i.Gx,i.Gy,i.Gz,i.Gyrox,i.Gyroy,i.Gyroz,i.Temperature))
         c = 0

except KeyboardInterrupt:
      raise

