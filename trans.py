from ctypes import cdll, py_object
libtrans = cdll.LoadLibrary('./libtrans.so')
libtrans.getTransducerNextMinPath.restype = py_object
