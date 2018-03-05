#!/usr/bin/env python3

import re
import csv
import json
import subprocess


UNKNOWN      = 0
PASSED       = 1
FAILED       = 2
CRASHED      = 3
PARTIAL      = 4
OUT_OF_SCOPE = 5

class RowData:
    def __init__(self, name, flags):
        self.name = name
        self.flags = flags


rows = []
with open('results.csv', 'r') as f:
    for row in csv.reader(f):
        if row[0] == 'title':
            continue

        file_name = row[0]

        # Note! We swapped resvg and chrome.
        flags = [int(row[2]), int(row[1]), int(row[3]), int(row[4]), int(row[5])]

        rows.append(RowData(file_name, flags))

file_list = []
with open('order.txt', 'r') as f:
    file_list = f.read().splitlines()

passed = [0, 0, 0, 0, 0]
for file_name in file_list:
    for row in rows:
        if row.name == file_name:
            for idx, flag in enumerate(row.flags):
                if flag == PASSED:
                    passed[idx] = passed[idx] + 1

barh_data = json.dumps(
    {
    "title": "resvg test suite",
    "items_font": {
        "family": "Arial",
        "size": 12
    },
    "items": [
        {
            "name": "resvg 0.2.0",
            "value": passed[0]
        },
        {
            "name": "Chromium r536395",
            "value": passed[1]
        },
        {
            "name": "Inkscape 0.92.2",
            "value": passed[2]
        },
        {
            "name": "librsvg 2.40.18",
            "value": passed[3]
        },
        {
            "name": "QtSvg 5.9.4",
            "value": passed[4]
        }
    ],
    "hor_axis": {
        "title": "Tests passed",
        "round_tick_values": True,
        "width": 700,
        "max_value": len(file_list)
    }
}, indent=4)

with open('chart.json', 'w') as f:
    f.write(barh_data)

try:
    subprocess.check_call(['./barh', 'chart.json', 'site/images/chart.svg'])
except FileNotFoundError:
    print('Error: \'barh\' executable is not found.\n'
          'You should build https://github.com/RazrFalcon/barh '
          'and link resultig binary to current directory.')
