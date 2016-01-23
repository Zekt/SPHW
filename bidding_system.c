#! /usr/bin/env python3
import sys
import os
import signal
import time


log = open('bidding_system_log', 'w')


def mem_hdlr(sig, frame):
    signal.signal(signal.SIGUSR1, mem_hdlr)
    log.write('receive 1 {}\n'.format(mem_hdlr.serial))
    time.sleep(.5)
    os.kill(cid, signal.SIGUSR1)
    log.write('finish 1 {}\n'.format(mem_hdlr.serial))
    mem_hdlr.serial += 1
mem_hdlr.serial = 1


def vip_hdlr(sig, frame):
    signal.signal(signal.SIGUSR2, vip_hdlr)
    signal.pthread_sigmask(signal.SIG_BLOCK, [signal.SIGUSR1])
    log.write('receive 2 {}\n'.format(vip_hdlr.serial))
    time.sleep(.2)
    os.kill(cid, signal.SIGUSR2)
    log.write('finish 2 {}\n'.format(vip_hdlr.serial))
    vip_hdlr.serial += 1
    signal.pthread_sigmask(signal.SIG_UNBLOCK, [signal.SIGUSR1])
vip_hdlr.serial = 1


def ord_hdlr():
    log.write('receive 0 {}\n'.format(ord_hdlr.serial))
    time.sleep(1)
    os.kill(cid, signal.SIGINT)
    log.write('finish 0 {}\n'.format(ord_hdlr.serial))
    ord_hdlr.serial += 1
ord_hdlr.serial = 1

# (read_fd, write_fd)
pfds = os.pipe()
cid = os.fork()

# Child Process
if(cid == 0):
    os.close(pfds[0])
    os.dup2(pfds[1], sys.stdout.fileno())
    os.execv('./customer', ['customer', sys.argv[1]])

# Parent Process
os.close(pfds[1])
os.dup2(pfds[0], sys.stdin.fileno())
signal.signal(signal.SIGUSR1, mem_hdlr)
signal.signal(signal.SIGUSR2, vip_hdlr)

while True:
    msg = sys.stdin.readline()
    if msg == 'ordinary\n':
        ord_hdlr()
    elif msg == '':
        log.write('terminate\n')
        break
