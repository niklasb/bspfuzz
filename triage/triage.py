#!/usr/bin/env python3
import argparse
import gdb
import glob
import hashlib
import os
import random
import shutil
import sys
import tempfile
from subprocess import Popen, check_output

ROOTDIR = os.path.dirname(os.path.abspath(__file__))
FUZZDIR = ROOTDIR + '/..'
FUZZBIN = FUZZDIR + '/bspfuzz'

def bugid(crash):
    ''' tries to create a hash describing the crash '''
    res = hashlib.md5('\n'.join(crash['backtrace'][:5]).encode('utf-8')).hexdigest()[:8]
    if crash['heuristic'] != 'unknown':
        res += '_' + crash['heuristic']
    for x in crash['backtrace']:
        if '.so' in x:
            res += '_' + x
            return res
    return res

def process_bt(bt):
    better_bt = []
    modcache = {}
    for x in bt:
        addr = x.split()[1]
        if '0x' in addr:
            addr = int(x.split()[1], 16)
            if ' from ' in x:
                mod = x.split(' from ')[1].split()[0]
                mod = mod.split('/')[-1] # basename
                if mod not in modcache:
                    try:
                        out, err = g.cmd('print $base("%s")' % mod)
                    except:
                        raise Exception("You need the base() function from https://github.com/niklasb/gdbinit")
                    if b'No entry found' not in err:
                        base = (out.decode('utf-8')
                                    .strip().split(' = ')[1])
                        if '0x' in base:
                            base = int(base, 16)
                        else:
                            base = int(base, 10)
                        modcache[mod] = base
                if mod in modcache:
                    addr = '%s+0x%x' % (mod.split('/')[-1], addr-modcache[mod])
                else:
                    addr = '0x%08x' % addr
            else:
                addr = '0x%08x' % addr
        better_bt.append(addr)
    return better_bt

def evaluate_impl(bspfile):
    target = tmpdir + '/test.bsp'
    shutil.copy(bspfile, target)
    res,_ = g.cmd("run '" + target + "'")
    if b'SIGSEGV' not in res:
        return None

    fault,_ = g.cmd("print $_siginfo._sifields._sigfault.si_addr")
    fault = 'segfault @ ' + (fault.decode('utf-8').split('=', 1)[1]
                .replace('(void *)', '').replace('(void*)', '').strip())
    assert '0x' in fault
    fault_addr = int(fault.split(' @ ')[1].strip(), 16)

    eip = int(g.cmd('info register eip')[0].decode('utf-8').split()[1], 16)
    bt = g.cmd('backtrace')[0].decode('utf-8').splitlines()
    bt = process_bt(bt)

    ctx = (g.cmd('x/3i $eip')[0].decode('utf-8') + '\n'
        + g.cmd('info registers')[0].decode('utf-8') + '\n')
    ins = ctx.splitlines()[0]

    heuristic = 'unknown'
    if fault_addr < 0x100000:
        heuristic = 'nullptr'
    elif ']' in ins and ',' in ins.split(']')[1]:
        heuristic = 'write'
    elif '[' in ins and ',' in ins.split('[')[0]:
        heuristic = 'read'
    return {"fault": fault, 'backtrace': bt, 'eip': eip, 'context': ctx,
            'heuristic': heuristic}

def evaluate(f):
    try:
        return evaluate_impl(f)
    except TimeoutError:
        print('    Timeout!')
        g.reset()
        return None

files = []
for f in sys.argv[1:]:
    if os.path.isfile(f):
        files.append(os.path.abspath(f))
    elif os.path.isdir(f):
        files += map(os.path.abspath, glob.glob(f + '/*'))

print("Found %d files" % len(files))

tmpdir = tempfile.mkdtemp()
def cleanup():
    shutil.rmtree(tmpdir)

os.chdir(FUZZDIR)

g = gdb.Gdb(FUZZBIN, timeout=10)
g.setup()

todo = []
for f in files:
    h = check_output(['sha1sum', f]).split(b' ')[0].decode('utf-8')
    crashname = ROOTDIR + '/crashes/' + h + '.bsp'
    if not os.path.exists(crashname):
        todo.append((f, h))
print("TODO %d files" % len(todo))

random.shuffle(todo)

cnt = 0
for fname, h in todo:
    cnt += 1
    crashname = ROOTDIR + '/crashes/' + h + '.bsp'
    if os.path.exists(crashname):
        print('    Skipped')
        continue

    print(" [%d/%d] Processing %s / %s" % (cnt, len(todo), fname, h))
    for _ in range(1):
        crash = evaluate(fname)
        if crash is not None:
            break
        print('    retrying...')

    if not crash:
        print('    => Could not reproduce!')
        continue

    id = bugid(crash)

    odir = ROOTDIR + '/bugid/' + id
    try:
        os.makedirs(odir)
        print('    => new bug id: %s' % id)
    except FileExistsError:
        pass

    with open(odir + '/' + h + '.gdb.txt', 'w') as f:
        f.write(crash['context'])

    if os.path.exists(crashname):
        print('    Skipped')
        continue
    os.symlink(fname, crashname)
