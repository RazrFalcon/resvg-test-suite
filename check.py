#!/usr/bin/env python3

import re
import os
import subprocess
from lxml import etree
from pathlib import Path


def split_qname(name):
    if name[0] == '{':
        return name[1:].split('}')
    else:
        return [None, name]


def check_title():
    """
    Checks that element/attribute tests has unique titles and shorter than 60 symbols
    """

    files = Path('tests').rglob('*.svg')

    titles = {}
    for file in files:
        file = str(file)
        tag_name = re.sub('-[0-9]+\.svg', '', file)

        tree = etree.parse(file)
        title = list(tree.getroot())[0].text

        if len(title) > 60:
            raise ValueError('{} has title longer than 60 symbols'.format(file))

        if title in titles:
            if titles[title][0] == tag_name:
                raise ValueError('{} and {} have the same title'.format(titles[title][1], file))

        titles[title] = (tag_name, file)


def check_node_ids():
    """
    Checks that all elements has an unique ID attribute.
    """

    files = sorted(list(Path('tests').rglob('*.svg')))

    ignore_files = [
        'tests/structure/svg/031.svg',  # because of ENTITY
        'tests/structure/svg/032.svg',  # because of ENTITY
        'tests/structure/use/024.svg',  # intended duplicate
    ]

    ignore_tags = [
        'title',
        'desc',
        'stop',
        'feBlend',
        'feColorMatrix',
        'feComponentTransfer',
        'feComposite',
        'feConvolveMatrix',
        'feDiffuseLighting',
        'feDistantLight',
        'feFlood',
        'feFuncA',
        'feFuncB',
        'feFuncG',
        'feFuncR',
        'feGaussianBlur',
        'feImage',
        'feMerge',
        'feMergeNode',
        'feMorphology',
        'feOffset',
        'fePointLight',
        'feSpecularLighting',
        'feSpotLight',
        'feTile',
        'feTurbulence',
        'feDropShadow',
    ]

    for file in ignore_files:
        files.remove(Path(file))

    for file in files:
        tree = etree.parse(file)
        ids = set()

        for node in tree.getroot().iter():
            if node.tag is etree.Comment:
                continue

            # extract tag name without namespace
            _, tag = split_qname(node.tag)

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
        'tests/structure/svg/004.svg',
        'tests/structure/svg/005.svg',
        'tests/structure/svg/007.svg',
        'tests/structure/svg/031.svg',
        'tests/structure/svg/032.svg',
        'tests/painting/fill/028.svg',
        'tests/text/tspan/010.svg',
        'tests/structure/image/040.svg',
    ]

    files = sorted(list(Path('tests').rglob('*.svg')))

    for file in allow:
        files.remove(Path(file))

    for file in files:
        with open(file, 'r') as f:
            for i, line in enumerate(f.read().splitlines()):
                if len(line) > 100:
                    raise ValueError('Line {} in {} is longer than 100 characters'.format(i, file))


def check_for_unused_xlink_ns():
    # In case when 'xlink:href' is present, but namespace is not set
    # the 'lxml' will raise an error.

    allow = [
        'tests/structure/svg/003.svg',
        'tests/structure/svg/032.svg',
    ]

    files = sorted(list(Path('tests').rglob('*.svg')))

    for file in allow:
        files.remove(Path(file))

    for file in files:
        tree = etree.parse(file)

        has_href = False
        for node in tree.getroot().iter():
            if '{http://www.w3.org/1999/xlink}href' in node.attrib:
                has_href = True
                break

        if not has_href and 'xlink' in tree.getroot().nsmap:
            raise ValueError('{} has an unneeded xlink namespace'.format(file))


check_title()
check_node_ids()
check_line_width()
check_for_unused_xlink_ns()
