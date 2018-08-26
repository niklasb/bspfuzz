#!/usr/bin/env python2
import sys
from struct import *

'''
struct lump_t
{
  int  fileofs;  // offset into file (bytes)
  int  filelen;  // length of lump (bytes)
  int  version;  // lump format version
  char  fourCC[4];  // lump ident code
};

#define  HEADER_LUMPS    64

struct dheader_t
{
  int  ident;                // BSP file identifier
  int  version;              // BSP file version
  lump_t  lumps[HEADER_LUMPS];  // lump directory array
  int  mapRevision;          // the map's revision (iteration, version) number
};
'''

def shorten(i, dat):
    if i in (40,53,8):
        return ''
    if i == 7:
        assert len(dat)%0x38 == 0
        return dat[:len(dat)//0x380*0x38]
    # if i == 29:
        # return dat[:len(dat)//0x10]
    if i == 13:
        assert len(dat)%4 == 0
        return dat[:len(dat)//0x40*0x4]
    if i == 10:
        assert len(dat)%0x20 == 0
        return dat[:len(dat)//0x200*0x20]
    if i == 3:
        assert len(dat)%0xc == 0
        return dat[:len(dat)//0xc0*0xc]
    if i == 5:
        assert len(dat)%0x20 == 0
        return dat[:len(dat)//0x200*0x20]
    return dat

dat = bytearray(open(sys.argv[1]).read())
print len(dat)

lumpdat = [None]*64
for i in range(64):
    fileofs, filelen = unpack("<II", dat[8 + i*16:8 + i*16 + 8])
    print i, fileofs, filelen

    assert fileofs != 0 or filelen == 0

    if fileofs != 0:
        lumpdat[i] = shorten(i, dat[fileofs:fileofs + filelen])

offset = 1036
sizes = []
for i in range(64):
    if lumpdat[i] is None:
        continue
    ldat = lumpdat[i]
    dat[offset:offset + len(ldat)] = ldat
    dat[8 + i*16:8 + i*16 + 8] = pack("<II", offset, len(ldat))
    offset += len(ldat)
    sizes.append((len(ldat), i))

sizes.sort(reverse=True)
for sz, lump in sizes[:10]:
    print 'lump=%d size=%d' % (lump, sz)

dat = dat[:offset]
with open(sys.argv[2], 'wb') as f:
    f.write(dat)
print len(dat)
