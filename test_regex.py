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
    for _ in range(0):
        s = screen(rstr.xeger(r).encode('utf-8'))
#        if Transducer.fromRegexp(r.encode('utf-8')).composition(Transducer.fromRegexp(s)).isEmpty():
        if Transducer.fromRegexp(s).composition(Transducer.fromRegexp(r.encode('utf-8'))).isEmpty():
            print "pizdoh on "+s
#            return False
            f = False

#    print "second part"
    for _ in range(0):
        s = rstr.xeger("[a-z][a-z]*")

        match = re.match(r, s)

#        ourautomat = Transducer.fromRegexp(r.encode('utf-8')).composition(Transducer.fromRegexp(s.encode('utf-8'))).isEmpty() 
        ourautomat = Transducer.fromRegexp(s.encode('utf-8')).composition(Transducer.fromRegexp(r.encode('utf-8')))
        ourautomat.visualize()
#        print "Our automat said ", ourautomat.isEmpty()
    
        pythonregex = (match == None or match.group() != s)
#        print "Python said ", pythonregex
        
        if ourautomat.isEmpty() != pythonregex:
            print "pizdoh on "+s
#            return False
            f = False

    print "levenstein"

    for _ in range(1000):
        s1 = rstr.xeger("[a-z][a-z]*")
        s2 = rstr.xeger("[a-z][a-z]*")

        path = pathsFromWordToRegexp(s1, s2, 't.am').next()
        minway = sum([w for (s, w) in path]) 
        
        print "Levenstein said ", levenshtein(s1, s2)
        print "Transducer said ", minway

        if levenshtein(s1, s2) != minway:
            print 'pizdoh on ({}, {})'.format(s1, s2)
            return False

    return f
        

