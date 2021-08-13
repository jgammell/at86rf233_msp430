# -*- coding: utf-8 -*-
"""
Created on Tue Apr 27 14:41:41 2021

@author: jgamm
"""

# This file contains code to interact with the AT86RF233 evaluation boards through the MSP-EXP430F5529LP.
# Currently, the code will exchange packets repeatedly between AT86RF233 boards and retreive phase measurements taken during each reception.
# Each set of phase data is unwrapped and a line is fit to it using linear regression. Lines that fit with sufficiently-large coefficient of determination are saved.
# This is repeated on each channel at which the AT86RF233 can transmit until 5 regression lines are recorded for each channel.
# The slopes and intercepts are then plotted with respect to channel.
# Plots and raw data for each payload reception are saved in folders corresponding to date and time inside the data folder as 'Data.pickle' and 'plot.png'. Plot of slopes and intercepts is in python_files folder saved as 'plot.png'.

# This experiment has not yielded interesting data so far, likely because phase transitions must be handled by a microcontroller for sufficiently-precise timing.
# Nonetheless, this file can serve as an example for how to control the AT86RF233 through a Python script.

# The below COM values should be changed to reflect the MSP-EXP430F5529LP COM ports that show up in the device manager.

RX_COM = 'COM36'
TX_COM = 'COM33'

import serial
from matplotlib import pyplot as plt
import numpy as np
from scipy.stats import linregress
import numpy as np
import pickle
import os
import datetime

def setChannel(ser, channel): # Configure an AT86RF233 to transmit and receive on a specific channel
    ser.write(b'CH\n') # Send set channel command
    ser.readline() # Wait for acknowledgement
    ser.write((str(channel&0x1F)+'\n').encode('ascii')) # Specify channel on which to transmit/receive

def startTransmit(ser): # Tell an AT86RF233 to start transmission
    ser.write(b'TX\n') # Sent start transmit command
    ser.readline() # Wait for acknowledgement

def endTransmit(ser): # Retrieve data after an AT86RF233 has finished transmitting
    vals = [] # Contains characters received from the VCOM port
    while len(vals)==0 or vals[-1]!='\n': # Record characters until we receive '\n', indicating end of command
        while not(ser.in_waiting):
            pass
        vals.append(ser.read().decode())
    m = ''.join(vals) # Join characters into string
    print(m)
    address = int(m.split(' ')[2][:-1], 16) # Extract address value from string
    time    = int(m.split(' ')[4]) # Extract transmit duration from string
    return {'address': address, 'time': time} # Return these parsed values

def startReceive(ser): # Tell an AT86RF233 to start receiving
    ser.write(b'RX\n') # Send start receive command
    ser.readline() # Wait for acknowledgement

def endReceive(ser): # Wait until reception is complete, then retrieve data about the reception
    vals = [] # Contains values received from the VCOM port
    while len(vals)==0 or vals[-1]!='\n': # Record characters until we receive '\n', indicating end of command
        while not(ser.in_waiting):
            pass
        vals.append(ser.read().decode())
    m = ''.join(vals) # Join characters into string
    print(m)
    if not('invalid RX' in m): # Sometimes the AT86RF233 receives erroneous packets; proceed only if packet is valid
        vals = []
        mm = ''
        while not('done' in mm): # Record phase values until we get command indicating all have been transmitted
            mm = ser.readline().decode()
            try:
                vals.append(int(mm[:-1], 16))
            except:
                pass
        print(vals)
        length  = int(m.split(' ')[3][:-1]) # Extract payload length
        address = int(m.split(' ')[5][:-1], 16) # Extract address
        time    = int(m.split(' ')[7]) # Extract time
        return vals, {'length': length, 'address': address, 'time': time} # Return parsed data
    else:
        return None, None # If payload was erroneous, return nothing


successes = 0
for channel in np.concatenate((np.arange(0, 0x20), np.arange(0x1F, -1, -1))): # Attempt for each channel that the AT86RF233 can transmit on
    while successes < 5:
        try: # Sometimes errors happen. If this is the case, close the serial connections and retry on the next loop.
            RX = serial.Serial(RX_COM, 115200) # Connect to AT86RF233 we will designate receiver over VCOM port
            TX = serial.Serial(TX_COM, 115200) # Connect to AT86RF233 we will designate transmitter over VCOM port
            
            setChannel(RX, channel) # Set both radios to the current channel
            setChannel(TX, channel)
            
            startReceive(RX) # Tell receiver to start listening for payload
            startTransmit(TX) # Start transmission of payload
            tx_info = endTransmit(TX) # Wait for end of payload transmission and record information about it
            vals, rx_info = endReceive(RX) # Wait for end of reception and record phase measurements and reception information
            if vals==None: # If the packet was erroneous, try again
                continue
           
             
            vals = np.unwrap(((2*np.pi/256)*np.array(vals))-np.pi) # Convert phase data to radians and unwrap it
            m, b, r, _, _ = linregress(range(len(vals)), vals) # Compute LSRL for unwrapped phase data
            
            print('Slope: %f, Intercept: %f, Correlation coefficient: %f'%(m, b, r))
            
            # Save plot of phase data
            plt.plot(np.linspace(0, rx_info['time'], len(vals)), np.unwrap(vals), marker='.', color='blue', label='Data')
            plt.plot(np.linspace(0, rx_info['time'], len(vals)), m*np.arange(len(vals))+b, '--', color='red', label='Regression line')
            plt.legend()
            plt.xlabel('Time (us)')
            plt.ylabel('Phase (radians)')
            
            # Save phase data provided it was sufficiently linear. If not, do not save and try again.
            if r**2 >= .99:
                print('Saving data...')
                successes += 1
                Data = {'channel': channel, 'rx info': rx_info, 'tx info': tx_info, 'values': vals, 'regression line': {'m': m, 'b': b, 'r': r}}
                dt = datetime.datetime.now()
                folder_name = 'data__%d_%d_%d__%d_%d_%d_%d'%(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, dt.microsecond)
                folder_path = os.path.join(os.getcwd(), 'data', folder_name)
                os.mkdir(folder_path)
                with open(os.path.join(folder_path, 'Data.pickle'), 'wb') as F:
                    pickle.dump(Data, F)
                plt.savefig(os.path.join(folder_path, 'plot.png'))
            plt.close()
            
        finally: # If errors happen, close VCOM ports and try again
            RX.close()
            TX.close()
    successes = 0

if True: # Toggle whether or not to generate plot of LSRL data over all channels -- takes a while
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
    fig.savefig(os.path.join(os.getcwd(), 'plot.png'))