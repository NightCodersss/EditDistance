import gen_regex
import re
import rstr
import random
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


def test_random_regex(n, k):
    f = True;
    for _ in range(n):
        r = gen_regex.gen_regex()
        for _ in range(k):
            s = screen(rstr.xeger(r).encode('utf-8'))
            t1 = Transducer.fromRegexp(s)
            t1.optimize()
            t2 = Transducer.fromRegexp(r.encode('utf-8'))
            t2.optimize()
            t = t1.composition(t2)
            t.optimize()
            if t.isEmpty():
                print "Failed on "+s
                return False
                f = False

        for _ in range(k):
            s = rstr.xeger("[a-z][a-z]*")

            match = re.match(r, s)

            ourautomat = Transducer.fromRegexp(s.encode('utf-8')).composition(Transducer.fromRegexp(r.encode('utf-8')))
            ourautomat.optimize()
        
            pythonregex = (match == None or match.group() != s)
            
            if ourautomat.isEmpty() != pythonregex:
                print "Our automat said ", ourautomat.isEmpty()
                print "Python said ", pythonregex
                print "Failed on "+s
                return False
                f = False
    return f

def test_levinstein(n):
    print "levenstein"

    for _ in range(n):
        s1 = rstr.xeger("[a-z]{1,15}")
        s2 = rstr.xeger("[a-z]{1,15}")

        path = pathsFromWordToRegexp(s1.encode('utf-8'), s2.encode('utf-8'), 'tbig.am').next()
        minway = sum((w for (s, w) in path[2])) 

        if levenshtein(s1, s2) != minway:
            print "Levenstein said ", levenshtein(s1, s2)
            print "Transducer said ", minway
            print 'Failed on ({}, {})'.format(s1, s2)
            return False
    return True

def test_of_weight(n):
    print "weight"

    for _ in range(n):
        s11 = rstr.xeger("[a-b]{1,15}")
        s12 = rstr.xeger("[a-b]{1,15}")
        s21 = rstr.xeger("[a-b]{1,15}")
        s22 = rstr.xeger("[a-b]{1,15}")
        
        path = pathsFromWordToRegexp((s11 + 't' + s12).encode('utf-8'), (s21 + 't' + s22).encode('utf-8'), 'tweight.am').next() 
        minway = sum((w for (s, w) in path[2])) 
        lev = levenshtein(s11,s21) + levenshtein(s12, s22)
        if lev != minway:
            print 'Failed on ({} t {}, {} t {}), found path: {}'.format(s11, s12, s21, s22, path)
            print 'Lev: {}; Trans: {}'.format(lev, minway)
            return False
    return True

        
def test():
    random.seed(16)
#    random.seed(4)
#    random.seed(1)
    if not test_random_regex(50, 20):
        return False
    print "Passed regexp tests"
    if not test_of_weight(50):
        return False
    print "Passed weight tests"
    if not test_levinstein(50):
        return False
    print "Passed levenshtein tests"
    return True

test()
