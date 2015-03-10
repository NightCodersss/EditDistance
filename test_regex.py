import gen_regex
import trans

def test_random_regex():
    r = gen_regex.gen_regex()
    for _ in range(50):
        t = Transducer.fromRegexp(r);
        s = rstr.xeger(r);
        t.composition(Transducer.fromRegexp(s)).
        

