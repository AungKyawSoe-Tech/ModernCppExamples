mDictionary = ["act",  "ant",  "art",  "bat",  "bet",  "boss",  "cat",
 "cap",  "cop",  "dear", "dog", "dip",
               "ear", "end", "eel", "fad", "fat", "fog", "gap", "god",
"hat", "hit", "hot", "ink", "irk",
               "jot", "jab", "lap", "lip", "lot", "man", "nan", "nat",
"net", "pat", "pet", "tap", "tar", "ten",
               "rat", "woo", "yoo", "zoo"]

Pairs = {x:(''.join(sorted(x))) for x in mDictionary}

print('----------Print Items In Dictionary "Pairs":---------------------')
for k, v in Pairs.items():
    print(k, v)

print('----------Find Keys With Same Values ---------------------')
Anagrams = {}
for k, v in Pairs.items():
    if v in mDictionary:
        Anagrams.setdefault(v, []).append(k)

print('----------Print "Annagrams":---------------------')
for k, v in Anagrams.items():
    print(k, v)

#Learning Links:
#https://docs.python.org/3/tutorial/datastructures.html
#https://stackoverflow.com/questions/40767647/populate-dictionary-from-list
#https://www.geeksforgeeks.org/python-program-to-sort-a-string/
#https://stackoverflow.com/questions/4900308/find-all-key-elements-by-the-same-value-in-dicts
