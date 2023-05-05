#!/usr/bin/env python3

import re
import csv
import xml.etree.ElementTree as ET


UNKNOWN      = 0
PASSED       = 1
FAILED       = 2
CRASHED      = 3
PARTIAL      = 4
OUT_OF_SCOPE = 5

class RowData:
    def __init__(self, category, subcategory, title, flags):
        self.category = category
        self.subcategory = subcategory
        self.title = title
        self.flags = flags


def global_flags(rows, category, subcategory):
    passed_list = [0, 0, 0, 0, 0, 0, 0, 0, 0]
    total = 0
    for row in rows:
        if row.category != category or row.subcategory != subcategory:
            continue

        if "(UB)" in row.title:
            continue

        total += 1
        for idx, flag in enumerate(row.flags):
            if flag == PASSED or flag == UNKNOWN:
                passed_list[idx] += 1

    return passed_list, total


def get_item_row(rows, category, subcategory):
    flags, total = global_flags(rows, category, subcategory)
    if flags == [0, 0, 0, 0, 0, 0, 0, 0, 0]:
        flags = [FAILED, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN]


    html  = '<tr>\n'
    html += f'<td id="{category}-{subcategory}" colspan="2">{subcategory}</td>\n'

    if total != 0:
        for count in flags:
            v = (float(count) / float(total)) * 100.0
            html += '<td class="td-align">{:.0f}%</td>\n'.format(v)
    else:
        if name in out_of_scope_list:
            html += '<td colspan="9">Not planned</td>\n'
        else:
            html += '<td colspan="9">Not supported by <b>resvg<b></td>\n'

    html += '</tr>\n'
    return html


def generate_table(category, rows):
    html = \
        ('<table width="100%">\n'
         '<colgroup>\n'
         '<col width="5%">\n'
         '<col width="50%">\n'
         '<col width="5%">\n'
         '<col width="5%">\n'
         '<col width="5%">\n'
         '<col width="5%">\n'
         '<col width="5%">\n'
         '<col width="5%">\n'
         '<col width="5%">\n'
         '</colgroup>\n'
         '<thead>\n'
         '<tr>\n'
         '<th></th>\n'
         '<th>Test</th>\n'
         '<th>resvg</th>\n'
         '<th>Chrome</th>\n'
         '<th>Firefox</th>\n'
         '<th>Safari</th>\n'
         '<th>Batik</th>\n'
         '<th>Inkscape</th>\n'
         '<th>librsvg</th>\n'
         '<th>SVG.NET</th>\n'
         '<th>QtSvg</th>\n'
         '</tr>\n'
         '</thead>\n')

    last_subcategory = ''
    for row in rows:
        if row.category != category:
            continue

        if row.subcategory != last_subcategory:
            if last_subcategory != '':
                html += '<tr>\n<td colspan="11"></td>\n<tr>\n'

            html += get_item_row(rows, row.category, row.subcategory)

        last_subcategory = row.subcategory

        html += '<tr>\n'
        html += '<td></td>\n'
        # TODO: replace ticks with <code>
        html += f'<td>{row.title}</td>\n'

        for flag in row.flags:
            if flag == UNKNOWN:
                html += '<td class="td-align"><b>?</b></td>\n'
            elif flag == PASSED:
                html += '<td class="td-align test-passed"><i class="icon-ok"></i></td>\n'
            elif flag == FAILED:
                html += '<td class="td-align test-failed"><i class="icon-cancel"></i></td>\n'
            elif flag == CRASHED:
                html += '<td class="td-align test-crashed"><i class="icon-attention"></i></td>\n'
            else:
                html += '<td></td>\n'

        html += '</tr>\n'

    html += '</table>\n'

    with open(f'autogen-{category}-table.html', 'w') as f:
        f.write(html)


def main():
    rows = []
    with open('../results.csv', 'r') as f:
        reader = csv.reader(f)
        next(reader, None)  # skip header
        for row in reader:
            file_name = row[0]

            # Note! We set resvg to the first place.
            flags = [int(row[4]), int(row[1]), int(row[2]), int(row[3]),
                     int(row[5]), int(row[6]), int(row[7]), int(row[8]),
                     int(row[9])]

            name_parts = file_name.split('/')
            category = name_parts[0]
            subcategory = name_parts[1]

            tree = ET.parse('../tests/' + file_name)
            title = list(tree.getroot())[0].text

            rows.append(RowData(category, subcategory, title, flags))

    generate_table('filters', rows)
    generate_table('masking', rows)
    generate_table('paint-servers', rows)
    generate_table('painting', rows)
    generate_table('shapes', rows)
    generate_table('structure', rows)
    generate_table('text', rows)


if __name__ == '__main__':
    main()
