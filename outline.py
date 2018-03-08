#!/usr/bin/env python3

import argparse
import re
import subprocess
import xml.etree.ElementTree as ET
from prettytable import PrettyTable


svgparser_tests = [
    'a-fill-001.svg',
    'a-fill-002.svg',
    'a-fill-003.svg',
    'a-fill-004.svg',
    'a-fill-005.svg',
    'a-fill-006.svg',
    'a-fill-007.svg',
    'a-fill-008.svg',
    'a-fill-009.svg',
    'a-fill-010.svg',
    'a-fill-011.svg',
    'a-fill-012.svg',
    'a-fill-013.svg',
    'a-fill-014.svg',
    'a-fill-015.svg',
    'a-fill-019.svg',
    'a-fill-027.svg',
    'a-fill-028.svg',
    'a-stroke-001.svg',
    'a-stroke-dasharray-010.svg',
    'a-stroke-dasharray-011.svg',
    'a-transform-013.svg',
    'a-transform-014.svg',
    'a-transform-017.svg',
    'e-path-011.svg',
    'e-path-031.svg',
    'e-path-032.svg',
    'e-path-033.svg',
    'e-path-034.svg',
    'e-path-035.svg',
    'e-path-036.svg',
    'e-path-037.svg',
    'e-path-038.svg',
    'e-path-039.svg',
    'e-path-040.svg',
    'e-path-041.svg',
    'e-path-042.svg',
    'e-path-043.svg',
]

svgdom_tests = [
    'a-fill-024.svg',
    'a-fill-025.svg',
    'a-fill-026.svg',
    'a-stroke-005.svg',
    'a-transform-002.svg',
    'a-transform-003.svg',
    'a-transform-004.svg',
    'a-transform-005.svg',
    'a-transform-006.svg',
    'a-transform-007.svg',
    'a-transform-008.svg',
    'a-transform-009.svg',
    'a-transform-012.svg',
    'a-transform-018.svg',
]


def find_test(dir, test):
    if test in svgparser_tests:
        return 'svgparser'

    if test in svgdom_tests:
        return 'svgdom'

    try:
        output = subprocess.check_output(['grep', '.', '-n', '-r', '-e', test], cwd=dir)
        output = output.decode('ascii')
        paths = []
        for line in output.split('\n'):
            if not line:
                continue

            parts = line.split(':')
            path = parts[0]
            path = path[2:]  # remove ./
            line_num = parts[1]
            paths.append(path + ':' + line_num)
        return ', '.join(paths)
    except subprocess.CalledProcessError:
        return ''


parser = argparse.ArgumentParser()
parser.add_argument('-f, --filter', dest='filter', help='Filter tests')
parser.add_argument('--check', dest='check', help='Find tests in source dir')
args = parser.parse_args()

with open('order.txt', 'r') as f:
    file_list = f.read().splitlines()

table = PrettyTable()
if args.check:
    table.field_names = ['#', 'File', 'Title', 'Path']
else:
    table.field_names = ['#', 'File', 'Title']

table.align = "l"

i = 1
for file_name in file_list:
    tag_name = file_name[2:]
    tag_name = re.sub('-[0-9]+\.svg', '', tag_name)

    if args.filter:
        if args.filter != tag_name:
            continue

    tree = ET.parse('svg/' + file_name)
    title = list(tree.getroot())[0].text

    if args.check:
        table.add_row([str(i), file_name, title, find_test(args.check, file_name)])
    else:
        table.add_row([str(i), file_name, title])

    i += 1

print(table)
