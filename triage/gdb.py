import os
import signal
import sys
import select
import atexit
import time
import threading
from subprocess import Popen, PIPE
from collections import defaultdict

class Gdb(object):
    def __init__(self, binary, timeout=10):
        self.binary = binary
        self.timeout = timeout
        self.setup()
        atexit.register(self.kill)

    def setup(self):
        self.p = Popen(['gdb', self.binary], stdin=PIPE, stdout=PIPE,
                       stderr=PIPE, preexec_fn=os.setsid)
        self.pid = self.p.pid
        self.fdout = self.p.stdout.fileno()
        self.fderr = self.p.stderr.fileno()
        self.cmd('set confirm off')
        self.cmd('set width unlimited')

    def kill(self):
        try:
            # print('Killing %d' % os.getpgid(self.pid))
            sys.stdout.flush()
            os.killpg(os.getpgid(self.pid), signal.SIGKILL)
        except ProcessLookupError:
            pass

    def reset(self):
        self.kill()
        self.setup()

    def prompt(self):
        self.p.stdin.write(b'python __import__("sys").stdout.write("__MARKER__\\n");__import__("sys").stdout.flush()\n')
        self.p.stdin.write(b'python __import__("sys").stderr.write("__MARKER__\\n");__import__("sys").stderr.flush()\n')
        self.p.stdin.flush()
        out = b''
        err = b''
        t0 = time.time()
        while not b'__MARKER__' in out or not b'__MARKER__' in err:
            if time.time() > t0 + self.timeout:
                raise TimeoutError()
            ready,_,_ = select.select([self.fdout, self.fderr], [], [], self.timeout)
            if not ready:
                raise TimeoutError()
            if self.fdout in ready:
                c = os.read(self.fdout, 1024)
                assert c
                out += c
            if self.fderr in ready:
                c = os.read(self.fderr, 1024)
                assert c
                err += c
        self.p.stdin.write(b'python __import__("sys").stdout.write("__MARKER2__\\n");__import__("sys").stdout.flush()\n')
        self.p.stdin.flush()
        while not b'__MARKER2__' in out:
            c = os.read(self.fdout, 1024)
            out += c
        while not out.endswith(b'(gdb) '):
            c = os.read(self.fdout, 1024)
            out += c
        out = out.rsplit(b'\n(gdb) __MARKER__', 1)[0]
        err = err.rsplit(b'__MARKER__', 1)[0]
        return out, err

    def cmd(self, cmd):
        self.p.stdin.write(cmd.encode('utf-8') + b'\n')
        self.p.stdin.flush()
        return self.prompt()


class GdbPool(object):
    def __init__(self, timeout):
        self.idle = defaultdict(list)
        self.timeout = timeout
        self.mx = threading.Lock()

    def clean(self):
        cleaned = defaultdict(list)
        for k, v in self.idle.items():
            for since, gdb in v:
                if since + 120 >= time.time():
                    cleaned[k].append((since, gdb))
                else:
                    gdb.kill()
        self.idle = cleaned

    def get(self, binary):
        assert os.path.exists(binary)
        with self.mx:
            available = self.idle[binary]
            if not available:
                return Gdb(binary, timeout=self.timeout)
            res = available.pop()[1]
            self.clean()
            return res

    def put(self, gdb):
        with self.mx:
            self.clean()
            self.idle[gdb.binary].append((time.time(), gdb))


class FromGdbPool(object):
    def __init__(self, pool, binary):
        self.pool = pool
        self.binary = binary

    def __enter__(self):
        self.instance = self.pool.get(self.binary)
        return self.instance

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.pool.put(self.instance)
