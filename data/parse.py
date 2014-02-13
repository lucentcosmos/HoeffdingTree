from random import randint, shuffle

f = open('titanic.dat', 'w')
L = open('titanic.tab').read().split('\n')
L = L[3:len(L)-2]
for k in range(1000000):
	shuffle(L)
	for x in L:
		line = x.replace('\t', ',')
		# print line
		f.write(line+'\n')
