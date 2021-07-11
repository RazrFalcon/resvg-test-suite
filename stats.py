#!/usr/bin/env python3

import csv
import json
import subprocess
import sys
import os

UNKNOWN      = 0
PASSED       = 1
FAILED       = 2
CRASHED      = 3

class RowData:
    def __init__(self, name, flags):
        self.name = name
        self.flags = flags


is_svg2_only = "--svg2" in sys.argv

svg2_files = []
if is_svg2_only:
    files = sorted(os.listdir('svg/'))
    files.remove('e-svg-007.svg') # not UTF-8
    for name in files:
        with open('svg/' + name, 'r') as f:
            if "(SVG 2)" in f.read():
                svg2_files.append(name)

rows = []
with open('results.csv', 'r') as f:
    for row in csv.reader(f):
        if row[0] == 'title':
            continue

        file_name = row[0]

        if is_svg2_only and file_name not in svg2_files:
            continue

        # Skip UB
        if int(row[1]) == UNKNOWN:
            continue

        flags = [int(row[1]), int(row[2]), int(row[3]), int(row[4]),
                 int(row[5]), int(row[6]), int(row[7]), int(row[8]),
                 int(row[9]), int(row[10])]

        rows.append(RowData(file_name, flags))

passed = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
for row in rows:
    for idx, flag in enumerate(row.flags):
        if flag == PASSED:
            passed[idx] = passed[idx] + 1

barh_data = json.dumps(
{
    "items_font": {
        "family": "Arial",
        "size": 12
    },
    "items": [
        {
            "name": "resvg 0.15.0",
            "value": passed[3]
        },
        {
            "name": "Chromium r883894",
            "value": passed[0]
        },
        {
            "name": "Firefox 89",
            "value": passed[1]
        },
        {
            "name": "Safari 14.1.1",
            "value": passed[2]
        },
        {
            "name": "Inkscape 1.1",
            "value": passed[5]
        },
        {
            "name": "librsvg 2.50.7",
            "value": passed[6]
        },
        {
            "name": "Batik 1.14",
            "value": passed[4]
        },
        {
            "name": "SVG.NET 3.2.3",
            "value": passed[7]
        },
        {
            "name": "QtSvg 6.1.1",
            "value": passed[9]
        },
        {
            "name": "wxSvg 1.5.11",
            "value": passed[8]
        }
    ],
    "hor_axis": {
        "title": "Tests passed",
        "round_tick_values": True,
        "width": 700,
        "max_value": len(rows)
    }
}, indent=4)

with open('chart.json', 'w') as f:
    f.write(barh_data)

if is_svg2_only:
    out_path = 'site/images/chart-svg2.svg'
else:
    out_path = 'site/images/chart.svg'

try:
    subprocess.check_call(['./barh', 'chart.json', out_path])
except FileNotFoundError:
    print('Error: \'barh\' executable is not found.\n'
          'You should build https://github.com/RazrFalcon/barh '
          'and link resultig binary to the current directory.')
