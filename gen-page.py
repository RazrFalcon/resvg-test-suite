#!/usr/bin/env python3

import subprocess

subprocess.check_call(['python3', 'gen-html-tables.py'])

subprocess.check_call(['asciidoctor', '--no-header-footer', '-o', 'site/svg-support-table-raw.html',
                       'site/svg-support-table.adoc'])

with open('site/layout.html', 'r') as f:
    layout = f.read()

    with open('site/style.css', 'r') as f2:
        layout = layout.replace('{{page.style}}', f2.read())

    with open('site/svg-support-table-raw.html', 'r') as f2:
        layout = layout.replace('{{page.data}}', f2.read())

    with open('site/svg-support-table.html', 'w') as f2:
        f2.write(layout)
