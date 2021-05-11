# -*- coding: utf-8 -*-
"""
Created on Tue Apr 27 14:41:41 2021

@author: jgamm
"""


import serial

def setChannel(ser, channel):
    ser.write(b'CH\n')
    ser.readline()
    ser.write((str(channel&0x1F)+'\n').encode('ascii'))

def startTransmit(ser):
    ser.write(b'TX\n')
    ser.readline()

def endTransmit(ser):
    vals = []
    while len(vals)==0 or vals[-1]!='\n':
        while not(ser.in_waiting):
            pass
        vals.append(ser.read().decode())
    m = ''.join(vals)
    print(m)
    address = int(m.split(' ')[2][:-1], 16)
    time    = int(m.split(' ')[4])
    return {'address': address, 'time': time}

def startReceive(ser):
    ser.write(b'RX\n')
    ser.readline()

def endReceive(ser):
    vals = []
    while len(vals)==0 or vals[-1]!='\n':
        while not(ser.in_waiting):
            pass
        vals.append(ser.read().decode())
    m = ''.join(vals)
    print(m)
    if not('invalid RX' in m):
        vals = []
        mm = ''
        while not('done' in mm):
            mm = ser.readline().decode()
            try:
                vals.append(int(mm[:-1], 16))
            except:
                pass
        print(vals)
        length  = int(m.split(' ')[3][:-1])
        address = int(m.split(' ')[5][:-1], 16)
        time    = int(m.split(' ')[7])
        return vals, {'length': length, 'address': address, 'time': time}
    else:
        return None, None

successes = 0
import numpy as np
for channel in np.concatenate((np.arange(0, 0x20), np.arange(0x1F, -1, -1))):
    while successes < 5:
        try:
            RX = serial.Serial('COM36', 115200)
            TX = serial.Serial('COM33', 115200)
            
            setChannel(RX, channel)
            setChannel(TX, channel)
            
            startReceive(RX)
            startTransmit(TX)
            tx_info = endTransmit(TX)
            vals, rx_info = endReceive(RX)
            if vals==None:
                continue
            
            from matplotlib import pyplot as plt
            import numpy as np
            from scipy.stats import linregress
            
            vals = np.unwrap(((2*np.pi/256)*np.array(vals))-np.pi)
            m, b, r, _, _ = linregress(range(len(vals)), vals)
            
            print('Slope: %f, Intercept: %f, Correlation coefficient: %f'%(m, b, r))
            
            plt.plot(np.linspace(0, rx_info['time'], len(vals)), np.unwrap(vals), marker='.', color='blue', label='Data')
            plt.plot(np.linspace(0, rx_info['time'], len(vals)), m*np.arange(len(vals))+b, '--', color='red', label='Regression line')
            plt.legend()
            plt.xlabel('Time (us)')
            plt.ylabel('Phase (radians)')
            
            if r**2 >= .99:
                print('Saving data...')
                successes += 1
                import pickle
                import os
                import datetime
                Data = {'channel': channel, 'rx info': rx_info, 'tx info': tx_info, 'values': vals, 'regression line': {'m': m, 'b': b, 'r': r}}
                dt = datetime.datetime.now()
                folder_name = 'data__%d_%d_%d__%d_%d_%d_%d'%(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, dt.microsecond)
                folder_path = os.path.join(os.getcwd(), 'data', folder_name)
                os.mkdir(folder_path)
                with open(os.path.join(folder_path, 'Data.pickle'), 'wb') as F:
                    pickle.dump(Data, F)
                plt.savefig(os.path.join(folder_path, 'plot.png'))
            plt.close()
            
        finally:
            RX.close()
            TX.close()
    successes = 0

if False:
    channels = []
    slopes = []
    intercepts = []
    for folder in os.listdir(os.path.join(os.getcwd(), 'data')):
        path = os.path.join(os.getcwd(), 'data', folder)
        with open(os.path.join(path, 'Data.pickle'), 'rb') as F:
            Data = pickle.load(F)
        channels.append(Data['channel'])
        slopes.append(Data['regression line']['m'])
        intercepts.append(Data['regression line']['b'])
    fig, ax = plt.subplots(1, 1)
    ax.plot(channels, slopes, '.', color='blue', label='slopes')
    ax.set_ylabel('slopes')
    ax.set_xlabel('channel')
    axt = ax.twinx()
    axt.set_ylabel('intercepts')
    axt.plot(channels, intercepts, '.', color='red', label='intercepts')
    ax.legend()
    axt.legend()