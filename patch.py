#!/usr/bin/env python3
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
dat = bytearray(open("bin/engine.orig.so", "rb").read())

# Jump to forkserver entry point after initialization.
# 0x286d20 is the NET_CloseAllSockets function.
patch(dat, 0x29E6B0,
(
  b'\xb8' + struct.pack('<I', startpoint) +  # mov eax, startpoint
  b'\xff\xd0'                                # call eax
))

# Patch out a function that is registered via atexit().
# You can find it by looking for the single xref to the string
# "Missing shutdown function for %s"
patch(dat, 0x2E5900, b"\xc3")

# nop out a call to BeginWatchdogTimer in the initialization sequence, so that we
# don't get SIGABRT after a while
patch(dat, 0x27459F, b"\x90"*6)

with open("bin/engine.so", "wb") as f:
  f.write(dat)

######### libtier0.so
dat = bytearray(open("bin/libtier0.orig.so", "rb").read())

# Avoid Plat_ExitPlatform crash
# .text:000156E7                 test    ebx, ebx
# .text:000156E9                 jz      short loc_156F5
# .text:000156EB                 mov     ds:dword_0, 1
patch(dat, 0x15877, b"\x90"*(0x15885 - 0x15877))
with open("bin/libtier0.so", "wb") as f:
  f.write(dat)
