## Perf

Let's say we want to check `resvg` performance:

```
./perf.py resvg-qt ../../../resvg/target/release/rendersvg /svg_path /tmp_dir
```

Chart can be created via [barh](https://github.com/RazrFalcon/barh):

```
barh oxygen.json ../../site/images/perf.svg
```
