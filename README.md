# [resvg](https://github.com/RazrFalcon/resvg) test suite

## Structure

- `images/` - raster images
- `site/` - sources for github pages
- `svg/` - SVG images. Actual tests
- `png/` - PNG images with expected rendering results
- `tools/` - various testing tools
- `_config.yml`, `Gemfile`, `Rakefile` - configs required for github pages
- `check.py` - pre-commit hook
- `gen-table.py` - asciidoctor tables generator
- `init_official_test_suite.py` - download and prepare the official SVG test suite
- `official.csv` - results of manual testing via `tools/vdiff` of the official SVG test suite
- `order.txt` - custom tests order for pretty-printing
- `outline.py` - pretty-print tests
- `results.csv` - results of manual testing via `tools/vdiff` of the `resvg` test suite
- `stats.py` - generates `site/images/chart.svg`

## Requirements

Linux. Other OS'es aren't supported, but should work.

Apps and libraries:
- node.js
- Qt 5
- Inkscape
- librsvg
- Batik (Java)
- Python 3
- Ruby (for `bundler`)
- ... and the [resvg](https://github.com/RazrFalcon/resvg) itself

Fonts:
- Arial
- Times New Roman
- Source Code Pro (all weights)

## Tests order

Test names must not be changed. All new test files should increment the latest number.
But if we need to insert some test in the middle of the order - we also have to increment
all the tests after it. Which is unacceptable. To avoid this we are using `order.txt` file,
that allows us to print tests in any order we want.

## License

MIT
