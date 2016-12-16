#!/usr/bin/python

import serial.tools.list_ports
#ports = list(serial.tools.list_ports.comports())
#for p in ports:
#    print p
ftdi_ports = [
    p.device
    for p in serial.tools.list_ports.comports()
    if 'FT232R' in p.description
]
if not ftdi_ports:
    raise IOError("No FTDI found")
if len(ftdi_ports) > 1:
    warnings.warn('Multiple FTDI found, using first')
    
#ser = serial.Serial(ftdi_ports[0]);
print ftdi_ports[0]
