from ctypes import cdll, py_object
libtrans = cdll.LoadLibrary('./libtrans.so')
libtrans.getTransducerNextMinPath.restype = py_object

class Transducer:
    
    def __init__(self, trans = None):
        if trans is None:
            trans = libtrans.newTransducer()
        self.trans = trans
        self.resetMinPaths()
    
    @staticmethod
    def fromRegexp(regex):
        return Transducer(libtrans.transducerFromRegexp(regex))

    @staticmethod
    def fromAligmentModel(filename):
        return Transducer(libtrans.transducerFromAligmentModel(filename))

    def readFromFile(self, filename):
        libtrans.readTransducerFromFile(self.trans, filename)

    def visualize(self):
        libtrans.visualizeTransducer(self.trans)

    def resetMinPaths(self):
        libtrans.resetTransducerMinPaths(self.trans)

    def getNextMinPath(self):
        return libtrans.getTransducerNextMinPath(self.trans)

    def pathsIterator(self):
        path = self.getNextMinPath()
        while path != []:
            yield path
            path = self.getNextMinPath()
    
    def composition(self, t):
        """self and t are no longer valid after composition"""
        c = libtrans.transducerComposition(self.trans, t.trans)
        self.trans = None
        t.trans    = None
        return Transducer(c)

def pathsFromWordToRegexp(word, regexp, error_model_file):
    X = Transducer.fromRegexp(word)
    A = Transducer.fromRegexp(regexp)    
    T = Transducer.fromAligmentModel(error_model_file)    

    composition = X.composition(T).composition(A)
    for path in composition.pathsIterator():
        yield path
