#!/usr/bin/python
import serial
import time

#The following line is for serial over GPIO
port = 'COM9'


ard = serial.Serial(port,9600,timeout=1)

message = 'sos'.encode("utf-8")

time.sleep(5)
print(ard.is_open)
ard.write(message)
time.sleep(1)
print(ard.out_waiting)
readMessage = ard.readline()
utf8Message = readMessage.decode("utf-8")
while (utf8Message[0:2] != "OK"):
    time.sleep(1)
    readMessage = ard.readline()
    utf8Message = readMessage.decode("utf-8")
