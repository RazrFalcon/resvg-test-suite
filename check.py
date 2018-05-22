#!/usr/bin/env python3

import re
import os
import subprocess
import csv
import xml.etree.ElementTree as ET


def check_order():
    """
    Checks that order.txt and ./svg dir has the same files
    """

    files = sorted(os.listdir('svg/'))

    with open('order.txt', 'r') as f:
        order = f.read().splitlines()

    added = True
    diff = [x for x in files if x not in order]
    if not diff:
        added = False
        diff = [x for x in order if x not in files]

    if not diff:
        # in sync - ok
        return

    if added:
        print('Add those files to the order.txt:')
    else:
        print('Remove those files from the order.txt:')

    diff.sort()

    for file in diff:
        print(file)

    print()  # new line

    raise ValueError('order.txt is out of date')


def check_results():
    """
    Checks that results.csv has all tests from order.txt
    """

    with open('order.txt', 'r') as f:
        order = f.read().splitlines()

    results = []
    with open('results.csv', 'r') as f:
        reader = csv.reader(f)
        next(reader, None)  # skip header
        for row in reader:
            results.append(row[0])

    diff = [x for x in order if x not in results]
    if diff:
        raise ValueError('results.csv is out of date')


def check_untracked_files():
    output = subprocess.check_output(['git', 'ls-files', '--others', '--exclude-standard', 'svg'])
    if not output:
        return

    output = output.decode('ascii')
    print('Untracked files:')
    print(output)
    raise ValueError('not all tests are added to the git')


def check_title():
    """
    Checks that element/attribute tests has unique titles and shorter than 60 symbols
    """

    files = os.listdir('svg/')

    titles = {}
    for file in files:
        tag_name = file[2:]
        tag_name = re.sub('-[0-9]+\.svg', '', tag_name)

        tree = ET.parse('svg/' + file)
        title = list(tree.getroot())[0].text

        if len(title) > 60:
            raise ValueError('{} has title longer than 60 symbols'.format(file))

        if title in titles:
            if titles[title][0] == tag_name:
                raise ValueError('{} and {} has the same title'.format(titles[title][1], file))

        titles[title] = (tag_name, file)


def check_node_ids():
    """
    Checks that all elements has an unique ID attribute.
    """

    files = os.listdir('svg/')

    ignore_files = [
        'e-svg-031.svg',  # because of ENTITY
        'e-svg-032.svg',  # because of ENTITY
    ]

    ignore_tags = [
        'title',
        'desc',
        'stop',
    ]

    for file in ignore_files:
        files.remove(file)

    for file in files:
        tree = ET.parse('svg/' + file)
        ids = set()

        for node in tree.getroot().iter():
            # extract tag name without namespace
            _, tag = node.tag[1:].split('}')  # WTF python?!

            if tag not in ignore_tags:
                node_id = node.get('id')
                # ID must be set
                if not node_id:
                    raise ValueError('\'{}\' element in {} has no ID'
                                     .format(tag, file))
                else:
                    # Check that ID is unique
                    if node_id in ids:
                        raise ValueError('\'{}\' ID already exist in {}'
                                         .format(node_id, file))
                    else:
                        ids.add(node_id)


def check_line_width():
    allow = [
        'e-svg-004.svg',
        'e-svg-005.svg',
        'e-svg-007.svg',
        'e-svg-031.svg',
        'e-svg-032.svg',
        'a-fill-028.svg',
        'e-tspan-010.svg',
    ]

    files = os.listdir('svg/')

    for file in allow:
        files.remove(file)

    for file in files:
        with open('svg/' + file, 'r') as f:
            for i, line in enumerate(f.read().splitlines()):
                if len(line) > 100:
                    raise ValueError('Line {} in {} is longer than 100 characters'.format(i, file))


def main():
    check_order()
    check_results()
    check_title()
    check_node_ids()
    check_untracked_files()
    check_line_width()


if __name__ == '__main__':
    try:
        main()
    except ValueError as e:
        print('Error: {}.'.format(e))
        exit(1)
