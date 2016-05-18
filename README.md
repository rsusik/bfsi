# Bloom Filter based Semi-Index (BFSI)

## About

This is a source code of Bloom filter based semi-index on *q*-grams. It was written for research (http://arxiv.org/abs/1507.02989) purpose and has a minimal error checking. The code may be not very readable and comments may not be adequate. There is no warranty, your use of this code is at your own risk. BFSI use FAOSO for verification (searching in block) which was originally written by Kimmo Fredriksson.

There are three variants of BFSI:
* STD
* SAM (sampling text every s*th* *q*-gram)
* MSAM (using minimizers)

## Requirements

* C++ compiler compatible with C++11 standard
* Unix based 64-bit OS (compilation works also in Cygwin)
* Multithreading CPU



## Compilation

Algorithm accepts several parameters on compilation level which are:
* VARIANT - the variant of algorithm
  * STANDARD (1) - STD
  * EVERY_G_QGRAM (2) - SAM
  * ON_MINIMIZERS (3) - MSAM
* NUMBER_OF_THREADS (1, 2, 4, etc.) - number of threads used in index building process

To compile the code run below line (example):

```shell
g++ -O3 -std=c++11 -lpthread -pthread -DVARIANT=1 -DNUMBER_OF_THREADS=4 -o bloom_faoso_std_th4 bloom_faoso.cpp xxhash.h xxhash.cpp -L ./ -I ./ -lrt 
```

Above command will compile the STD variant, where 4 threads will be used for index building process.


## Execution

There are a number of parameters to be specified to execute the code. The parameters should be separated by space and order matters:

1. File with patterns (patterns are not separated and equal)
2. Pattern size
3. Text file
4. FAOSO-U parameter (FAOSO parameter, in all tests we use 4)
5. FAOSO-k parameter (FAOSO parameter, in all tests we use 2)
6. *q*-Gram size
7. Sigma (not applicable for FAOSO)
8. Block size (size of a block, in bytes, e.g., 8192)
9. c (number of bits per item in a Bloom Filter)
10. s (frequency of *q*-gram sampling, should be set to 1 for STD variant, 0 for MSAM and all values > 1 for SAM) (*only for SAM*)
11. Size of minimizer (p) (*only for MSAM*)
12. Window size (w) (*only for MSAM*)

For more details please refer to mentioned paper.

Example:
```bash
./bloom_faoso_std_h1h2_th4 /media/disk1/patterns.m16 16 /media/disk1/english.200MB 8 2 6 5 8192 6 1 0 0
```

## Authors

* Szymon Grabowski
* Robert Susik
* [Marcin Raniszewski](https://github.com/mranisz)
