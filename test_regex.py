import gen_regex
import re
import rstr
from trans import Transducer, screen, pathsFromWordToRegexp
    
def levenshtein(s1, s2):
    if len(s1) < len(s2):
        return levenshtein(s2, s1)
             
    # len(s1) >= len(s2)
    if len(s2) == 0:
        return len(s1)
                                 
    previous_row = range(len(s2) + 1)
    for i, c1 in enumerate(s1):
        current_row = [i + 1]
        for j, c2 in enumerate(s2):
            insertions = previous_row[j + 1] + 1 # j+1 instead of j since previous_row and current_row are one character longer
            deletions = current_row[j] + 1       # than s2
            substitutions = previous_row[j] + (c1 != c2)
            current_row.append(min(insertions, deletions, substitutions))
        previous_row = current_row
         
    return previous_row[-1]


def test_random_regex():
    r = gen_regex.gen_regex()
#    print r
    f = True;
    for _ in range(1000):
        s = screen(rstr.xeger(r).encode('utf-8'))
#        if Transducer.fromRegexp(r.encode('utf-8')).composition(Transducer.fromRegexp(s)).isEmpty():
        if Transducer.fromRegexp(s).composition(Transducer.fromRegexp(r.encode('utf-8'))).isEmpty():
            print "Failed on "+s
#            return False
            f = False

#    print "second part"
    for _ in range(1000):
        s = rstr.xeger("[a-z][a-z]*")

        match = re.match(r, s)

#        ourautomat = Transducer.fromRegexp(r.encode('utf-8')).composition(Transducer.fromRegexp(s.encode('utf-8'))).isEmpty() 
        ourautomat = Transducer.fromRegexp(s.encode('utf-8')).composition(Transducer.fromRegexp(r.encode('utf-8')))
        ourautomat.visualize()
#        print "Our automat said ", ourautomat.isEmpty()
    
        pythonregex = (match == None or match.group() != s)
#        print "Python said ", pythonregex
        
        if ourautomat.isEmpty() != pythonregex:
            print "Failed on "+s
#            return False
            f = False
    return f

def test_levinstein():
    print "levenstein"

    for _ in range(1):
        s1 = rstr.xeger("[a-z][a-z]{1,15}")
        s2 = rstr.xeger("[a-z][a-z]{1,15}")

#        path = pathsFromWordToRegexp(s1.encode('utf-8'), s2.encode('utf-8'), 't.am').next()
        X = Transducer.fromRegexp(s1.encode('utf-8'))
        A = Transducer.fromRegexp(s2.encode('utf-8'))    
        T = Transducer.fromAlignmentModel('tbig.am')    

        print "comp is started"
        composition = X.composition(T).composition(A)
        print "compsed"
        for path in composition.pathsIterator():        
#        path = pathsFromWordToRegexp("bba", "bab", 't.am').next()
            minway = sum([w for (s, w) in path]) 
#        composition.minWay()           
            print "Levenstein said ", levenshtein(s1, s2)
            print "Transducer said ", minway

        if levenshtein(s1, s2) != minway:
            print 'Failed on ({}, {})'.format(s1, s2)
            return False

        
def test():
    test_levinstein()
#    for i in range(100):
#        test_random_regex()
