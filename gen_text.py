
import rstr, random, time, math
import matplotlib.pyplot as plt
from trans import pathsFromWordToRegexp

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

def gen_dict(size):
    regex = "[a-z][a-z]{0,10}"
    result = set([])

    for _ in range(size):
        result.add(rstr.xeger(regex))

    return result

def gen_regexp(dic):
    return "((" + "|".join(dic) + "))*"

def gen_text(regex):
    return rstr.xeger(regex)

def gen_graph(number_of_tests, dic_size): 
   
    dic = gen_dict(dic_size)
    regex = gen_regexp(dic)

    times = []
    lens  = []

    dist = []    

    for i in range(number_of_tests):
        text = gen_text(regex)
        spoiled_text = "".join(map(lambda c: c if random.uniform(0.0, 1.0) < 0.9 else chr(random.randint(ord('a'), ord('z'))), text))
        
        start_time = time.time()
        path = pathsFromWordToRegexp(spoiled_text.encode('utf-8'), regex.encode('utf-8'), 'tenglishandpunct.am').next()        

        times.append(time.time() - start_time)
        lens.append(len(text))

        our_weight = path[1]
        dist.append(our_weight)
        lev = levenshtein(spoiled_text, path[0])

        if lev != our_weight:
            print "Lev failed:"
            print "Dict = ", dic
            print "Regex = ", regex
            print "Text = ", text
            print "Spoiled Text = ", spoiled_text
            print path[0], path[1]
    
    plt.plot(lens, times, 'ro')
    plt.show()
    plt.plot(dist, lens, 'ro')
    plt.show()

gen_graph(100, 10)
