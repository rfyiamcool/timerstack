#!/usr/bin/env python

from distutils.core import setup, Extension

timerfdmodule = Extension("timerstack", sources = ["timerstack.c"])

longdescription = """" python use timerfd to timer engine with epoll"""

setup(
	name = "timerstack",
	version = "1.0",
	description = "make python use timerfd_* syscalls",
	long_description = longdescription,
	author = "fengyun",
	author_email = "rfyiamcool@163.com",
    url = "http://xiaorui.cc",
	platforms = "Linux",
	ext_modules = [timerfdmodule]
)
