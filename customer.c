#! /usr/bin/env python3
import sys
import os
import signal
import time
import heapq


log = open('customer_log', 'w')


class cus_obj:
    def __init__(self, lmt):
        self.lmt = lmt
        self.serial = 1
        self.timeout_heap = []


class ord_obj(cus_obj):
    def __init__(self):
        cus_obj.__init__(self, 1000.0)

    def __call__(self):
        print('ordinary', flush=True)
        self.serial += 1


class mem_obj(cus_obj):
    def __init__(self):
        cus_obj.__init__(self, 1.0)

    def __call__(self):
        os.kill(os.getppid(), signal.SIGUSR1)
        self.serial += 1


class vip_obj(cus_obj):
    def __init__(self):
        cus_obj.__init__(self, 0.3)

    def __call__(self):
        os.kill(os.getppid(), signal.SIGUSR2)
        self.serial += 1

cus_list = [ord_obj(), mem_obj(), vip_obj()]


def ord_hdlr(sig, frame):
    ser = heapq.heappop(cus_list[0].timeout_heap)
    log.write('finish 0 {}\n'.format(ser))


def mem_hdlr(sig, frame):
    ser = heapq.heappop(cus_list[1].timeout_heap)
    log.write('finish 1 {}\n'.format(ser))


def vip_hdlr(sig, frame):
    ser = heapq.heappop(cus_list[2].timeout_heap)
    log.write('finish 2 {}\n'.format(ser))

signal.signal(signal.SIGINT, ord_hdlr)
signal.signal(signal.SIGUSR1, mem_hdlr)
signal.signal(signal.SIGUSR2, vip_hdlr)

data = open(sys.argv[1])

init_time = time.time()
for line in data.readlines():
    code = int(line.split()[0])
    ets = float(line.split()[1])+init_time
    heapq.heappush(
        cus_list[code].timeout_heap,
        [ets+cus_list[code].lmt, cus_list[code].serial]
    )
    while time.time() < ets:
        for obj in cus_list:
            try:
                if time.time() >= obj.timeout_heap[0][0]:
                    log.write('timeout {} {}'.format(code, obj.serial))
                    os.close(sys.stdin.fileno())
                    sys.exit()
            except IndexError:
                pass

    log.write('send {} {}\n'.format(code, cus_list[code].serial))
    cus_list[code]()


def allempty(l):
    for obj in l:
        if obj.timeout_heap:
            return False
    return True

while True:
    if(allempty(cus_list)):
        os.close(sys.stdin.fileno())
        break
    for obj in cus_list:
        try:
            if time.time() >= obj.timeout_heap[0][0]:
                log.write('timeout {} {}'.format(code, obj.serial))
                os.close(sys.stdin.fileno())
                sys.exit()
        except IndexError:
            pass
