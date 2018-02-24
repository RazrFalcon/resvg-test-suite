[`resvg`](https://github.com/RazrFalcon/resvg) Test Suite.

## Notes

- We doesn't test basic shapes rendering with different styles
  because they will be converted to paths anyway. So we can test only them.
- `stroke` attribute tests doesn't duplicate color parsing tests form the `fill` attribute tests.
  They should be processed in the same way.
