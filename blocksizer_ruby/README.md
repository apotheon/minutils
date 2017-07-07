# BlockSizer

At present, this is just a simple command line utility for calculating the
biggest blocksize (up to 1MB) by which you can divide a filesize without a
remainder or fractional component, and optionally tells you how many blocks
result from using that blocksize when writing to a storage device with `dd`.
