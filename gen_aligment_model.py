# -*- coding: utf-8 -*-
f = open("tbig.am", "w");
letters = map(chr, range(ord('a'), ord('z')+1))
letters.append(u'ε'.encode('utf-8'))

f.write("{}\n".format(len(letters)**2))

for ch1 in letters:
    for ch2 in letters:
        f.write("{} {} {}\n".format(ch1, ch2, int(ch1 != ch2)))

f.close()
