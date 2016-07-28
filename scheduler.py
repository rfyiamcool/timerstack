#coding:utf-8
import os
import time
import logging
import select
import functools

import timerstack


logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

func_map = {}


def go(*args):
    logger.info(args)


def test():
    for i in range(100000):
        i += 10
        f = timerstack.create(timerstack.CLOCK_REALTIME,0)
        timerstack.settime(f,0,i,i)
        func_map[f] = functools.partial(go, i)
    
    f = timerstack.create(timerstack.CLOCK_REALTIME,0)
    timerstack.settime(f,0,10,0) #only once
    func_map[f] = functools.partial(go, i)
    run(func_map.keys())


def run(inputs):
    outputs = []
    try:
        # 创建 epoll 句柄
        epoll_fd = select.epoll()
        # 向 epoll 句柄中注册 监听 socket 的 可读 事件
        for ff in inputs:
            epoll_fd.register(ff, select.EPOLLIN)
    except select.error, msg:
        logger.error(msg)

    while True:
        epoll_list = epoll_fd.poll()
        for fd, events in epoll_list:
            func_map[fd]()

    
def io_select():
    while inputs:
        readable , writable , exceptional = select.select(inputs, outputs, inputs, 0.1)
        for s in readable:
            os.read(s,1024)
            func_map[s](123)
    

if __name__ == "__main__":
    test()
