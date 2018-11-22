import subprocess, sys, os, platform, argparse, gzip, shutil
from urllib.request import urlopen

pattern_loc = '.' # each folder should start with patterns and have a name with suffix .r1000 where 1000 is number of patterns
set_loc = './sets'
alg_loc = '.'

curr_path = os.getcwd()

pizza_corpus = {
		'proteins.50MB': 'http://pizzachili.dcc.uchile.cl/texts/protein/proteins.50MB.gz',
		'proteins.100MB': 'http://pizzachili.dcc.uchile.cl/texts/protein/proteins.100MB.gz',
		'proteins.200MB': 'http://pizzachili.dcc.uchile.cl/texts/protein/proteins.200MB.gz',
		'sources.50MB': 'http://pizzachili.dcc.uchile.cl/texts/code/sources.50MB.gz',
		'sources.100MB': 'http://pizzachili.dcc.uchile.cl/texts/code/sources.100MB.gz',
		'sources.200MB': 'http://pizzachili.dcc.uchile.cl/texts/code/sources.200MB.gz',
		'dna.50MB': 'http://pizzachili.dcc.uchile.cl/texts/dna/dna.50MB.gz',
		'dna.100MB': 'http://pizzachili.dcc.uchile.cl/texts/dna/dna.100MB.gz',
		'dna.200MB': 'http://pizzachili.dcc.uchile.cl/texts/dna/dna.200MB.gz',
		'english.50MB': 'http://pizzachili.dcc.uchile.cl/texts/nlang/english.50MB.gz',
		'english.100MB': 'http://pizzachili.dcc.uchile.cl/texts/nlang/english.100MB.gz',
		'english.200MB': 'http://pizzachili.dcc.uchile.cl/texts/nlang/english.200MB.gz',
		'english.1024MB':'http://pizzachili.dcc.uchile.cl/texts/nlang/english.1024MB.gz',
		'xml.50MB': 'http://pizzachili.dcc.uchile.cl/texts/xml/dblp.xml.50MB.gz',
		'xml.100MB': 'http://pizzachili.dcc.uchile.cl/texts/xml/dblp.xml.100MB.gz',
		'xml.200MB': 'http://pizzachili.dcc.uchile.cl/texts/xml/dblp.xml.200MB.gz',
}

def check_patterns_existance(corpus, r, m):
		filename = "{}patterns.r{}/patterns.{}.{}.bin".format(pattern_loc, r, corpus, m)
		if os.path.isfile(filename):
			return True
		os.system("python3 generate_patterns.py -c {} -r {} -m {}".format(corpus, r, m))


def check_corpus_existance(corpus):
		filename = "{}/{}".format(set_loc, corpus)
		if os.path.isfile(filename):
			return True
		if corpus not in pizza_corpus:
			return False
		print("Warning: Corpus does NOT exists. Trying to download (it may take a while).")
		sys.stdout.flush()
		url = pizza_corpus[corpus]
		with urlopen(url) as ret:
			if ret.code != 200:
				return False
		gz_filename = "{}.gz".format(filename)
		os.makedirs(os.path.dirname(gz_filename), exist_ok=True)
		with urlopen(url) as response, open(gz_filename, 'wb') as gz_file:
			shutil.copyfileobj(response, gz_file)
		if os.path.isfile(gz_filename):
			with gzip.open(gz_filename, 'rb') as gz_file, open(filename, 'wb') as dest_file:
				dest_file.write(gz_file.read())
			if os.path.isfile(filename):
				os.remove(gz_filename)
				return True
		return False

parser = argparse.ArgumentParser(description='BFSI testing script.',
		epilog="Example:\npython3 test.py -a bloom_faoso_std_h1h2 -c dna.100MB -m 32,64 -q 4,6,8 -u 4,8 -k 1,2")
