#!/usr/bin/env python3

# Usage:
# ./stats.py results.csv chart.svg
# ./stats.py official.csv official_chart.svg

import argparse
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


parser = argparse.ArgumentParser()
parser.add_argument('input', help='CSV file')
parser.add_argument('output', help='SVG file')
args = parser.parse_args()

rows = []
with open(args.input, 'r') as f:
    for row in csv.reader(f):
        if row[0] == 'title':
            continue

        file_name = row[0]
        flags = [int(row[1]), int(row[2]), int(row[3]), int(row[4]),
                 int(row[5]), int(row[6]), int(row[7]), int(row[8]),
                 int(row[9])]

        rows.append(RowData(file_name, flags))

passed = [0, 0, 0, 0, 0, 0, 0, 0, 0]
for row in rows:
    for idx, flag in enumerate(row.flags):
        if flag == PASSED or flag == UNKNOWN:
            passed[idx] = passed[idx] + 1

barh_data = json.dumps(
{
    "items_font": {
        "family": "Arial",
        "size": 12
    },
    "items": [
        {
            "name": "resvg 0.10.0",
            "value": passed[2]
        },
        {
            "name": "Chromium r756035",
            "value": passed[0]
        },
        {
            "name": "Firefox 77",
            "value": passed[1]
        },
        {
            "name": "Inkscape 1.0",
            "value": passed[4]
        },
        {
            "name": "librsvg 2.49.1",
            "value": passed[5]
        },
        {
            "name": "Batik 1.13",
            "value": passed[3]
        },
        {
            "name": "SVG.NET 3.0.102",
            "value": passed[6]
        },
        {
            "name": "QtSvg 5.14.2",
            "value": passed[8]
        },
        {
            "name": "wxSvg 1.5.11",
            "value": passed[7]
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

try:
    subprocess.check_call(['./barh', 'chart.json', 'site/images/' + args.output])
except FileNotFoundError:
    print('Error: \'barh\' executable is not found.\n'
          'You should build https://github.com/RazrFalcon/barh '
          'and link resultig binary to the current directory.')
