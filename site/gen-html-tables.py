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

ELEMENT_TYPE   = 'element'
ATTRIBUTE_TYPE = 'attribute'

elements_order = [
    '<a href="https://www.w3.org/TR/SVG11/struct.html">Document Structure</a>',
    'svg',
    'g',
    'defs',
    'desc',
    'title',
    'symbol',
    'use',
    'image',
    'switch',
    '<a id="section_styling" href="https://www.w3.org/TR/SVG11/styling">Styling</a>',
    'style',
    '<a id="section_paths" href="https://www.w3.org/TR/SVG11/paths">Paths</a>',
    'path',
    '<a id="section_shapes" href="https://www.w3.org/TR/SVG11/shapes">Basic Shapes</a>',
    'rect',
    'circle',
    'ellipse',
    'line',
    'polyline',
    'polygon',
    '<a id="section_text" href="https://www.w3.org/TR/SVG11/text">Text</a>',
    'text',
    'tspan',
    'tref',
    'textPath',
    'altGlyph',
    'altGlyphDef',
    'altGlyphItem',
    'glyphRef',
    '<a id="section_painting" href="https://www.w3.org/TR/SVG11/painting">Painting: Filling, Stroking and Marker Symbols</a>',
    'marker',
    '<a id="section_color" href="https://www.w3.org/TR/SVG11/color">Color</a>',
    'color-profile',
    '<a id="section_pservers" href="https://www.w3.org/TR/SVG11/pservers">Gradients and Patterns</a>',
    'linearGradient',
    'radialGradient',
    'stop',
    'pattern',
    '<a id="section_masking" href="https://www.w3.org/TR/SVG11/masking">Clipping, Masking and Compositing</a>',
    'clipPath',
    'mask',
    '<a id="section_filters" href="https://www.w3.org/TR/SVG11/filters">Filter Effects</a>',
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
    '<a id="section_interact" href="https://www.w3.org/TR/SVG11/interact">Interactivity</a>',
    'cursor',
    '<a id="section_linking" href="https://www.w3.org/TR/SVG11/linking">Linking</a>',
    'a',
    'view',
    '<a id="section_script" href="https://www.w3.org/TR/SVG11/script">Scripting</a>',
    'script',
    '<a id="section_animate" href="https://www.w3.org/TR/SVG11/animate">Animation</a>',
    'animate',
    'set',
    'animateMotion',
    'animateColor',
    'animateTransform',
    'mpath',
    '<a id="section_fonts" href="https://www.w3.org/TR/SVG11/fonts">Fonts</a>',
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
    '<a id="section_metadata" href="https://www.w3.org/TR/SVG11/metadata">Metadata</a>',
    'metadata',
    '<a id="section_extend" href="https://www.w3.org/TR/SVG11/extend">Extensibility</a>',
    'foreignObject',
]

