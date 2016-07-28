# timerstack

### 介绍:

Python基于linux timerfd实现的定时器模块. 使用Epoll来监听调度timerfd.

### Install:
```
python setup.py install
```

### Simple Method:

```
import timerstack,os
f = timerstack.create(timerfd.CLOCK_REALTIME,0)
timerstack.settime(f,0,10,0)
os.read(f,1024)
```

### Test:
```
python scheduler.py
```


### TO DO:
more...
