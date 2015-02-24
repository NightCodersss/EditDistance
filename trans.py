from ctypes import cdll, py_object
libtrans = cdll.LoadLibrary('./libtrans.so')
libtrans.getTransducerNextMinPath.restype = py_object
libtrans.transducerComposition.restype = py_object

class Transducer:
    
    def __init__(self, trans = None):
        if trans is None:
            trans = libtrans.newTransducer()
        self.trans = trans
#        self.resetMinPaths()
    
    @staticmethod
    def fromRegexp(regex):
        return Transducer(libtrans.transducerFromRegexp(regex))

    def readFromFile(self, filename):
        libtrans.readTransducerFromFile(self.trans, filename)

    def visualize(self):
        libtrans.visualizeTransducer(self.trans)

    def resetMinPaths(self):
        libtrans.resetTransducerMinPaths(self.trans)

    def getNextMinPath(self):
        return libtrans.getTransducerNextMinPath(self.trans)
    
    def composition(self, t):
        """self and t are no longer valid after composition"""
        c = libtrans.transducerComposition(self.trans, t.trans)
        self.trans = None
        t.trans    = None
        return Transducer(c)
