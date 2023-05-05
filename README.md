# [resvg](https://github.com/RazrFalcon/resvg) test suite

## Structure

- `tests/` - SVG images. Actual tests
- `resources/` - files that will be referenced by SVG files
- `fonts/` - fonts used by SVG files
- `site/` - sources for github pages
- `tools/` - various testing tools
- `check.py` - pre-commit hook
- `results.csv` - results of manual testing via `tools/vdiff` of the `resvg` test suite
- `stats.py` - generates `site/images/chart.svg`

## Tests order

Test names must not be changed. All new test files should increment the latest number.
But if we need to insert some test in the middle of the order - we also have to increment
all the tests after it. Which is unacceptable. To avoid this we are using `order.txt` file,
that allows us to print tests in any order we want.

## License

MIT
