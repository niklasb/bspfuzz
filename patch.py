import shutil
import struct
import os
from subprocess import check_output

startpoint = int(check_output("nm bspfuzz | grep startpoint | cut -d' ' -f1", shell=True), 16)

shutil.copyfile('bin/dedicated.orig.so', 'bin/dedicated.so')
shutil.copyfile('bin/engine.orig.so', 'bin/engine.so')
shutil.copyfile('bin/libtier0.orig.so', 'bin/libtier0.so')

def patch(d, offset, s):
    d[offset:offset+len(s)] = s

######### engine.so
dat = bytearray(open("bin/engine.orig.so").read())

# Jump to forkserver entry point after initialization sequence is complete (enough)
patch(dat, 0x286D20,
    (
    '\xb8' + struct.pack('<I', startpoint) +  # mov eax, startpoint
    '\xff\xd0'                                # call eax
    ))
patch(dat, 0x2CCDB0, "\xc3")

# nop out call to BeginWatchdogTimer, so that we don't get SIGABRT after a while
patch(dat, 0x263F5F, "\x90"*6)

with open("bin/engine.so", "wb") as f:
  f.write(dat)

######### libtier0.so
dat = bytearray(open("bin/libtier0.orig.so").read())

# Plat_ExitPlatform should not crash
patch(dat, 0x156E7, "\x90"*(0x156F5-0x156e7))
with open("bin/libtier0.so", "wb") as f:
  f.write(dat)
