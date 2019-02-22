#!/usr/bin/env python3

import subprocess
import base64
import sys

MIN_PYTHON = (3, 6)
if sys.version_info < MIN_PYTHON:
    sys.exit("Python %s.%s or later is required.\n" % MIN_PYTHON)

subprocess.check_call(['python3', 'gen-html-tables.py'])

subprocess.check_call(['asciidoctor', '--no-header-footer', '-o', 'svg-support-table-raw.html',
                       'svg-support-table.adoc'])

with open('layout.html', 'r') as f:
    layout = f.read()

    with open('style.css', 'r') as f2:
        layout = layout.replace('{{page.style}}', f2.read())

    with open('svg-support-table-raw.html', 'r') as f2:
        layout = layout.replace('{{page.data}}', f2.read())

    with open('favicon.ico', 'rb') as f2:
        layout = layout.replace('{{page.favicon}}', base64.b64encode(f2.read()).hex())

    with open('images/chart.svg', 'r') as f2:
        layout = layout.replace('<img src="images/chart.svg" alt="chart">', f2.read())

    with open('svg-support-table.html', 'w') as f2:
        f2.write(layout)