out_of_scope_elems = [
    'altGlyph',
    'altGlyphDef',
    'altGlyphItem',
    'animate',
    'animateColor',
    'animateMotion',
    'animateTransform',
    'color-profile',
    'cursor',
    'desc',
    'font',
    'font-face',
    'font-face-format',
    'font-face-name',
    'font-face-src',
    'font-face-uri',
    'glyph',
    'glyphRef',
    'hkern',
    'metadata',
    'missing-glyph',
    'mpath',
    'script',
    'set',
    'title',
    'view',
    'vkern',
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
    'marker-start',
    'marker-mid',
    'marker-end',
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
    def __init__(self, type, name, index, title, flags):
        self.type = type
        self.name = name
        self.index = index
        self.title = title
        self.flags = flags


def global_flags(rows, type, name):
    passed_list = [0, 0, 0, 0, 0, 0, 0]
    total = 0
    for row in rows:
        if row.type != type or row.name != name:
            continue

        total += 1
        for idx, flag in enumerate(row.flags):
            if flag == PASSED or flag == UNKNOWN:
                passed_list[idx] += 1

    return passed_list, total


def get_item_row(rows, out_of_scope_list, type, name):
    if name in out_of_scope_list:
        flags = [OUT_OF_SCOPE, OUT_OF_SCOPE, OUT_OF_SCOPE, OUT_OF_SCOPE,
                 OUT_OF_SCOPE, OUT_OF_SCOPE, OUT_OF_SCOPE]
        total = 0
    else:
        flags, total = global_flags(rows, type, name)
        if flags == [0, 0, 0, 0, 0, 0, 0]:
            flags = [FAILED, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN]

    if type == ELEMENT_TYPE:
        anchor = 'e-' + name
    else:
        anchor = 'a-' + name

    html  = '<tr>\n'
    html += '<td colspan="3"><a id="{}"></a>{}</td>\n'.format(anchor, name)

    if total != 0:
        for count in flags:
            v = (float(count) / float(total)) * 100.0
            html += '<td class="td-align">{:.0f}%</td>\n'.format(v)
    else:
        html += ('<td></td>\n'
                 '<td></td>\n'
                 '<td></td>\n'
                 '<td></td>\n'
                 '<td></td>\n'
                 '<td></td>\n'
                 '<td></td>\n')

    html += '</tr>\n'
    return html


def gen_row(row):
    html  = '<tr>\n'
    html += '<td></td>\n'
    html += '<td>{}</td>\n'.format(row.index)
    html += '<td>{}</td>\n'.format(row.title)

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
    return html


def main():
    rows = []
    with open('../results.csv', 'r') as f:
        reader = csv.reader(f)
        next(reader, None)  # skip header
        for row in reader:
            file_name = row[0]
            if file_name[0] == 'e':
                type = ELEMENT_TYPE
            else:
                type = ATTRIBUTE_TYPE

            # Note! We set resvg to the first place.
            flags = [int(row[3]), int(row[1]), int(row[2]), int(row[4]),
                     int(row[5]), int(row[6]), int(row[7])]

            tag_name = file_name
            tag_name = tag_name[2:]
            tag_name = re.sub('-[0-9]+\.svg', '', tag_name)

            index = re.sub('\D', '', file_name)

            tree = ET.parse('../svg/' + file_name)
            title = list(tree.getroot())[0].text

            rows.append(RowData(type, tag_name, int(index), title, flags))

    table_hline = '<tr>\n<td class="td-align" colspan="10"></td>\n<tr>\n'

    table_header = \
        ('<table width="100%">\n'
         '<colgroup>\n'
         '<col width="5%">\n'
         '<col width="5%">\n'
         '<col width="60%">\n'
         '<col width="5%">\n'
         '<col width="5%">\n'
         '<col width="5%">\n'
         '<col width="5%">\n'
         '<col width="5%">\n'
         '</colgroup>\n'
         '<thead>\n'
         '<tr>\n'
         '<th></th>\n'
         '<th>#</th>\n'
         '<th>Feature</th>\n'
         '<th>resvg</th>\n'
         '<th>Chrome</th>\n'
         '<th>Firefox</th>\n'
         '<th>Batik</th>\n'
         '<th>Inkscape</th>\n'
         '<th>librsvg</th>\n'
         '<th>QtSvg</th>\n'
         '</tr>\n'
         '</thead>\n')

    html = table_header

    for elem in elements_order:
        if elem.startswith('<a '):
            html += '<tr>\n<td class="td-align" colspan="10">{}</td>\n<tr>\n'.format(elem)
        else:
            html += get_item_row(rows, out_of_scope_elems, ELEMENT_TYPE, elem)

            for row in rows:
                if row.type == ELEMENT_TYPE and row.name == elem:
                    html += gen_row(row)

            html += table_hline

    html += '</table>\n'

    with open('autogen-elements-table.html', 'w') as f:
        f.write(html)

    html = table_header

    for attr in presentation_attrs:
        if not attr.startswith('https'):
            html += get_item_row(rows, out_of_scope_pres_attrs, ATTRIBUTE_TYPE, attr)

            for row in rows:
                if row.type == ATTRIBUTE_TYPE and row.name == attr:
                    html += gen_row(row)

            html += table_hline

    html += '</table>\n'

    # collect all non presentation attributes
    attrs_order = set()
    for row in rows:
        if row.type == ATTRIBUTE_TYPE:
            if row.name not in presentation_attrs:
                attrs_order.add(row.name)

    with open('autogen-presentation-attributes-table.html', 'w') as f:
        f.write(html)

    html = table_header

    for attr in attrs_order:
        if not attr.startswith('https'):
            html += get_item_row(rows, [], ATTRIBUTE_TYPE, attr)

            for row in rows:
                if row.type == ATTRIBUTE_TYPE and row.name == attr:
                    html += gen_row(row)

            html += table_hline

    html += '</table>\n'

    with open('autogen-attributes-table.html', 'w') as f:
        f.write(html)


if __name__ == '__main__':
    main()
