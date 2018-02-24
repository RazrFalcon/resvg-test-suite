#!/usr/bin/env python3

import argparse
import os
import re
import xml.etree.ElementTree as ET
from prettytable import PrettyTable


parser = argparse.ArgumentParser()
parser.add_argument('-f, --filter', dest='filter', help='Filter tests')
args = parser.parse_args()

file_list = []

for root, _, files in os.walk('./svg'):
    for file_name in files:
        file_stem, file_ext = os.path.splitext(file_name)
        if file_ext == '.svg':
            abs_path = os.path.join(root, file_name)
            file_list.append((abs_path, file_name))

file_list = sorted(file_list, key=lambda x: x[1])

table = PrettyTable()
table.field_names = ['File', 'Title']
table.align = "l"

titles = {}

for idx, file in enumerate(file_list):
    abs_path, file_name = file

    tag_name = file_name[2:];
    tag_name = re.sub('-.*', '', tag_name)

    if args.filter:
        if args.filter != tag_name:
            continue

    tree = ET.parse(abs_path)
    title = list(tree.getroot())[0].text

    if title in titles:
        print('Warning: the message \'{}\' already set in \'{}\''.format(title, titles[title]))

    table.add_row([file_name, title])

    titles[tag_name] = file_name

print(table)
