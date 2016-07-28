#include <Python.h>
#include <time.h>
#include <sys/timerfd.h>


/* Python: create(clockid,flags) -> fd
   C:      int timerfd_create(int clockid, int flags); */
static PyObject * _create(PyObject *self, PyObject *args) {
	/* variable definitions */
	int clockid;
	int flags;
	int result;
	
	/* parse the function's arguments: int clockid, int flags */
	if (!PyArg_ParseTuple(args, "ii", &clockid, &flags)) return NULL;
	
	/* call timerfd_create; catch errors by raising an exception */
	result = timerfd_create(clockid, flags);
	if(result == -1)
		return PyErr_SetFromErrno(PyExc_OSError);
	
	/* everything's fine, return file descriptor returned by timerfd_create() */
	return PyLong_FromLong(result);
};

/* Python: settime(fd,flags,value,interval) -> value,interval
   C:      int timerfd_settime(int fd, int flags,
                               const struct itimerspec *new_value,
                               struct itimerspec *old_value); */
static PyObject * _settime(PyObject *self, PyObject *args) {
	/* variable definitions */
	int fd;
	int flags;
	int result;
	float value;
	float interval;
	struct itimerspec old_value;
	struct itimerspec new_value;
	PyObject *resulttuple;
	
	/* parse the function's arguments: int clockid, int flags */
	if (!PyArg_ParseTuple(args, "iiff", &fd, &flags, &value, &interval))
		return NULL;
	
	/* prepare struct itimerspec */
	new_value.it_value.tv_sec  = (time_t)value;
	new_value.it_value.tv_nsec = (long int)( 1e9 * (value - (int)value) );
	
	new_value.it_interval.tv_sec  = (time_t)interval;
	new_value.it_interval.tv_nsec = (long int)( 1e9 * (interval - (int)interval) );
	
	/* call timerfd_settime; catch errors by raising an exception */
	result = timerfd_settime(fd, flags, &new_value, &old_value);
	if (result == -1)
		return PyErr_SetFromErrno(PyExc_OSError);
	
	/* convert returned struct old_value */
	value    = (float)old_value.it_value.tv_sec    + (float)old_value.it_value.tv_nsec / 1e9;
	interval = (float)old_value.it_interval.tv_sec + (float)old_value.it_interval.tv_nsec / 1e9;
	resulttuple = Py_BuildValue("(ff)", value, interval);
	
	/* everything's fine, return tuple (value,interval) created from old_value */
	return resulttuple;
};


/* Python: gettime(fd) -> value,interval
   C:      int timerfd_gettime(int fd, struct itimerspec *curr_value); */
static PyObject * _gettime(PyObject *self, PyObject *args) {
	/* variable definitions */
	int fd;
	int flags;
	int result;
	float value;
	float interval;
	struct itimerspec curr_value;
	PyObject *resulttuple;
	
	/* parse the function's arguments: int clockid, int flags */
	if (!PyArg_ParseTuple(args, "i", &fd))
		return NULL;
	
	/* call timerfd_gettime; catch errors by raising an exception */
	result = timerfd_gettime(fd, &curr_value);
	if(result == -1)
		return PyErr_SetFromErrno(PyExc_OSError);
	
	/* convert returned struct old_value */
	value    = (float)curr_value.it_value.tv_sec    + (float)curr_value.it_value.tv_nsec / 1e9;
	interval = (float)curr_value.it_interval.tv_sec + (float)curr_value.it_interval.tv_nsec / 1e9;
	resulttuple = Py_BuildValue("(ff)", value, interval);
	
	/* everything's fine, return tuple (value,interval) created from old_value */
	return resulttuple;
};


