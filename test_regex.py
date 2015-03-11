import gen_regex
import re
import rstr
from trans import Transducer, screen

def test_random_regex():
    r = gen_regex.gen_regex()
#    print r
    f = True;
    for _ in range(1000):
        s = screen(rstr.xeger(r).encode('utf-8'))
#        if Transducer.fromRegexp(r.encode('utf-8')).composition(Transducer.fromRegexp(s)).isEmpty():
        if Transducer.fromRegexp(s).composition(Transducer.fromRegexp(r.encode('utf-8'))).isEmpty():
            print "pizdoh on "+s
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
            print "pizdoh on "+s
#            return False
            f = False

    return f
        

