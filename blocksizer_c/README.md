# BlockSizer

At present, this is just a simple command line utility for calculating the
biggest blocksize (up to 1MB) by which you can divide a filesize without a
remainder or fractional component, and optionally tells you how many blocks
result from using that blocksize when writing to a storage device with `dd`.

The intended purpose of this tool is to help you optimize for speed when you
use `dd` to write a filesystem image to a storage device.

## Copyright

Copyright 2017 Chad Perrin

You may borrow, imitate, share, steal, fold, spindle, or mutilate this work,
desecrate it, worship its graven image, or use it as a doorstop, as you desire,
under the terms of the COIL.  See the LICENSE file or comments in the source
code file `blocksizer.c` for details about this license.
