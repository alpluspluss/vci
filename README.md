<div align="center">

vci
====

### Video Console Interface

vci is a high performance video-to-ASCII converter using FFmpeg and SIMD optimizations.
Renders videos as ASCII art in your terminal with support for aspect ratio preservation
and optional colored output.

</div>

> [!WARNING]
> vci is currently supported on Unix-based operating system

## Examples

TBA

## Dependencies

- FFmpeg

## Building

```shell
#!/usr/bin/env sh

mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Installation

```shell
#!/usr/bin/env sh

# After building...
# In build/
sudo make install
```

## Usage

```shell
vci <video_file>
```

## License

This project is under the MIT license. See [License](LICENSE.txt) for more information.

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.