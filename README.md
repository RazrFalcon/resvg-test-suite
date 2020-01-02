# [resvg](https://github.com/RazrFalcon/resvg) test suite

## Structure

- `svg/` - SVG images. Actual tests
- `png/` - PNG images with expected rendering results
- `images/` - images that will be referenced by SVG files
- `site/` - sources for github pages
- `tools/` - various testing tools
- `check.py` - pre-commit hook
- `init_official_test_suite.py` - download and prepare the official SVG test suite (optional)
- `gh-pages.sh` - generates github pages
- `official.csv` - results of manual testing via `tools/vdiff` of the official SVG test suite
- `order.txt` - custom tests order for pretty-printing
- `results.csv` - results of manual testing via `tools/vdiff` of the `resvg` test suite
- `stats.py` - generates `site/images/chart.svg`

## Required fonts

- Arial
- Times New Roman
- [Source Code Pro](https://fonts.google.com/specimen/Source+Code+Pro) (all weights)
- Any font with emojis, like [Noto Color Emoji](https://www.google.com/get/noto/help/emoji/)

## Tests order

Test names must not be changed. All new test files should increment the latest number.
But if we need to insert some test in the middle of the order - we also have to increment
all the tests after it. Which is unacceptable. To avoid this we are using `order.txt` file,
that allows us to print tests in any order we want.

## License

MIT
