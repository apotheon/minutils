# BlockSizer

At present, this is just a simple command line utility for calculating the
biggest blocksize (up to 1MB) by which you can divide a filesize without a
remainder or fractional component, and optionally tells you how many blocks
result from using that blocksize when writing to a storage device with `dd`.

The intended purpose of this tool is to help you optimize for speed when you
use `dd` to write a filesystem image to a storage device.
