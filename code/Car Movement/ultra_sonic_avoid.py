#!/usr/bin/env python
'''
**********************************************************************
* Filename    : ultra_sonic_avoidance.py
* Description : An example for sensor car kit to followe light
* Author      : Dream
* Brand       : SunFounder
* E-mail      : service@sunfounder.com
* Website     : www.sunfounder.com
* Update      : Dream    2016-09-27    New release
**********************************************************************
'''

from SunFounder_Ultrasonic_Avoidance import Ultrasonic_Avoidance
from picar import front_wheels
from picar import back_wheels
import time
import picar
import random
import serial

force_turning = 0    # 0 = random direction, 1 = force left, 2 = force right, 3 = orderdly

picar.setup()

#Variables for serial.
serial_port = 9600
#Initialize serial
ser = serial.Serial('/dev/ttyACM1',serial_port)

ua = Ultrasonic_Avoidance.Ultrasonic_Avoidance(20) #Create an object ua of a UltraSonic_Avoidance class in the Ultra_Sonic module.
#20 - the pin number the SIG of the module is connected to.
fw = front_wheels.Front_Wheels(db='config')
bw = back_wheels.Back_Wheels(db='config')
fw.turning_max = 45 #Maximum value the servo can turn.

forward_speed = 70
backward_speed = 70

back_distance = 10 #Distance to go back.
turn_distance = 20 #Distance to turn.

timeout = 10
last_angle = 90 #Last angle the servo turned.
last_dir = 0 #Last direction it turned.

#Choose a random or forced angle to turn.
def rand_dir():
    global last_angle, last_dir
    if force_turning == 0:
        _dir = random.randint(0, 1)
    elif force_turning == 3:
        _dir = not last_dir
        last_dir = _dir
        print('last dir  %s' % last_dir)
    else:
        _dir = force_turning - 1
    angle = (90 - fw.turning_max) + (_dir * 2* fw.turning_max)
    last_angle = angle
    return angle

#Turn an angle opposite of the last one.
def opposite_angle():
    global last_angle
    if last_angle < 90:
        angle = last_angle + 2* fw.turning_max
    else:
        angle = last_angle - 2* fw.turning_max
    last_angle = angle
    return angle

def start_avoidance():
    print('start_avoidance')
    count = 0
    while True:  
        #If stop car sent from arduino - call stop() function and break from loop.
        #distance = ua.get_distance()
        #Reading serial data.
        if(ser.in_waiting > 0):
            line = str(ser.readline().decode('utf-8').rstrip())
            print("Serial Data:", line)
            #If line is stop.
            if(line == "stop"):
                print("movement halted")
                stop()
                break
        #If stop car sent from arduino - call stop() function and break from loop.
        distance = ua.get_distance()
        print("distance: %scm" % distance)
        if distance > 0:
            count = 0
            if distance < back_distance: # backward
                print( "backward")
                fw.turn(opposite_angle())
                bw.backward()
                bw.speed = backward_speed
                time.sleep(0.8)
                fw.turn(opposite_angle())
                bw.forward()
                time.sleep(0.8)
            elif distance < turn_distance: # turn
                print("turn")
                fw.turn(rand_dir())
                bw.forward()
                bw.speed = forward_speed
                time.sleep(0.8)
            else:
                fw.turn_straight()
                bw.forward()
                bw.speed = forward_speed
        else:                       # forward
                fw.turn_straight()
                if count > timeout:  # timeout, stop;
                    bw.stop()
                else:
                    bw.backward()
                    bw.speed = forward_speed
                    count += 1 
                
        

def stop():
    bw.stop()
    fw.turn_straight()

def start():
    ser.flushInput()
    #Enter while loop until keybaord interrupt or maybe some off switch?
    while True:
        if(ser.in_waiting > 0):
            line = str(ser.readline().decode('utf-8').rstrip())
            #Read from Serial - if start car is printed from Arduino - start the car loop.
            if(line == "start"):
                start_avoidance()
            else:
                print("idle")
        
if __name__ == '__main__':
    try:
        start()
    except KeyboardInterrupt:
        stop()
        ser.close()