parser.add_argument("-r", "--npatterns", dest='r', type=str, default='1', help="number of patterns")
parser.add_argument("-a", "--algorithm", dest='a', type=str, default='bloom_faoso_std_h1h2', help="algorithm[s] to be tested")
parser.add_argument("-c", "--corpus", dest='c', type=str, default='english.50MB', help="corpus")
parser.add_argument("-m", "--length", dest='m', type=str, default='8,16,32,64', help="pattern length[s] (e.g. 8,16,32)")
parser.add_argument("-u", "--faosou", dest='u', type=str, default='4', help="FAOSO parameter U")
parser.add_argument("-k", "--faosok", dest='k', type=str, default='2', help="FAOSO parameter k")
parser.add_argument("-q", "--q-gram", dest='q', type=str, default='2', help="q-gram size")
parser.add_argument("-s", "--sigma", dest='s', type=str, default='4', help="dest. alph. size (sigma)")
parser.add_argument("-b", "--bsize", dest='b', type=str, default='8192', help="block size")
parser.add_argument("-cp", "--cparam", dest='cp', type=str, default='6', help="c param")
parser.add_argument("-g", "--gparam", dest='g', type=str, default='1', help="g param")
parser.add_argument("-p", "--pparam", dest='p', type=str, default='0', help="p param")
parser.add_argument("-w", "--wsize", dest='w', type=str, default='0', help="w param")


args = parser.parse_args()


set_list = args.c.split(',')
a_list   = args.a.split(',')
r_list   = args.r.split(',')
m_list   = args.m.split(',')
u_list   = args.u.split(',')
k_list   = args.k.split(',')
q_list        = args.q.split(',')
sig_list        = args.q.split(',')
bs_list       = args.b.split(',')
cp_list       = args.cp.split(',')
g_list        = args.g.split(',')
p_list        = args.p.split(',')
w_list        = args.w.split(',')

for s in set_list:
	if check_corpus_existance(s) == False:
		print("Error: corpus {} does NOT exists".format(s))
		exit(100)

for s in set_list:
	for r in r_list:
		for m in m_list:
			check_patterns_existance(s, r, m)



print('A\tS\tR\tQ\tM\tU\tK\tc\tg\tp\tw_size\tBloomSize\tBlockSize\t' + \
	'#Blocks\tVerifiedBlocks\tTxtFileSize\tTxt+Bloom\t'  + 'Ver\tMatches\t' + \
	'Building\tBloom\tFaoso\tB+F\tBuilding [MBs]\tBloom [MBs]\tFaoso [MBs]\tB+F [MBs]')
for a in a_list:
	for bs in bs_list:
		for sig in sig_list:
			for q in q_list:
				for s in set_list:
					for r in r_list:
						for m in m_list:
							for u in u_list:
								for k in k_list:
									for cp in cp_list:
										for g in g_list:
											for p in p_list:
												for w_size in w_list:
													proc_filename    = "{}/{}".format(alg_loc, a)
													pattern_filename = "{}/patterns.r{}/patterns.{}.{}.bin".format(pattern_loc, r, s, m)
													corpus_filename     = "{}/{}".format(set_loc, s)

													if not os.path.isfile(proc_filename):
														print("Error: The prog file not found ({})".format(proc_filename))
														exit()
													if not os.path.isfile(pattern_filename):
														print("Error: The pattern file not found ({})".format(pattern_filename))
														exit()
													if not os.path.isfile(corpus_filename):
														print("Error: The set file not found ({})".format(corpus_filename))
														exit()

													sys.stdout.flush()
													proc = subprocess.Popen([proc_filename, pattern_filename, m, corpus_filename, u, k, q, sig, bs, cp, g, p, w_size], stdout=subprocess.PIPE)
													output = proc.stdout.read()
													output = '{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}'.format(a, s, r, q, m, u, k, cp, g, p, w_size, output.decode('ascii'));
													sys.stdout.write(output)
													sys.stdout.flush()
print()
print("Removing filters")
os.system("rm -f {}/*.bf".format(set_loc))
print("Completed")
