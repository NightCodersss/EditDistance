import gen_regex
import re
import rstr
from trans import Transducer

def test_random_regex():
    r = gen_regex.gen_regex()
    print r
    f = True;
    for _ in range(3):
        s = rstr.xeger(r).encode('utf-8')
        if not Transducer.fromRegexp(r.encode('utf-8')).composition(Transducer.fromRegexp(s)).isEmpty():
            print "pizdoh on "+s
            f = False

    print "second part"
    for _ in range(3):
        s = rstr.xeger("[a-z]*")
        if not Transducer.fromRegexp(r.encode('utf-8')).composition(Transducer.fromRegexp(s.encode('utf-8'))).isEmpty() == (re.match(r, s).group() == ''):
            print "pizdoh on "+s
            f = False

    return f
        

