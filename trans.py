# -*- coding: utf-8 -*-

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

    def __del__(self):
        libtrans.release(self.trans)

    
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
        
        words = set([])

        path = self.getNextMinPath()
        while path != []:
            word   = ''.join((filter(lambda x: x not in (u'\u03b5',), s.decode('utf-8')[2]) for (s, _) in path)).encode('utf-8')
            weight = sum((w for (_, w) in path))
            if word not in words:
                words.add(word)          
            yield (word, weight, path)
            path = self.getNextMinPath()
    
    def composition(self, t):
        """self and t are no longer valid after composition"""
        c = libtrans.transducerComposition(self.trans, t.trans)
#        self.trans = None
#        t.trans    = None
        return Transducer(c)

    def minWay(self):
        libtrans.minWay(self.trans)

    def optimize(self):
        libtrans.optimizeTransducer(self.trans)

def pathsFromWordToRegexp(word, regexp, error_model_file):
    """iterator for min edit-distance paths"""
    X = Transducer.fromRegexp(word)
    X.optimize()
    A = Transducer.fromRegexp(regexp)        
    A.optimize()
    T = Transducer.fromAlignmentModel(error_model_file)
    T.optimize()

    composition = X.composition(T).composition(A)
    composition.optimize()

    for path in composition.pathsIterator():
        yield path
