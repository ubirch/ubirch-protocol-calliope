#! /usr/bin/env python3
"""
This little script waits for TIME: and then sets the time and waits for lines starting with 95, which is
the first byte of the message-pack encoded packages.
"""
import sys
from datetime import datetime

import serial

with serial.Serial(port=sys.argv[1], baudrate=sys.argv[2]) as ser:
    while ser.readable():
        line = bytes.decode(ser.readline()).replace('\r\n', '')
        if line.startswith('TIME:'):
            seconds = "{}\r\n".format(int(datetime.utcnow().timestamp()))
            ser.write(str.encode(seconds))
        elif line.startswith('95'):
            print(line, end="")
            print(repr(bytes.fromhex(line)))