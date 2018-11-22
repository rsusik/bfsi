bloom_faoso_std_h1h2: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=1 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_std_h1h2 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_std_h1h2_th1: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=1 -DNUMBER_OF_THREADS=1 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_std_h1h2_th1 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_std_h1h2_th2: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=1 -DNUMBER_OF_THREADS=2 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_std_h1h2_th2 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_std_h1h2_th4: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=1 -DNUMBER_OF_THREADS=4 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_std_h1h2_th4 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_std_h1h2_th6: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=1 -DNUMBER_OF_THREADS=6 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_std_h1h2_th6 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_std_h1h2_th8: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=1 -DNUMBER_OF_THREADS=8 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_std_h1h2_th8 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_std_h1h2_th10: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=1 -DNUMBER_OF_THREADS=10 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_std_h1h2_th10 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_std_h1h2_th12: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=1 -DNUMBER_OF_THREADS=12 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_std_h1h2_th12 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 
	
bloom_faoso_eq_h1h2: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=2 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_eq_h1h2 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 
	
bloom_faoso_eq_h1h2_th1: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=2 -DNUMBER_OF_THREADS=1 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_eq_h1h2_th1 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_eq_h1h2_th2: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=2 -DNUMBER_OF_THREADS=2 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_eq_h1h2_th2 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_eq_h1h2_th4: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=2 -DNUMBER_OF_THREADS=4 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_eq_h1h2_th4 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_eq_h1h2_th6: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=2 -DNUMBER_OF_THREADS=6 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_eq_h1h2_th6 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_eq_h1h2_th8: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=2 -DNUMBER_OF_THREADS=8 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_eq_h1h2_th8 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_eq_h1h2_th10: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=2 -DNUMBER_OF_THREADS=10 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_eq_h1h2_th10 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_eq_h1h2_th12: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=2 -DNUMBER_OF_THREADS=12 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_eq_h1h2_th12 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_eq_h1h2_all: bloom_faoso_eq_h1h2 bloom_faoso_eq_h1h2_th1 bloom_faoso_eq_h1h2_th2 bloom_faoso_eq_h1h2_th4 bloom_faoso_eq_h1h2_th6 bloom_faoso_eq_h1h2_th8 bloom_faoso_eq_h1h2_th10 bloom_faoso_eq_h1h2_th12

bloom_faoso_std_h1h2_all: bloom_faoso_std_h1h2 bloom_faoso_std_h1h2_th1 bloom_faoso_std_h1h2_th2 bloom_faoso_std_h1h2_th4 bloom_faoso_std_h1h2_th6 bloom_faoso_std_h1h2_th8 bloom_faoso_std_h1h2_th10 bloom_faoso_std_h1h2_th12


bloom_faoso_min_h1h2: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=3 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_min_h1h2 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_min_h1h2_th1: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=3 -DNUMBER_OF_THREADS=1 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_min_h1h2_th1 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_min_h1h2_th2: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=3 -DNUMBER_OF_THREADS=2 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_min_h1h2_th2 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_min_h1h2_th4: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=3 -DNUMBER_OF_THREADS=4 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_min_h1h2_th4 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_min_h1h2_th6: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=3 -DNUMBER_OF_THREADS=6 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_min_h1h2_th6 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_min_h1h2_th8: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=3 -DNUMBER_OF_THREADS=8 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_min_h1h2_th8 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_min_h1h2_th10: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=3 -DNUMBER_OF_THREADS=10 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_min_h1h2_th10 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 

bloom_faoso_min_h1h2_th12: bloom_faoso.cpp bloom_faoso.h bloom.h common.h xxhash.h xxhash.cpp
	g++ -O3 -std=c++11 -Wno-unused-result -lpthread -pthread -DVARIANT=3 -DNUMBER_OF_THREADS=12 -DHASH_FUNCTION=hashFunction_h1h2 -o bloom_faoso_min_h1h2_th12 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 
	
all:      bloom_faoso_std_h1h2 bloom_faoso_eq_h1h2 bloom_faoso_min_h1h2 bloom_faoso_min_h1h2_th1 bloom_faoso_min_h1h2_th2 bloom_faoso_min_h1h2_th4 bloom_faoso_min_h1h2_th6 bloom_faoso_min_h1h2_th8 bloom_faoso_min_h1h2_th10 bloom_faoso_min_h1h2_th12 bloom_faoso_eq_h1h2_th1 bloom_faoso_eq_h1h2_th2 bloom_faoso_eq_h1h2_th4 bloom_faoso_eq_h1h2_th6 bloom_faoso_eq_h1h2_th8 bloom_faoso_eq_h1h2_th10 bloom_faoso_eq_h1h2_th12 bloom_faoso_std_h1h2_th1 bloom_faoso_std_h1h2_th2 bloom_faoso_std_h1h2_th4 bloom_faoso_std_h1h2_th6 bloom_faoso_std_h1h2_th8 bloom_faoso_std_h1h2_th10 bloom_faoso_std_h1h2_th12

clean:
	rm -f bloom_faoso_std_h1h2 bloom_faoso_eq_h1h2 bloom_faoso_min_h1h2 bloom_faoso_min_h1h2_th1 bloom_faoso_min_h1h2_th2 bloom_faoso_min_h1h2_th4 bloom_faoso_min_h1h2_th6 bloom_faoso_min_h1h2_th8 bloom_faoso_min_h1h2_th10 bloom_faoso_min_h1h2_th12 bloom_faoso_eq_h1h2_th1 bloom_faoso_eq_h1h2_th2 bloom_faoso_eq_h1h2_th4 bloom_faoso_eq_h1h2_th6 bloom_faoso_eq_h1h2_th8 bloom_faoso_eq_h1h2_th10 bloom_faoso_eq_h1h2_th12 bloom_faoso_std_h1h2_th1 bloom_faoso_std_h1h2_th2 bloom_faoso_std_h1h2_th4 bloom_faoso_std_h1h2_th6 bloom_faoso_std_h1h2_th8 bloom_faoso_std_h1h2_th10 bloom_faoso_std_h1h2_th12