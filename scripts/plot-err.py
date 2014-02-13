from matplotlib import pyplot

f = open('err.dat').read().split('\n')

y = [float(k) if len(k) > 0 else 0 for k in f]
x = range(len(y))

pyplot.plot(x, y)
pyplot.show()
