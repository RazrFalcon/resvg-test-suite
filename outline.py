#!/usr/bin/env python3

import argparse
import re
import subprocess
import xml.etree.ElementTree as ET
from prettytable import PrettyTable


parser = argparse.ArgumentParser()
parser.add_argument('-f, --filter', dest='filter', help='Filter tests')
args = parser.parse_args()

with open('order.txt', 'r') as f:
    file_list = f.read().splitlines()

table = PrettyTable()
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
    table.add_row([str(i), file_name, title])

    i += 1

print(table)