static PyMethodDef methods[] = {
	{"create", _create, METH_VARARGS,
		"create(clockid,flags) -> file descriptor\n\n"
		"Create a new timer object and return a file descriptor that refers to that timer.\n\n"
		"The 'clockid' argument specifies the clock that is used to mark the progress of\n"
		"the timer, and must be either CLOCK_REALTIME or CLOCK_MONOTONIC.\n\n"
		"CLOCK_REALTIME is a settable system-wide clock. CLOCK_MONOTONIC is a nonsettable\n"
		"clock that is not affected by discontinuous changes in the system clock\n"
		"(e.g., manual changes to system time).\n\n"
		"Starting with Linux 2.6.27, the following values may be bitwise ORed in flags to\n"
		"change the behavior of timerfd_create():\n\n"
		"   TFD_NONBLOCK   Set the O_NONBLOCK file status flag on the new open file\n"
		"                  description. Using this flag saves extra calls to 'fcntl()' to\n"
		"                  achieve the same result.\n\n"
		"   TFD_CLOEXEC    Set the close-on-exec (FD_CLOEXEC) flag on the new file\n"
		"                  descriptor. See the description of the O_CLOEXEC flag in\n"
		"                  'open()' for reasons why this may be useful.\n\n"
		"An OSError exception will be raised if an error occurs. The following error\n"
		"numbers (refer to module 'errno') might occur:\n\n"
		"   EINVAL   Either the 'clockid' argument is neither CLOCK_MONOTONIC nor\n"
		"            CLOCK_REALTIME; or 'flags' is invalid; or, in Linux 2.6.26 or\n"
		"            earlier, 'flags' is nonzero.\n\n"
		"   EMFILE   Either the per-process limit of open file descriptors or the system-\n"
		"            wide limit on the total number of open files has been reached.\n\n"
		"   ENODEV   Could not mount (internal) anonymous inode device.\n\n"
		"   ENOMEM   There was insufficient kernel memory to create the timer.\n"
    },
	{"settime", _settime, METH_VARARGS,
		"settime(fd,flags,value,interval) -> old_value,old_interval\n\n"
		"Arm (start) or disarm (stop) the timer referred to by the file descriptor fd\n"
		"and return the previous settings.\n\n"
		"The 'value' argument specifies the initial expiration of the timer, in seconds\n"
		"(as a float). A value of zero disarms the timer.\n\n"
		"Setting the 'interval' argument to nonzero float values specifies the period,\n"
		"in seconds, for repeated timer expirations after the initial expiration. If\n"
		"'interval' is set to zero, the timer expires just once, at the time specified\n"
		"by 'value'.\n\n"
		"The 'flags' argument is either zero, to start a relative timer or\n"
		"TFD_TIMER_ABSTIME, to start an absolute timer. While a relative timer will\n"
		"expire after an amount of time specified in 'value', an absolute timer will\n"
		"expire if the internal clock reaches the value specified by 'value'\n"
		"(i.e. seconds since the epoch).\n\n"
		"An OSError exception will be raised if an error occurs. The following error\n"
		"numbers (refer to module 'errno') might occur:\n\n"
		"   EBADF    fd is not a valid file descriptor.\n\n"
		"   EINVAL   Either fd is not a valid timer file descriptor or flags is invalid.\n"
    },
	{"gettime", _gettime, METH_VARARGS,
		"gettime(fd) -> value,interval\n\n"
		"Return the current setting of the timer referred to by the file descriptor 'fd'.\n\n"
		"The 'value' variable returns the amount of time until the timer will expire.\n"
		"If it is zero, then the timer is currently disarmed. This variable always\n"
		"contains a relative value, regardless of whether the TFD_TIMER_ABSTIME flag was\n"
		"specified when setting the timer.\n\n"
		"The 'interval' variable returns the interval of the timer. If it is zero, then\n"
		"the timer is set to expire just once, at the time specified by 'value'.\n\n"
		"An OSError exception will be raised if an error occurs. The following error\n"
		"numbers (refer to module 'errno') might occur:\n\n"
		"   EBADF    fd is not a valid file descriptor.\n\n"
		"   EINVAL   fd is not a valid timerfd file descriptor.\n"

	},
    {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef timerfdmodule = {
	PyModuleDef_HEAD_INIT,
	"timerfd",
	"Python bindings for the Linux system calls 'timerfd_create()',\n"
	"'timerfd_settime()' and 'timerfd_gettime()'. These system calls create/manage\n"
	"timers that notify via file descriptors (available since kernel 2.6.25).\n\n"
	"Reading such a timer file returns an unsigned 8-byte integer containing the\n"
	"number of expirations that have occurred. As of Python 3.2 the returned bytes\n"
	"can be converted to a Python int using the 'from_bytes()' method:\n\n"
	"   filecontent = os.read(fd,1024)\n"
	"   expirations = int.from_bytes(filecontent,byteorder=sys.byteorder)\n\n"
	"Of course these timers can be monitored with 'select()' or 'epoll()'.\n\n"
	"Feel free to consult the manpage of timerfd_create for further information.\n"
	"(this documentation is based on the original manpage of 'timerfd_create()')\n",
	-1,
	methods
};
#endif

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit_timerfd(void) {
#else
void inittimerstack(void) {
//void inittimerfd(void) {
#endif
	PyObject *m;
#if PY_MAJOR_VERSION >= 3
	m = PyModule_Create(&timerfdmodule);
#else
	m = Py_InitModule("timerstack",methods);
#endif
	if (m != NULL) {
		/* define timerfd constants */
		PyModule_AddIntConstant(m, "CLOCK_REALTIME", CLOCK_REALTIME);
		PyModule_AddIntConstant(m, "CLOCK_MONOTONIC", CLOCK_MONOTONIC);
		PyModule_AddIntConstant(m, "TFD_CLOEXEC", TFD_CLOEXEC);
		PyModule_AddIntConstant(m, "TFD_NONBLOCK", TFD_NONBLOCK);
		PyModule_AddIntConstant(m, "TFD_TIMER_ABSTIME", TFD_TIMER_ABSTIME);
	}
#if PY_MAJOR_VERSION >= 3
	return m;
#endif
}
                                                                                                                                                                                 
