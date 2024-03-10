#!/usr/bin/env python

### TO USE:
#### Press Y on the HBMenu (opens the NetLoader)
#### Change host/port combination
#### Run the script

import socket
import sys
import time

TCP_IP = '192.168.xx.xx'
TCP_PORT = 9000
MESSAGE = open("hwtests.3dsx", "rb").read();

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
s.send(MESSAGE)
time.sleep(10)
s.close()

