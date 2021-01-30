import sys
import string

mapping = dict()

for line in open(sys.argv[1], 'r', encoding = 'big5-hkscs'):
    line = line.strip().split(" ")
    ch = line[0]
    zhuyin = line[1].split('/')
    # map character to the same character
    if ch not in mapping:
        mapping[ch] = set()
        mapping[ch].add(ch)
    #reverse mapping zhuyin to character
    for i in range(len(zhuyin)):
        if zhuyin[i][0] not in mapping:
            mapping[zhuyin[i][0]] = set()
            mapping[zhuyin[i][0]].add(zhuyin[i][0])
        else:
            mapping[zhuyin[i][0]].add(ch)

with open(sys.argv[2], 'w', encoding = 'big5-hkscs') as out_file:
    for ch in mapping:
        print (ch, end=" ", file=out_file)
        for zh in mapping[ch]:
            print (zh, end=" ", file=out_file)
        print("", file=out_file)
