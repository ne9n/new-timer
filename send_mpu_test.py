#!/usr/bin/env python3
"""
Simple sender for MPU test values over serial to the Arduino.
Sends TESTMODE ON, MPU X Y Z lines, then TESTMODE OFF.
Usage: python send_mpu_test.py --port COM3 --baud 115200 --file mpu_tests.json
"""
import serial
import time
import json
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--port', required=True)
parser.add_argument('--baud', default=115200, type=int)
parser.add_argument('--file', default=None, help='JSON test case file')
args = parser.parse_args()

# default cases (delay in seconds, x,y,z)
default_cases():
    return [
        (0.05, 0, 0, 0),
        (0.1, 5, 0, 0),
        (0.1, 10, 0, 0),
        (0.1, 20, 0, 0),
        (0.1, 40, 0, 0),
        (0.1, 0, 0, 0),
        (0.1, 0, 90, 0),
        (0.1, 0, 180, 0),
        (0.1, 0, 0, 200),
    ]


def load_cases(path):
    with open(path, 'r') as f:
        data = json.load(f)
    return [(c.get('delay', 0.1), c['x'], c['y'], c['z']) for c in data]

cases = load_cases(args.file) if args.file else default_cases()

print(f"Opening {args.port} @ {args.baud}")
with serial.Serial(args.port, args.baud, timeout=1) as s:
    # give Arduino time to reset after opening port
    time.sleep(2)
    print("Enabling TESTMODE")
    s.write(b'TESTMODE ON\n')
    time.sleep(0.1)
    for delay,x,y,z in cases:
        line = f"MPU {x} {y} {z}\n"
        print(f"SEND: {line.strip()}")
        s.write(line.encode('ascii'))
        time.sleep(delay)
    time.sleep(0.1)
    print("Disabling TESTMODE")
    s.write(b'TESTMODE OFF\n')

print("Done")
