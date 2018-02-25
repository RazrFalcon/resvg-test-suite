#!/usr/bin/env python3

# TODO: write to file and not to stdout

import argparse
import re
import csv
import xml.etree.ElementTree as ET


class RowData:
    def __init__(self, type, name, title, flags):
        self.type = type
        self.name = name
        self.title = title
        self.flags = flags


parser = argparse.ArgumentParser()
parser.add_argument('type', choices=['elements', 'attributes', 'presentation-attributes'],
                    help='Sets the table type')
args = parser.parse_args()

rows = []

with open('results.csv', 'r') as f:
    for row in csv.reader(f):
        file_name = row[0]
        if file_name[0] == 'e':
            type = 'element'
        else:
            type = 'attribute'

        # Note! We swapped resvg and chrome.
        flags = [int(row[2]), int(row[1]), int(row[3]), int(row[4]), int(row[5])]

        tag_name = file_name
        tag_name = tag_name[2:];
        tag_name = re.sub('-.*', '', tag_name)

        tree = ET.parse('svg/' + file_name)
        title = list(tree.getroot())[0].text

        rows.append(RowData(type, tag_name, title, flags))

if args.type == 'elements':
    print('// This file is autogenerated. Do not edit it.\n')
    print('[cols="1,10,^1,^1,^1,^1,^1"]')
    print('|===')
    print('| Element | Feature | resvg | Chrome | Inkscape | rsvg | QtSvg')

    elements_order = [
        'https://www.w3.org/TR/SVG/struct.html[Document Structure]',
        'svg',
        'g',
        'defs',
        'desc',
        'title',
        'symbol',
        'use',
        'image',
        'switch',
        'https://www.w3.org/TR/SVG/styling.html[Styling]',
        'style',
        'https://www.w3.org/TR/SVG/paths.html[Paths]',
        'path',
        'https://www.w3.org/TR/SVG/shapes.html[Basic Shapes]',
        'rect',
        'circle',
        'ellipse',
        'line',
        'polyline',
        'polygon',
        'https://www.w3.org/TR/SVG/text.html[Text]',
        'text',
        'tspan',
        'tref',
        'textPath',
        'altGlyph',
        'altGlyphDef',
        'altGlyphItem',
        'glyphRef',
        'https://www.w3.org/TR/SVG/painting.html[Painting: Filling, Stroking and Marker Symbols]',
        'marker',
        'https://www.w3.org/TR/SVG/color.html[Color]',
        'color-profile',
        'https://www.w3.org/TR/SVG/pservers.html[Gradients and Patterns]',
        'linearGradient',
        'radialGradient',
        'stop',
        'pattern',
        'https://www.w3.org/TR/SVG/masking.html[Clipping, Masking and Compositing]',
        'clipPath',
        'mask',
        'https://www.w3.org/TR/SVG/filters.html[Filter Effects]',
        'filter',
        'feDistantLight',
        'fePointLight',
        'feSpotLight',
        'feBlend',
        'feColorMatrix',
        'feComponentTransfer',
        'feComposite',
        'feConvolveMatrix',
        'feDiffuseLighting',
        'feDisplacementMap',
        'feFlood',
        'feGaussianBlur',
        'feImage',
        'feMerge',
        'feMorphology',
        'feOffset',
        'feSpecularLighting',
        'feTile',
        'feTurbulence',
        'feFuncR',
        'feFuncG',
        'feFuncB',
        'feFuncA',
        'https://www.w3.org/TR/SVG/interact.html[Interactivity]',
        'cursor',
        'https://www.w3.org/TR/SVG/linking.html[Linking]',
        'a',
        'view',
        'https://www.w3.org/TR/SVG/script.html[Scripting]',
        'script',
        'https://www.w3.org/TR/SVG/animate.html[Animation]',
        'animate',
        'set',
        'animateMotion',
        'animateColor',
        'animateTransform',
        'mpath',
        'https://www.w3.org/TR/SVG/fonts.html[Fonts]',
        'font',
        'glyph',
        'missing-glyph',
        'hkern',
        'vkern',
        'font-face',
        'font-face-src',
        'font-face-uri',
        'font-face-format',
        'font-face-name',
        'https://www.w3.org/TR/SVG/metadata.html[Metadata]',
        'metadata',
        'https://www.w3.org/TR/SVG/extend.html[Extensibility]',
        'foreignObject',
    ]

    for elem in elements_order:
        if elem.startswith('https'):
            print('7+^|' + elem)
        else:
            # we have to precede cells after span with ^| because of
            # https://github.com/asciidoctor/asciidoctor/issues/989
            print('2+|', elem, '^| {unk-box} | {unk-box} | {unk-box} | {unk-box} | {unk-box}')

            i = 1
            for row in rows:
                if row.name != elem:
                    continue;

                flags_str = ''
                for flag in row.flags:
                    if flag == 0:
                        flags_str += '|{unk-box}'
                    elif flag == 1:
                        flags_str += '|{ok-box}'
                    elif flag == 2:
                        flags_str += '|{fail-box}'
                    elif flag == 3:
                        flags_str += '|{crash-box}'

                print('||', str(i) + '. ' + row.title, flags_str)
                i += 1

    print('|===')
