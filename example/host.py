#! /usr/bin/env python3
"""
This little script waits for TIME: and then sets the time and waits for lines starting with 95, which is
the first byte of the message-pack encoded packages.
"""
import sys
from datetime import datetime

import msgpack
import serial

with serial.Serial(port=sys.argv[1], baudrate=sys.argv[2]) as ser:
    while ser.readable():
        line = bytes.decode(ser.readline()).replace('\r\n', '')
        if line.startswith('TIME:'):
            seconds = "{}\r\n".format(int(datetime.now().timestamp()))
            ser.write(str.encode(seconds))
        elif line.startswith('95') or line.startswith('96'):
            print(msgpack.unpackb(bytes.fromhex(line)))
        else:
            print(line)