#!/usr/bin/env python3

import argparse
import re
import csv
import xml.etree.ElementTree as ET


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

out_of_scope_elems = [
    'desc',
    'title',
    'altGlyph',
    'altGlyphDef',
    'altGlyphItem',
    'glyphRef',
    'color-profile',
    'cursor',
    'script',
    'animate',
    'set',
    'animateMotion',
    'animateColor',
    'animateTransform',
    'mpath',
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
    'metadata',
]

presentation_attrs = [
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

out_of_scope_pres_attrs = [
    'cursor',
    'pointer-events',
]


class RowData:
    def __init__(self, type, name, title, flags):
        self.type = type
        self.name = name
        self.title = title
        self.flags = flags


def global_flags(rows, name):
    flags = [0, 0, 0, 0, 0]
    has_any_tests = False
    for row in rows:
        if row.name == name:
            has_any_tests = True

            for idx, flag in enumerate(row.flags):
                if flag == 1 and flags[idx] == 0:
                    flags[idx] = 1

                if flag == 2 or flag == 3:
                    flags[idx] = 4

    if not has_any_tests:
        flags = [2, 0, 0, 0, 0]

    return flags


def flags_to_string(flags):
    flags_str = ''
    for flag in flags:
        if   flag == 0:
            flags_str += '|{unk-box}'
        elif flag == 1:
            flags_str += '|{ok-box}'
        elif flag == 2:
            flags_str += '|{fail-box}'
        elif flag == 3:
            flags_str += '|{crash-box}'
        elif flag == 4:
            flags_str += '|{part-box}'
        elif flag == 5:
            flags_str += '|{oos-box}'

    return flags_str


def gen_header(name):
    out  = '// This file is autogenerated. Do not edit it.\n\n'
    out += '[cols="1,10,^1,^1,^1,^1,^1"]\n'
    out += '|===\n'
    out += '| ' + name + ' | Feature | resvg | Chrome | Inkscape | rsvg | QtSvg\n'
    return out


def get_item_row(rows, out_of_scope_list, name):
    flags = []
    if name in out_of_scope_list:
        flags = [5, 5, 5, 5, 5]
    else:
        flags = global_flags(rows, name)

    # we have to precede cell after span with ^ because of
    # https://github.com/asciidoctor/asciidoctor/issues/989
    return '2+| {} ^{}\n'.format(name, flags_to_string(flags))


parser = argparse.ArgumentParser()
parser.add_argument('type', choices=['elements', 'attributes', 'presentation', 'all'],
                    help='Sets the table type')
args = parser.parse_args()

rows = []
with open('results.csv', 'r') as f:
    for row in csv.reader(f):
        if row[0] == 'title':
            continue

        file_name = row[0]
        if file_name[0] == 'e':
            type = 'element'
        else:
            type = 'attribute'

        # Note! We swapped resvg and chrome.
        flags = [int(row[2]), int(row[1]), int(row[3]), int(row[4]), int(row[5])]

        tag_name = file_name
        tag_name = tag_name[2:];
        tag_name = re.sub('-[0-9]+\.svg', '', tag_name)

        tree = ET.parse('svg/' + file_name)
        title = list(tree.getroot())[0].text

        rows.append(RowData(type, tag_name, title, flags))

if args.type == 'elements' or args.type == 'all':
    out = gen_header('Element')

    for elem in elements_order:
        if elem.startswith('https'):
            out += '7+^|' + elem + '\n'
        else:
            out += get_item_row(rows, out_of_scope_elems, elem)

            i = 1
            for row in rows:
                if row.name == elem:
                    out += '|| {}. {} {}\n'.format(i, row.title, flags_to_string(row.flags))
                    i += 1

            out += '7+^|\n'

    out += '|===\n'
    with open('site/elements-table.adoc', 'w') as f:
        f.write(out)

if args.type == 'presentation' or args.type == 'all':
    out = gen_header('Attribute')

    for attr in presentation_attrs:
        out += get_item_row(rows, out_of_scope_pres_attrs, attr)

        i = 1
        for row in rows:
            if row.name == attr:
                out += '|| {}. {} {}\n'.format(i, row.title, flags_to_string(row.flags))
                i += 1

        out += '7+^|\n'

    out += '|===\n'
    with open('site/presentation-attributes-table.adoc', 'w') as f:
        f.write(out)

if args.type == 'attributes' or args.type == 'all':
    out = gen_header('Attribute')

    # collect all non presentation attributes
    attrs_order = set()
    for row in rows:
        if row.type == 'attribute':
            if row.name not in presentation_attrs:
                attrs_order.add(row.name)

    for attr in attrs_order:
        out += get_item_row(rows, [], attr)

        i = 1
        for row in rows:
            if row.name == attr:
                out += '|| {}. {} {}\n'.format(i, row.title, flags_to_string(row.flags))
                i += 1

        out += '7+^|\n'

    out += '|===\n'
    with open('site/attributes-table.adoc', 'w') as f:
        f.write(out)