elif args.type == 'presentation-attributes':
    print('// This file is autogenerated. Do not edit it.\n')
    print('[cols="1,10,^1,^1,^1,^1,^1"]')
    print('|===')
    print('| Attribute | Feature | resvg | Chrome | Inkscape | rsvg | QtSvg')

    attrs_order = [
        'alignment-baseline',
        'baseline-shift',
        'clip',
        'clip-path',
        'clip-rule',
        'color',
        'color-interpolation',
        'color-interpolation-filters',
        'color-profile',
        'color-rendering',
        'cursor',
        'direction',
        'display',
        'dominant-baseline',
        'enable-background',
        'fill',
        'fill-opacity',
        'fill-rule',
        'filter',
        'flood-color',
        'flood-opacity',
        'font',
        'font-family',
        'font-size',
        'font-size-adjust',
        'font-stretch',
        'font-style',
        'font-variant',
        'font-weight',
        'glyph-orientation-horizontal',
        'glyph-orientation-vertical',
        'image-rendering',
        'kerning',
        'letter-spacing',
        'lighting-color',
        'marker',
        'marker-end',
        'marker-mid',
        'marker-start',
        'mask',
        'opacity',
        'overflow',
        'pointer-events',
        'shape-rendering',
        'stop-color',
        'stop-opacity',
        'stroke',
        'stroke-dasharray',
        'stroke-dashoffset',
        'stroke-linecap',
        'stroke-linejoin',
        'stroke-miterlimit',
        'stroke-opacity',
        'stroke-width',
        'text-anchor',
        'text-decoration',
        'text-rendering',
        'unicode-bidi',
        'visibility',
        'word-spacing',
        'writing-mode',
    ]

    for attr in attrs_order:
        # we have to precede cells after span with ^| because of
        # https://github.com/asciidoctor/asciidoctor/issues/989
        print('2+|', attr, '^| {unk-box} | {unk-box} | {unk-box} | {unk-box} | {unk-box}')

        i = 1
        for row in rows:
            if row.name != attr:
                continue;

            flags_str = ''
            for flag in row.flags:
                if flag == 0:
                    flags_str += '|{unk-box}'
                elif flag == 1:
                    flags_str += '|{ok-box}'
                elif flag == 2:
                    flags_str += '|{fail-box}'
                elif flag == 3:
                    flags_str += '|{crash-box}'

            print('||', str(i) + '. ' + row.title, flags_str)
            i += 1

    print('|===')
else:
    print('// This file is autogenerated. Do not edit it.\n')
    print('[cols="1,10,^1,^1,^1,^1,^1"]')
    print('|===')
    print('| Attribute | Feature | resvg | Chrome | Inkscape | rsvg | QtSvg')

    attrs_order = [
        'transform',
    ]

    for attr in attrs_order:
        # we have to precede cells after span with ^| because of
        # https://github.com/asciidoctor/asciidoctor/issues/989
        print('2+|', attr, '^| {unk-box} | {unk-box} | {unk-box} | {unk-box} | {unk-box}')

        i = 1
        for row in rows:
            if row.name != attr:
                continue;

            flags_str = ''
            for flag in row.flags:
                if flag == 0:
                    flags_str += '|{unk-box}'
                elif flag == 1:
                    flags_str += '|{ok-box}'
                elif flag == 2:
                    flags_str += '|{fail-box}'
                elif flag == 3:
                    flags_str += '|{crash-box}'

            print('||', str(i) + '. ' + row.title, flags_str)
            i += 1

    print('|===')
