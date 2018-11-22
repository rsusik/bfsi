import os, sys, random, argparse

set_loc = './sets'
pat_loc = '.'

parser = argparse.ArgumentParser(description='Generates r patterns of length m for the pattern matching problem', 
	epilog="Example:\npython3 generate_patterns.py -c english.10MB -r 100,1000 -m 8,32")
parser.add_argument("-r", "--npatterns", dest='r', type=str, default='100', help="number of patterns")
parser.add_argument("-c", "--corpus", dest='c', type=str, default='english.10MB', help="corpus")
parser.add_argument("-m", "--length", dest='m', type=str, default='8,16,32,64', help="pattern length[s] (e.g. 8,16,32)")

args = parser.parse_args()

c_args_list	= args.c.split(',')
r_args_list	= args.r.split(',')
m_args_list	= args.m.split(',')

for c_el in c_args_list:
	for r_el in r_args_list:
		for m_el in m_args_list:
			m = int(m_el)
			c = open("{}/{}".format(set_loc, c_el), "rb").read()
			pattern_filename = "{}/patterns.r{}/patterns.{}.{}.bin".format(pat_loc, r_el, c_el, m_el)
			filename = "./patterns.r" + str(r_el) + "/patterns." + c_el + "." + str(m) + ".bin"
			os.makedirs(os.path.dirname(filename), exist_ok=True)

			outFile = open(filename, "wb")
			for i in range(int(r_el)):
				startPos = random.randint(0, len(c) - m)
				patt = c[startPos:startPos + m]
				outFile.write(patt)
			print("Patterns generated to file " + filename)
			outFile.close()
