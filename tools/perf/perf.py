#!/usr/bin/env python3

import argparse
import os
import subprocess
import time


def render_file(render, exe):
    if render == 'resvg-qt':
        args = [exe, in_svg, out_png, '--backend', 'qt']
    elif render == 'resvg-cairo':
        args = [exe, in_svg, out_png, '--backend', 'cairo']
    elif render == 'inkscape':
        args = [exe, in_svg, '--export-png=' + out_png]
    elif render == 'librsvg':
        args = [exe, '-f', 'png', in_svg, '-o', out_png]
    elif render == 'qtsvg':
        args = [exe, in_svg, out_png]

    res = subprocess.run(args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return res.returncode == 0


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('render', help='Render type',
                        choices=['resvg-qt', 'resvg-cairo', 'inkscape', 'librsvg', 'qtsvg'])
    parser.add_argument('exe', help='Render executable')
    parser.add_argument('in_dir', help='Input directory')
    parser.add_argument('out_dir', help='Output directory')
    args = parser.parse_args()

    if not os.path.exists(args.out_dir):
        os.mkdir(args.out_dir)

    svg_files = []
    broken_files = []

    start_time = time.time()
    for root, _, files in os.walk(args.in_dir):
        for file_name in files:
            file_stem, file_ext = os.path.splitext(file_name)
            if file_ext == '.svg':
                abs_path_in = os.path.join(root, file_name)
                abs_path_out = os.path.join(args.out_dir, file_stem + '.png')
                svg_files.append((abs_path_in, abs_path_out))

    for idx, file in enumerate(svg_files):
        print('{} out of {}'.format(idx + 1, len(svg_files)))
        in_svg, out_png = file
        if not render_file(args.render, args.exe):
            broken_files.append(in_svg)

    elapsed_time = time.time() - start_time
    print('{:.2f}sec'.format(elapsed_time))

    if broken_files:
        print('Failed:', broken_files)
