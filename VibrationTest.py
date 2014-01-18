#!/usr/bin/python

import MPU6050
import math

mpu6050 = MPU6050.MPU6050()

mpu6050.setup()
mpu6050.setSampleRate(100)
mpu6050.setGResolution(16)
#sometimes I need to reset twice
I = mpu6050.readData()



Threshold = 0.2
ShakeFlag = False;

while True:

  #wait until new data available
  while (mpu6050.readStatus() & 1)==0 :
    pass

  #read initial reading and put in I class
  I = mpu6050.readData()


  PeakForce = 0

  for loop in range(20):

     #wait until new data available
     while (mpu6050.readStatus() & 1)==0 :
       pass
 
     # read the accelerometer
     C = mpu6050.readData()

     #calculate new force
     CurrentForce = math.sqrt( (C.Gx - I.Gx) * ( C.Gx - I.Gx) + 
                               (C.Gy - I.Gy) * ( C.Gy - I.Gy) +
                               (C.Gz - I.Gz) * ( C.Gz - I.Gz))

     if CurrentForce > PeakForce :
        PeakForce = CurrentForce  

   
  if PeakForce > Threshold :
    if not(ShakeFlag) :
        ShakeFlag = True
        print "Vibration detected ", PeakForce, "G"
  else:
    ShakeFlag= False
     
