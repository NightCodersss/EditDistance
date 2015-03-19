from ctypes import cdll, py_object
libtrans = cdll.LoadLibrary('./libtrans.so')
libtrans.getTransducerNextMinPath.restype = py_object
libtrans.screen.restype = py_object

def screen(s):
    return libtrans.screen(s)

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
    def fromAlignmentModel(filename):
        return Transducer(libtrans.transducerFromAligmentModel(filename))

    def readFromFile(self, filename):
        libtrans.readTransducerFromFile(self.trans, filename)

    def visualize(self):
        libtrans.visualizeTransducer(self.trans)

    def isEmpty(self):
        return libtrans.isTransducerEmpty(self.trans)

    def resetMinPaths(self):
        """resets path iterator to the beginning"""
        libtrans.resetTransducerMinPaths(self.trans)

    def getNextMinPath(self):
        """It's used by pathsIterator"""
        return libtrans.getTransducerNextMinPath(self.trans)

    def pathsIterator(self):
        """generator of the paths"""
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

    def minWay(self):
        libtrans.minWay(self.trans)

    def optimize(self):
        libtrans.optimizeTransducer(self.trans)

def pathsFromWordToRegexp(word, regexp, error_model_file):
    """iterator for min edit-distance paths"""
    X = Transducer.fromRegexp(word)
    A = Transducer.fromRegexp(regexp)    
    T = Transducer.fromAlignmentModel(error_model_file)    

    composition = X.composition(T).composition(A)

    for path in composition.pathsIterator():
        yield path
