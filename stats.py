#!/usr/bin/env python3

import csv
import json
import subprocess
import sys
import os
from pathlib import Path

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
    files = list(Path('tests').rglob('*.svg'))
    files.remove(Path('tests/structure/svg/not-UTF-8-encoding.svg'))
    for file in files:
        with open(file, 'r') as f:
            if "(SVG 2)" in f.read():
                svg2_files.append(str(file).replace('tests/', ''))

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
                 int(row[9])]

        rows.append(RowData(file_name, flags))

passed = [0, 0, 0, 0, 0, 0, 0, 0, 0]
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
            "name": "resvg 0.40.0",
            "value": passed[3]
        },
        {
            "name": "Chrome 123",
            "value": passed[0]
        },
        {
            "name": "Firefox 124",
            "value": passed[1]
        },
        {
            "name": "Safari 17.3.1",
            "value": passed[2]
        },
        {
            "name": "librsvg 2.58.0",
            "value": passed[6]
        },
        {
            "name": "Inkscape 1.3.2",
            "value": passed[5]
        },
        {
            "name": "Batik 1.17",
            "value": passed[4]
        },
        {
            "name": "SVG.NET 3.2.3",
            "value": passed[7]
        },
        {
            "name": "QtSvg 6.7.0",
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
