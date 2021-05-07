
from dataclasses import dataclass
import queue
import subprocess
import sys
import threading
import time
from typing import *


class Logger:
    """ logs information to stdout. this class is not thread-safe. """

    @staticmethod
    def echo(msg: str):
        print('    ::: %s' % msg)

    @staticmethod
    def sys(msg: str):
        print('  [SYS] %s' % msg)

    @staticmethod
    def out(proc: str, msg: str):
        print((' %s | ' % proc).rjust(8, ' ') + msg)

    @staticmethod
    def err(proc: str, msg: str):
        print(('<%s> | ' % proc).rjust(8, ' ') + msg)

    @staticmethod
    def inp(*args) -> Tuple[str, str]:
        """ inp(), inp(proc), inp(proc, msg) """
        if len(args) == 0:
            print('------> ', end='')
            proc, *msg = input().split(' ')
            return proc, ' '.join(msg)
        elif len(args) == 1:
            proc = args[0]
            print('------> %s $ ' % proc, end='')
            return proc, input()
        proc, msg = args[0], args[1]
        print('------> %s $ %s' % (proc, msg))
        return proc, msg
    pass


class MultiTerm:
    ProcKey = str
    ProcStartParam = List[str]
    ProcThread = Any
    ProcThreadDict = Dict[ProcKey, ProcThread]
    Message = Union[str, None]
    MessageQueue = queue.Queue   # [Message]
    PrintMsg = Tuple[ProcKey, str]
    PrintMsgQueue = queue.Queue  # [Union[PrintMsg, None]]
    Schedule = Tuple[float, str, Union[Tuple[Any], str]]
    ScheduleQueue = queue.Queue  # [Union[Schedule, None]]

    def _glob_out_worker(self, queue, func):
        """ output worker that listens for the [self._qstd***] message queue,
        and carries messages over to [Logger]. """
        while True:
            pmsg = queue.get(True)
            if pmsg is None:
                break
            proc, msg = pmsg
            func(proc, msg)
        return

    def _glob_sched_worker(self):
        """ reads scheduled (Logger.sys / _start) events and executes. """
        while True:
            smsg = self._schqueue.get(True)
            if smsg is None:
                break
            wakeup, cmd, data = smsg
            delay = wakeup - time.time()
            if delay > 0.0:
                time.sleep(delay)
            if cmd == 'start':
                self._start(*data)
            elif cmd == 'echo':
                Logger.echo(data[0])
            elif cmd == 'push':
                self._push(*data)
            elif cmd == 'kill':
                self._kill()
            pass
        return

    def __init__(self):
        self._keys: Set[ProcKey] = set()
        self._th_stdin: ProcThreadDict = {}
        self._th_stdout: ProcThreadDict = {}
        self._th_stderr: ProcThreadDict = {}
        self._mqueue: Dict[ProcKey, MessageQueue] = {}
        self._qstdout: PrintMsgQueue = queue.Queue()
        self._qstderr: PrintMsgQueue = queue.Queue()
        self._schqueue: ScheduleQueue = queue.Queue()
        # start output listeners
        self._th_glob_out = threading.Thread(
            target=self._glob_out_worker,
            args=[self._qstdout, Logger.out])
        self._th_glob_err = threading.Thread(
            target=self._glob_out_worker,
            args=[self._qstderr, Logger.err])
        self._th_glob_out.start()
        self._th_glob_err.start()
        # start schedule listeners
        self._th_glob_sched = threading.Thread(
            target=self._glob_sched_worker)
        self._th_glob_sched.start()
        # join lock, use this to listen for termination
        self._join_lock = threading.Lock()
        self._join_lock.acquire()
        return

    def _push(self, proc: ProcKey, command: Message):
        """ push [command] to [proc] """
        self._mqueue[proc].put(command)
        return

    def push(self, proc: ProcKey, delay: float, command: Message):
        """ push [command] to [proc] after [delay] seconds from now """
        self._schqueue.put((time.time() + delay, 'push', (proc, command)))

    def _in_worker(self, proc: ProcKey, handle, pipe):
        """ carries messages that were in the [mqueue] into the process [pipe].
        if a message was `None`, this worker should stop carrying messages
        immediately. terminate process and cleanup thereupon. after termination
        worker would retain a thread handle in [self._th_stdin]. """
        while True:
            line = self._mqueue[proc].get(True)
            if line is None:
                break
            if handle.poll() is not None:
                break
            Logger.inp(proc, line)
            pipe.write(('%s\n' % line).encode('utf-8', 'ignore'))
            pipe.flush()
        # terminate process and cleanup
        handle.terminate()
        handle.wait()
        handle.stdout.close()
        handle.stderr.close()
        # remove [proc] from [keys]. this is used for output threads to
        # determine if the process is still running.
        self._keys.remove(proc)
        del self._mqueue[proc]
        self._th_stdout[proc].join()
        self._th_stderr[proc].join()
        del self._th_stdout[proc]
        del self._th_stderr[proc]
        # get return code and debug
        retcode = handle.returncode
        Logger.sys("process '%s' terminated with code %d" % (proc, retcode))
        return

    def _out_worker(self, proc: ProcKey, pipe, handle, queue):
        """ stdout / stderr worker which carries output to the message queue.
        worker halts upon process termination. empty lines will be dropped. """
        while handle.poll() is None:
            line = pipe.readline().decode('utf-8', 'ignore').strip('\r\n')
            if handle.poll() is not None:
                break
            if not line:
                continue
            queue.put((proc, line))
        return

    def _start(self, proc: ProcKey, params: ProcStartParam) -> None:
        """ initiate process threads and start running rightaway. [params] is
        a list of strings which represent its command line arguments. """
        if proc in self._keys:
            raise KeyError(proc)
        if len(proc) > 4:
            raise ValueError('process keys must be less than 4 chars')
        self._keys.add(proc)
        self._mqueue[proc] = queue.Queue()
        # create process handle
        handle = subprocess.Popen(
            list(params),
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        Logger.sys("started process '%s'" % proc)
        # start threads
        self._th_stdin[proc] = threading.Thread(
            target=self._in_worker,
            args=[proc, handle, handle.stdin])
        self._th_stdout[proc] = threading.Thread(
            target=self._out_worker,
            args=[proc, handle.stdout, handle, self._qstdout])
        self._th_stderr[proc] = threading.Thread(
            target=self._out_worker,
            args=[proc, handle.stderr, handle, self._qstderr])
        self._th_stdin[proc].start()
        self._th_stdout[proc].start()
        self._th_stderr[proc].start()
        return

    def start(self, proc: ProcKey, delay: float, params: ProcStartParam):
        """ schedule start with cmdline params. this asynchronously calls
        _start(proc, params). """
        self._schqueue.put((time.time() + delay, 'start', (proc, params)))
        return

    def start_cmd(self, proc: ProcKey, delay: float, cmdline: str):
        """ start with command line string. same as start(..) """
        params = list(filter(lambda x: x, cmdline.split(' ')))
        self.start(proc, delay, params)
        return

    def echo(self, delay: float, msg: str):
        self._schqueue.put((time.time() + delay, 'echo', (msg, )))
        return

    def _kill(self):
        """ kill all processes by sending termination messages to mqueues,
        this function is non-blocking """
        curtm = time.time()
        for proc in self._mqueue:
            que = self._mqueue[proc]
            que.put(None)
        return

    def kill(self, delay: float):
        """ schedule kill at [delay] seconds from now """
        self._schqueue.put((time.time() + delay, 'kill', ()))
        return

    def join(self):
        """ await all started processes and i/o thread managers """
        self._schqueue.put(None)
        self._th_glob_sched.join()
        for key in self._th_stdin:
            self._th_stdin[key].join()
        self._th_stdin.clear()
        self._qstdout.put(None)
        self._qstderr.put(None)
        self._th_glob_out.join()
        self._th_glob_err.join()
        Logger.sys('all processes terminated')
        self._join_lock.release()
        return
    pass


class MTSLParser:
    """ MTSL: MultiTerm Shell Language
    Performs MultiTerm commands with a shell script. MultiTerm commands are
    case-sensitive. Comments start with character '#', and there are no options
    to escape that character.

      - $echo <message>
            sends message to logger output only
      - $start <process> <command line>
            process: a 4-character string identifier used to identify which
                process produced an stdout / stderr output (or input)
            command line: arguments like you would use in a shell
            all processes are started at begin of script
      - <time> <process> <input>
            process: the aforementioned identifier
            input: send input through stdin into that process
      - $input
            read "<process> <input>" from user dynamically to send to process
      - $input <process>
            read "<input>" from user dynamically to send to that process
      - $kill <time> <process>
            kill process
      - $end <time>

    The parameter <time> could be an time offset from the beginning, or could
    also be an offset from the last MTSL command. More specifically:

      - "+<time>" => execute that command after <time> seconds from last
      - "-" => execute that command immediately after last (same as "+0.0")
      - "@<time>" => execute after <time> seconds from beginning. note that
            this event shall not be earlier than the last command

    The MTSL can be understood by reading a program sample from below:

    $echo   -               start process 'w1' with command line args
    $start  -       w1      python3 test.py 3
    $echo   +0.5            start process 'w2' similarly
    $start  -       w2      python3 test.py 4
    $echo   +1.0            send line 'teststring' to process w1 after 1 second
            -       w1      teststring
    $echo   @2.0            send line 'other' to process w2 after 2 seconds
            -       w2      other
    $echo   @3.0            kill process w1 at the 3.0-th second
    $kill   -       w1
    $echo   +0.2            kill process w2 0.2 secs after w1 kill
    $kill   -       w2
    $echo   +0              kill all processes immediately
    $end    -

    The output from the previous sample is as:

        ::: start process 'w1' with command line args
      [SYS] started process 'w1'
        ::: start process 'w2' similarly
      [SYS] started process 'w2'
        ::: send line 'teststring' to process w1 after 1 second
    ------> w1 $ teststring
       w1 | teststring teststring teststring
        ::: send line 'other' to process w2 after 2 seconds
    ------> w2 $ other
       w2 | other other other other
        ::: kill process w1 at the 3.0-th second
      [SYS] process 'w1' terminated with code 1
        ::: kill process w2 0.2 secs after w1 kill
        ::: kill all processes immediately
      [SYS] process 'w2' terminated with code 1
      [SYS] all processes terminated

    You may provide MTSL commands in a file to read from. """

    def __init__(self):
        self._timestamp = 0.0
        self._mtinstance = MultiTerm()
        return

    def _remove_first(self, s):
        """ take out the first argument and keeping the rest of the argument,
        spaces within [s] other than the first argument are not stripped. """
        s = s.lstrip(' ').split(' ')
        return s[0], ' '.join(s[1:]).lstrip(' ')

    def _resolve_time(self, tm):
        """ resolve [tm] to (time relative) to start of parser """
        if len(tm) < 1:
            raise ValueError('bad time format')
        # '-' is equivalent to '+0.0'
        if tm[0] == '-':
            tm = '+0.0'
        # resolve '+?' and '@?'
        t = float(tm[1:])
        if tm[0] == '+':
            t += self._timestamp
        # this timestamp is earlier than the last one
        if t < self._timestamp - 1.0e-5:
            raise ValueError('time not sequential')
        self._timestamp = t
        return t

    def _user_input(self, args):
        while True:
            try:
                proc, msg = Logger.inp(*args)
                self._mtinstance.push(proc, 0.0, msg)
            except KeyboardInterrupt:
                break
        return

    def push(self, msg):
        """ translates mtsl to MultiTerm instance """
        # strip comments
        msg = msg.split('#')[0].strip()
        if not msg:
            return
        # parse command
        rawargs = list(filter(lambda x: x, msg.strip().split(' ')))
        cmd, msg = self._remove_first(msg)
        # checks for commands
        if cmd == '$start':
            tm, msg = self._remove_first(msg)
            tm = self._resolve_time(tm)
            proc, msg = self._remove_first(msg)
            self._mtinstance.start_cmd(proc, tm, msg)
        elif cmd == '$input':
            tm = self._resolve_time(rawargs[1])
            args = rawargs[1:][:1]
            self._user_input(args)
        elif cmd == '$kill':
            tm, proc = self._resolve_time(rawargs[1]), rawargs[2]
            self._mtinstance.push(proc, tm, None)
        elif cmd == '$end':
            tm = self._resolve_time(rawargs[1])
            self._mtinstance.kill(tm)
        elif cmd == '$echo':
            tm, msg = self._remove_first(msg)
            tm = self._resolve_time(tm)
            self._mtinstance.echo(tm, msg)
        else:
            tm = self._resolve_time(cmd)
            proc, msg = self._remove_first(msg)
            self._mtinstance.push(proc, tm, msg)
        return

    def join(self):
        self._mtinstance.join()
        return
    pass


def main(argv):
    if len(argv) != 2:
        print('usage: python multiterm.py [script.txt]')
        print('multiterm: fatal error: no input files.')
        print('execution terminated.')
        return 1
    mtslp = MTSLParser()
    with open(argv[1], 'r', encoding='utf-8') as f:
        ls = f.readlines()
        for l in ls:
            mtslp.push(l)
    mtslp.join()
    return 0


if __name__ == '__main__':
    exit(main(sys.argv))
