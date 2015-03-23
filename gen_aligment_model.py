# -*- coding: utf-8 -*-

letters = []

def gen_letters(filename):
    s = set(open(filename, 'r').read().decode('utf-8'))
    s.remove(u'\n')
    s.remove(u' ')
    s.remove(u'\t')
    s.add(u'\\n')
    s.add(u'\\s')
    s.add(u'\\t')
    s.add(u'ε')
    return s

#letters = map(chr, range(ord('a'), ord('z')+1))
#letters.append(u'ε'.encode('utf-8'))

def gen_model(filename, letters):

    f = open(filename, "w");
    f.write("{}\n".format(len(letters)**2))

    for ch1 in letters:
        for ch2 in letters:            
            f.write("{} {} {}\n".format(ch1.encode('utf-8'), ch2.encode('utf-8'), int(ch1 != ch2)))


    f.close()

gen_model('trussianandpunct.am', gen_letters('warandpeace.txt'))
