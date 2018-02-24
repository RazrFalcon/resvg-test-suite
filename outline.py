#!/usr/bin/env python3

import argparse
import re
import xml.etree.ElementTree as ET
from prettytable import PrettyTable


parser = argparse.ArgumentParser()
parser.add_argument('-f, --filter', dest='filter', help='Filter tests')
args = parser.parse_args()

file_list = []

with open('order.txt', 'r') as f:
    file_list = f.read().splitlines()

table = PrettyTable()
table.field_names = ['File', 'Title']
table.align = "l"

titles = {}

for idx, file_name in enumerate(file_list):
    tag_name = file_name[2:];
    tag_name = re.sub('-.*', '', tag_name)

    if args.filter:
        if args.filter != tag_name:
            continue

    tree = ET.parse('svg/' + file_name)
    title = list(tree.getroot())[0].text

    if title in titles:
        print('Warning: the message \'{}\' already set in \'{}\''.format(title, titles[title]))

    table.add_row([file_name, title])

    titles[tag_name] = file_name

print(table)
