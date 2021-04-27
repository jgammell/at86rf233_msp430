# -*- coding: utf-8 -*-
"""
Created on Tue Apr 27 14:41:41 2021

@author: jgamm
"""

import serial

def startTransmit(ser, payload):
    ser.write(b'TX\n')
    ser.readline()
    ser.write(('%d\n'%(payload&0xFF)).encode('ascii'))

def endTransmit(ser):
    vals = []
    while len(vals)==0 or vals[-1]!='\n':
        while not(ser.in_waiting):
            pass
        vals.append(ser.read().decode())
    print(''.join(vals))

def startReceive(ser):
    ser.write(b'RX\n')
    ser.readline()

def endReceive(ser):
    vals = []
    while len(vals)==0 or vals[-1]!='\n':
        while not(ser.in_waiting):
            pass
        vals.append(ser.read().decode())
    print(''.join(vals))

RX = serial.Serial('COM24', 115200)
TX = serial.Serial('COM33', 115200)

while True:
    startReceive(RX)
    startTransmit(TX, 0xFF)
    endTransmit(TX)
    endReceive(RX)