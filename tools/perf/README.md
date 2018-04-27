## Perf

Let's say we want to check `resvg` performance:

```
./perf.py resvg-qt ../../../resvg/target/release/rendersvg /svg_path /tmp_dir
```

Chart can be created via [barh](https://github.com/RazrFalcon/barh):

```
barh oxygen.json ../../site/images/perf.svg
```

## Details

- Oxygen Icon Theme 4.12
- Contains 4947 files.
- All images are converted from `.svgz` to `.svg`.
- In the `resvg` >80% of the time is spent during PNG generation.
- The `librsvg` is slower than `resvg` because those icons are using Gaussian blur heavily, which is expensive.
- QtSvg is suspiciously slow for no reasons. Source code can be found
  [here](https://github.com/RazrFalcon/resvg-test-suite/tree/master/tools/qtsvgrender).
- Batik is used with icedtea-bin 3.7.0.
