# timerstack

### 介绍:

Python基于linux timerfd实现的定时器模块. 使用Epoll来监听调度timerfd.  [更多介绍](http://xiaorui.cc)

`需要说明的是, 社区中python timerfd 存在一些bug, 不可用呀...    timerstack.c 是我用PyObject timerfd c api封装的. 我这边修复并加入Epoll监听事件重新打包一个可用的项目.`

具体文档可直接参考timerfd api.  [linux timerfd ](http://man7.org/linux/man-pages/man2/timerfd_create.2.html)

### Install:

`pip`
```
pip install timerstack
```

`source`
```
git clone https://github.com/rfyiamcool/timerstack.git
cd timerstack
python setup.py install
```

### Simple Method:

timerstack.create():   创建一个相对时间的定时器fd
timerstack.settime():  设置新旧时间，可以简单理解为间隔时间和次数.
timerstack.gettime():  查看模式

```
import timerstack,os
f = timerstack.create(timerfd.CLOCK_REALTIME,0)
timerstack.settime(f,0,10,0)     #单次 10s 
timerstack.settime(f,0,0,0)      #停止 
timerstack.settime(f,0,5,5)      #每5秒钟轮一次,次数不限制
os.read(f,1024)
```

### Test:
```
python scheduler.py
```


### TO DO:
more...
