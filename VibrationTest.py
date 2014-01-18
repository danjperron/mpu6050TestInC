#!/usr/bin/python

import MPU6050
import math

mpu6050 = MPU6050.MPU6050()

mpu6050.Setup()
#sometimes I need to reset twice
I = mpu6050.ReadData()
mpu6050.Setup()



Threshold = 0.2
ShakeFlag = False;

while True:

  #wait until new data available
  while not(mpu6050.GotNewData()):
    pass

  #read initial reading and put in I class
  I = mpu6050.ReadData()


  PeakForce = 0

  for loop in range(20):

     #wait until data is available
     while not(mpu6050.GotNewData()):
       pass
 
     # read the accelerometer
     C = mpu6050.ReadData()

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
     
