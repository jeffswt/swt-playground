
from dataclasses import dataclass
import dns.resolver
import graphviz
import os
from prettytable import PrettyTable
import re
import sqlite3
import socket
import subprocess
import sys
import threading
from typing import List, Tuple, Union


def exec_process(args):
    # process arguments, windows crap
    if sys.platform.startswith('win'):
        CREATE_NO_WINDOW = 0x08000000
        platform_subprocess_flags = CREATE_NO_WINDOW
    else:
        platform_subprocess_flags = 0
    # start process and communicate
    proc = subprocess.Popen([str(i) for i in args],
        creationflags=platform_subprocess_flags,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE)
    stdout, stderr = proc.communicate()
    ret = proc.wait()
    return stdout.decode('utf-8')


@dataclass
class TraceEntry:
    hop: int
    delay: float
    router_ip: str


class Kumoko:
    def __init__(self, max_threads=8):
        # database related
        self._io_lock = threading.Lock()
        self._db_filename = './kumoko.db'
        if not os.path.exists(self._db_filename):
            self._init_db()
        # logging
        self._stdout_lock = threading.Lock()
        # concurrency
        self._th_lock = threading.Lock()
        self._th_semaphore = threading.Semaphore(max_threads)
        self._th_list = []  # thread lists

    def _init_db(self):
        """ _init_db() -- first time table creation """
        db = sqlite3.connect(self._db_filename)
        cur = db.cursor()
        cur.execute(""" CREATE TABLE TraceEntries (
            run_id      INTEGER  NOT NULL,
            hop         INTEGER  NOT NULL,
            delay       FLOAT    NOT NULL,
            router_ip   TEXT     NOT NULL
        ); """)
        cur.execute(""" CREATE TABLE Traces (
            run_id      INTEGER  NOT NULL,
            desc        TEXT     NOT NULL,
            src_ip      TEXT     NOT NULL,
            src_host    TEXT     NOT NULL,
            dst_ip      TEXT     NOT NULL,
            dst_host    TEXT     NOT NULL
        ); """)
        cur.close()
        db.commit()
        db.close()

    def log(self, note: str, fmt: str, *args):
        """ log(error_mode, format_string, *args) """
        result = fmt % tuple(str(i) for i in args)
        self._stdout_lock.acquire()
        print(('[%s]' % note).ljust(9, ' '), result)
        self._stdout_lock.release()

    def _save_traces(self, desc: str, src_ip: str, src_host: str, dst_ip: str,
                     dst_host: str, traces: List[TraceEntry]) -> None:
        """ _save_traces -- commit traced route to database
        @note: this part is thread safe. """
        self._io_lock.acquire()
        # retrieve run id
        db = sqlite3.connect(self._db_filename)
        cur = db.cursor()
        cur.execute('SELECT MAX(run_id) FROM Traces;')
        run_id = cur.fetchone()[0]
        run_id = 1 if run_id is None else run_id + 1
        # add main entry
        cur.execute(""" INSERT INTO Traces (run_id, desc, src_ip, src_host,
            dst_ip, dst_host) VALUES (?, ?, ?, ?, ?, ?); """,
            (run_id, desc, src_ip, src_host, dst_ip, dst_host))
        # save traces
        for trace in traces:
            cur.execute(""" INSERT INTO TraceEntries
                (run_id, hop, delay, router_ip)
                VALUES (?, ?, ?, ?); """,
                (run_id, trace.hop, trace.delay, trace.router_ip))
        cur.close()
        db.commit()
        db.close()
        self._io_lock.release()

    def _perform_local_trace_win(self, dst_ip: str, max_hops: int,
                                 max_delay: float) -> List[TraceEntry]:
        # Using Windows `tracert`
        stdout = exec_process([
            'tracert',
            '-h', max_hops,  # maximum hops traced
            '-w', str(int(max_delay * 1000)),  # maximum allowed delay
            '-d',  # do not resolve hostnames
            dst_ip,  # can also be host, anyway
        ])
        # filter hop lines
        valid_lines = list(filter(
            lambda x: x,  # drop empty lines
            [line.strip() for line in stdout.split('\n')][3:]
        ))[:-1]  # 'Trace complete.'
        # render each hop
        traces = []
        for line in valid_lines:
            # parse hop
            words = line.split()
            hop = int(words[0])
            # parse delay msecs
            router_ip = ''
            delays = []
            for word in words[1:]:
                if word == 'ms':
                    continue
                if word == '*':
                    delays.append(None)
                    continue
                if len(delays) < 3:  # guaranteed 3 tries
                    delays.append(int(word))
                else:
                    router_ip = word if '.' in word else ''
                    break
            delays = list(filter(lambda x: x, delays))
            # ensure this is a valid hop
            if len(delays) == 0 or not router_ip:
                continue
            delay = sum(delays) / len(delays) / 1000
            # put result
            traces.append(TraceEntry(
                hop=hop,
                delay=delay,
                router_ip=router_ip,
            ))
        return traces

    def _perform_local_ping_win(self, dst_ip: str, max_delay: float
                                ) -> Union[None, float]:
        """ _perform_local_ping_win() """
        stdout = exec_process([
            'ping',
            '-n', '3',  # test thrice
            '-l', '64',  # send 64 bytes of ping pkg
            '-w', str(int(max_delay * 1000)),  # maximum allowed delay
            dst_ip,  # can also be host, anyway
        ])
        # parse lines
        delays = []
        for line in stdout.split('\n'):
            line = line.strip()
            delay = re.findall(r'^Reply from.*?time=(\d+)ms', line)
            if len(delay) > 0:
                delays.append(int(delay[0]))
        # get average
        if not delays:
            return None
        return sum(delays) / len(delays) / 1000

    def _record_single(self, desc: str, src_ip: str, src_host: str,
                       dst_ip: str, dst_host: str, max_delay: float):
        """ _record_single() -- perform single destination traceroute """
        self._th_semaphore.acquire()
        # ping and tracert
        ping_delay = None
        traces = []
        if sys.platform.startswith('win'):
            ping_delay = self._perform_local_ping_win(dst_ip, max_delay)
            if ping_delay is not None:
                traces = self._perform_local_trace_win(dst_ip, 60, max_delay)
        else:
            raise NotImplementedError('win only')
        # check if unreachable
        if ping_delay is None:
            self.log('error', '%s[%s] not reachable from %s[%s]', dst_ip,
                     dst_host, src_ip, src_host)
            return
        # prepend first hop
        traces = [TraceEntry(
            hop=0,
            delay=0.0,
            router_ip=src_ip,
        )] + traces
        # append last hop
        if len(traces) == 0 or traces[-1].router_ip != dst_ip:
            traces.append(TraceEntry(
                hop=61,
                delay=ping_delay,
                router_ip=dst_ip,
            ))
        # save results
        self._save_traces(desc, src_ip, src_host, dst_ip, dst_host, traces)
        self.log('ok', 'trace %s[%s] -> %s[%s] completed', src_ip,
                 src_host, dst_ip, dst_host)
        self._th_semaphore.release()

    def record(self, desc: str, dst_host: str, max_delay: float,
               rtype: str = 'A') -> None:
        # get current ip
        src_ip = socket.gethostbyname(socket.gethostname())
        src_host = 'localhost'
        # there are many possible ips
        self.log('info', 'resolving ip addresses for [%s]', dst_host)
        try:
            answers = dns.resolver.resolve(dst_host, rtype)
        except dns.resolver.NoAnswer:
            self.log('error', 'destination host [%s] invalid', dst_host)
            answers = []
        for rdata in answers:
            dst_ip = str(rdata)
            self.log('info', 'spawning traceroute event %s[%s] -> %s[%s]',
                     src_ip, src_host, dst_ip, dst_host)
            self._th_lock.acquire()
            th = threading.Thread(target=self._record_single, args=(
                desc, src_ip, src_host, dst_ip, dst_host, max_delay))
            th.start()
            self._th_lock.release()
            self._th_list.append(th)
        return

    def record_mx(self, desc: str, dst_host: str, max_delay: float) -> None:
        """ record_mx() -- resolve mail server """
        self.log('info', 'resolving mail addresses for [mail@%s]', dst_host)
        try:
            answers = dns.resolver.resolve(dst_host, 'MX')
        except dns.resolver.NoAnswer:
            self.log('error', 'destination host [mail@%s] invalid', dst_host)
            answers = []
        for rdata in answers:
            dst_mx = str(rdata).split(' ')[-1]
            self.log('info', 'found mail server [%s] for [mail@%s]', dst_mx,
                     dst_host)
            self.record(desc, dst_mx, max_delay)
        return

    def join(self):
        """ join() -- wait for all threads to stop
        @note: this is NOT THREAD SAFE. make sure you've done all record
               spawning before join()ing. """
        for th in self._th_list:
            th.join()
        self.log('info', 'all tasks completed')

    def show(self) -> None:
        """ show() -- display database """
        self._io_lock.acquire()
        db = sqlite3.connect(self._db_filename)
        cur = db.cursor()
        cur.execute("""
            SELECT a.run_id, desc, src_host, src_ip, dst_host, dst_ip,
                hop, CAST(delay * 1000 AS INTEGER) || ' ms', router_ip
            FROM Traces AS a INNER JOIN TraceEntries AS b
            ON a.run_id = b.run_id
            ORDER BY src_host, src_ip, dst_host, dst_ip, a.run_id, hop ASC;
        """)
        table = PrettyTable()
        table.field_names = [
            'runID', 'Description', 'Source Host', 'Source IP', 'Dest Host',
            'Dest IP', 'Hop #', 'Delay', 'Router IP',
        ]
        for row in cur.fetchall():
            table.add_row(row)
        cur.close()
        db.close()
        self._io_lock.release()
        print(table)
        return

    def get_topology(self) -> List[Tuple[str, str, float]]:
        """ get_topology() -- retrieves list of (u_ip, v_ip, delay) tuples
        generated from traceroute results """
        self._io_lock.acquire()
        db = sqlite3.connect(self._db_filename)
        cur = db.cursor()
        runs = {}  # run_id -> [(hop, router_ip, delay)]
        # parse all
        cur.execute('SELECT run_id, hop, router_ip, delay FROM TraceEntries;')
        for run_id, *data in cur.fetchall():
            if run_id not in runs:
                runs[run_id] = []
            runs[run_id].append(tuple(data))
        cur.close()
        db.close()
        # extract every pair of delay
        edges = []
        for run_id, hops in runs.items():
            data_per_hop = []  # (hop) [(router_ip, delay)]
            for hop in sorted(set(t[0] for t in hops)):
                data_per_hop.append(list(filter(lambda x: x[0] == hop, hops)))
            # adjacent hops create adjacency
            for i in range(1, len(data_per_hop)):
                pre, cur = data_per_hop[i - 1], data_per_hop[i]
                for j in pre:  # any -> any
                    for k in cur:
                        edges.append((j[1], k[1], k[2] - j[2]))
                pass
        # group by edge tuple
        edge_grp = {}
        for u, v, delay in edges:
            if (u, v) not in edge_grp:
                edge_grp[(u, v)] = []
            edge_grp[(u, v)].append(delay)
        # get averaged result
        result = []
        for (u, v), delays in edge_grp.items():
            result.append((u, v, max(sum(delays) / len(delays), 0.0)))
        return result

    def export_graph(self) -> None:
        """ export_graph() -- export graph to image """
        # initialize
        self._io_lock.acquire()
        db = sqlite3.connect(self._db_filename)
        graph = graphviz.Digraph()
        # capture available ip addresses from routers
        addresses = {}
        cur = db.cursor()
        cur.execute("SELECT router_ip FROM TraceEntries;")
        for ip, in cur.fetchall():
            addresses[ip] = ip
        cur.close()
        # capture ip addresses from src & dest
        cur = db.cursor()
        cur.execute("""SELECT src_ip, src_host FROM Traces
            UNION SELECT dst_ip, dst_host FROM Traces""")
        for ip, host in cur.fetchall():
            addresses[ip] = '%s [%s]' % (ip, host)
        db.close()
        self._io_lock.release()
        # get topology
        topo = self.get_topology()
        # determine gateway routers
        gateways = set()
        is_gateway = (lambda delay: delay >= 0.020)
        for u, v, delay in topo:
            if is_gateway(delay):
                gateways.add(u)
                gateways.add(v)
        # draw nodes and edges
        for addr, alias in addresses.items():
            # node type
            if addr == alias:  # regular node
                attr = {'shape': 'box'}
            elif 'localhost' in alias:  # special node / src endpoint
                attr = {'shape': 'box3d', 'style': 'filled',
                        'fillcolor': 'palegreen3'}
            else:  # special node / dst endpoint
                attr = {'shape': 'box3d', 'style': 'filled',
                        'fillcolor': 'skyblue1'}
            # is gateway?
            if addr in gateways and 'fillcolor' not in attr:
                attr['style'] = 'filled'
                attr['fillcolor'] = 'plum1'
            graph.node(addr, alias, **attr)
        for u, v, delay in topo:
            attr = {'color': 'red'} if is_gateway(delay) else {}  # gw
            delay = '%.3f ms' % (delay * 1000)
            graph.edge(u, v, delay, **attr)
        # paint
        graph.format = 'svg'
        graph.render('./kumoko-output.gv')
    pass


if __name__ == '__main__':
    k = Kumoko()
    k.record('Localhost -> THU', 'www.tsinghua.edu.cn', 0.5)
    k.record('Localhost -> HIT', 'www.hit.edu.cn', 0.5)
    k.record('Localhost -> PKU', 'www.pku.edu.cn', 0.5)
    k.record('Localhost -> RUC', 'www.ruc.edu.cn', 0.5)
    k.join()
    k.show()
