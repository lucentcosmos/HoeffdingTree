from random import randint

L = open('housing.dat').read().split('\n')
f = open('housing-100K.dat', 'w')
# print L
n = 10**5
while n > 0:
	f.write(L[randint(0, len(L)-1)]+'\n')
	if n%10**6 == 0:
		print "Writing line number", n
	n = n-1
f.write(L[randint(0, len(L)-1)])
