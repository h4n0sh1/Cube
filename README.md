# 3D Formula Renderer

A minimal 3D graphics renderer in pure C, inspired by the great [Alexey Kutepov (Tsoding)](https://github.com/tsoding)'s ["One Formula That Demystifies 3D Graphics"](https://github.com/tsoding/formula) video.

## Features

- Pure C, cross-platform (Windows/Mac/Linux)
- Single-header PNG output via [stb_image_write](https://github.com/nothings/stb)
- Software rendering - no GPU dependencies

## Build

```bash
gcc main.c -o cube.exe -O2
./cube.exe
```

Outputs `output.png`.

## Credits

- Original inspiration: [tsoding/formula](https://github.com/tsoding/formula) by Alexey Kutepov
- PNG writing: [nothings/stb](https://github.com/nothings/stb)
