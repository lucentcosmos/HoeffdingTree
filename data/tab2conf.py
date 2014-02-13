import sys

def list2vals(value):
	if value == 'continuous':
		return 'numeric'
	else:
		return "discrete(" + value.strip().replace(' ', ',') + ")"

def getConf(names, values):
	namesL = names.split('\t')
	valuesL = values.split('\t')
	assert(len(namesL) == len(valuesL))
	cfgStr = "dataFormat = ("
	for item in namesL:
		cfgStr += item + ","
	cfgStrL = list(cfgStr)
	cfgStrL[len(cfgStrL)-1] = ')'
	cfgStr = "".join(cfgStrL) + "\n"
	for attrN in range(len(namesL)):
		cfgStr += namesL[attrN] + ": " + list2vals(valuesL[attrN]) + "\n"
	return cfgStr

# entry point 
if __name__ == "__main__":
	f = open(sys.argv[1] if len(sys.argv) > 1 else 'audiology.tab.txt')
	names = f.readline().strip()
	values = f.readline().strip()
	cfgStr = getConf(names, values)
	print cfgStr
