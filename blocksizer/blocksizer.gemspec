Gem::Specification.new do |s|
  s.name            =  'blocksizer'
  s.version         =  '0.1'
  s.authors         =  ['Chad Perrin']
  s.date            =  '2017-07-06'
  s.description     =  <<-EOF
    Blocksizer is a simple tool designed to determine an optimized blocksize to
    specify when using dd to write filesystem image files to USB storage
    devices, such as when creating a bootable USB installer for your favorite
    BSD Unix operating system.  Note that specifying blocksize does not seem to
    have much effect on modern Linux systems, but can result in significant
    speedup for write time on typical BSD Unix systems.

    Blocksizer can also, optionally, provide a count of blocks that will be
    written using that blocksize, which can be useful for verifying the write
    operation completed as expected.  Most people do not verify the integrity
    of the written filesystem image, so this feature is probably going to be
    almost universally ignored.
  EOF
  s.summary         =  'Blocksizer - determine optimal blocksize for dd'
  s.email           =  'code@apotheon.net'
  s.files           =  [
    'LICENSE',
    'README.md',
    'bin/blocksizer',
  ]
  s.homepage        =  'http://fossrec.com/u/apotheon/minutils/blocksizer'
  s.has_rdoc        =  true
  s.license         =  'COIL'
  s.bindir          =  'bin'
  s.executables     =  ['blocksizer']

  s.post_install_message    = <<-EOF
    Thank you for using blocksizer.  Usage help for the "blocksizer" command
    line utility is a "blocksizer help" command away.
  EOF

  s.required_ruby_version   = '>= 1.9.3'
end
