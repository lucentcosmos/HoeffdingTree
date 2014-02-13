#!/usr/bin/python

import random as rnd

# entry point 
if __name__ == "__main__":
	f = open('numeric-test.dat', 'w')
	for i in range(5000000):
		if rnd.randint(0,1) == 0:
			f.write(str(rnd.uniform(6.0, 10.0))+",Y\n")
		else:
			f.write(str(rnd.uniform(1.0, 5.9))+",N\n")
	for i in range(5000000):
		if rnd.randint(0,1) == 0:
			f.write(str(rnd.uniform(6.0, 10.0))+",N\n")
		else:
			f.write(str(rnd.uniform(1.0, 5.9))+",Y\n")
	f.close()
# end of file 
