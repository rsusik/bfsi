#ifndef BLOOM_COMMON_H
#define BLOOM_COMMON_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <string.h>
#include <vector>
#include <thread>
#include "xxhash.h"
#include "common.h"
#include "hash.h"

#ifndef NUMBER_OF_THREADS
	#define NUMBER_OF_THREADS 1
#endif

#ifndef PART_SIZE
	#define PART_SIZE 256
#endif


namespace bloom {

	// typedef 
	struct TText {
		unsigned int
			number_of_blocks,
			length,
			length_ext,
			max_m;
		unsigned long block_size;
		uchar *text;		// content of text file

		TText(): text(NULL), number_of_blocks(0), length(0), length_ext(0), max_m(0), block_size(0) { }

		~TText() {
			if(text) free(text); text = NULL;
			number_of_blocks = length = length_ext = max_m = block_size = 0;
		}

		static void printValues( TText *t ) {
			printf("t->number_of_blocks = %d\n", t->number_of_blocks);
			printf("t->length = %d\n", t->length);
			printf("t->length_ext = %d\n", t->length_ext);
			printf("t->max_m = %d\n", t->max_m);
			printf("t->block_size = %lu\n", t->block_size);
		}

	};

	/// Never return by value
	// typedef 
	struct TBloomFilters {
		unsigned int 
			max_m, 
			bloom_size;	// size of bloom filter
		unsigned long 
			filters_size,
			number_of_64bit_elememts,
			number_of_all_64bit_elements,
			number_of_unique_qgrams;
		uint64_t *bloom_filters;	// content of bfFilename (skipping first 4 bytes)

		TBloomFilters() : max_m(0), bloom_size(0), filters_size(0), number_of_64bit_elememts(0), number_of_all_64bit_elements(0), bloom_filters( NULL ) { }

		~TBloomFilters() {
			if(bloom_filters) free(bloom_filters); bloom_filters = NULL;
			max_m = bloom_size = filters_size = number_of_64bit_elememts = number_of_all_64bit_elements = 0;
		}
	} ;

	/// Result of readPatterns function
	/// Never return by value
	// typedef 
	struct TPatterns {
		int 
			length,	// 
			number_of_patterns,	// number of patterns
			m;					// pattern length
		char 
			*patterns;	// content of patterns file

		TPatterns() : length(0), number_of_patterns(0), m(0), patterns(NULL) { }

		~TPatterns() {
			if(patterns) free(patterns); patterns = NULL;
			length = number_of_patterns = m = 0;
		}

	} ;

	// typedef 
	struct TBloomResult {
		unsigned int length;
		long *blocks_to_verify;

		TBloomResult() : length(0), blocks_to_verify(NULL) { }

		~TBloomResult() {
			if(blocks_to_verify) free(blocks_to_verify); blocks_to_verify = NULL; length = 0;
		}
	} ;

	template <unsigned int Q, unsigned int SIGMA> struct get_ { 
		static uint64_t gram(const uchar * s, const uint64_t start /* at start set to 1 */) {
			return s[Q - 1] * start + get_<Q - 1, SIGMA>::gram(s, start*SIGMA);
		}
	};

	template <unsigned int SIGMA> struct get_<0, SIGMA> { 
		static uint64_t gram(const uchar * s, const uint64_t start /* at start set to 1 */) { return 0; }
	};

	template <unsigned int Q, unsigned int SIGMA> uint64_t get_q_gram(const uchar *s) {
		return get_<Q, SIGMA>::gram(s, 1);
	};

	uint64_t get_q_gram_wrapper( unsigned int q, const uchar *s ) {
		if  (q==1) {
			return get_q_gram<1, 255>(s);
		} else if (q==2) {
			return get_q_gram<2, 255>(s);
		} else if (q==3) {
			return get_q_gram<3, 255>(s);
		} else if (q==4) {
			return get_q_gram<4, 255>(s);
		} else if (q==5) {
			return get_q_gram<5, 255>(s);
		} else if (q==6) {
			return get_q_gram<6, 255>(s);
		} else if (q==7) {
			return get_q_gram<7, 255>(s);
		} else if (q==8) {
			return get_q_gram<8, 255>(s);
		} else if (q==9) {
			return get_q_gram<9, 255>(s);
		} else if (q==10) {
			return get_q_gram<10, 255>(s);
		} else {
			printf("Erorr: Not supported q\n"); exit(EXIT_FAILURE);
		}
	}

	// Create bloom

	static inline int getSizeOfOverlay(int m, int q_size) {
		return 128;
	}
	
	unsigned int countUniqueQGramsInBlockPrecise(
		const uchar *text, const unsigned int block_number, const unsigned long block_size, 
		const unsigned int max_m, const unsigned int q_size,
		const unsigned int g
	) {
		if(q_size <= 8) {
			std::vector<unsigned long long int> qgrams;
			for (int i = block_number * block_size; i < (block_number + 1) * block_size + getSizeOfOverlay(max_m, q_size); i+=g) {
				qgrams.push_back(bloom::get_q_gram_wrapper(q_size, text + i));
			}
			std::sort( qgrams.begin(), qgrams.end() );
			std::vector<unsigned long long int>::iterator it;
			it = unique( qgrams.begin(), qgrams.end() );
			return (it - qgrams.begin());
		} else {
			std::vector<std::string> qgrams;
			char* qgram = new char[q_size + 1];
			qgram[q_size] = '\0';
			for (int i = block_number * block_size; i < (block_number + 1) * block_size + getSizeOfOverlay(max_m, q_size); i+=g) {
				strncpy(qgram, (char*)text + i, q_size);
				qgrams.push_back((std::string)qgram);
			}
			std::sort(qgrams.begin(), qgrams.end());
			std::string str = qgrams[0];
			int unique_qgrams = 1;
			for (int i = 1; i < qgrams.size(); i++) {
				if (qgrams[i] != str) {
					++unique_qgrams;
					str = qgrams[i];
				}
			}
			delete[] qgram;
			return unique_qgrams;
		}
	}
	
	void countUniqueQGramsInBlockPrecise_task(uchar *text, unsigned long long* sizes, unsigned int start_block, unsigned int end_block, unsigned long long block_size, unsigned int max_m, const unsigned int q_size, const unsigned int g) {
		
		for (int i = start_block; i < end_block; ++i) {
			sizes[i] = countUniqueQGramsInBlockPrecise(text, i, block_size, max_m, q_size, g);
		}
		
	}
	
	unsigned long inline getNumberOfAll64bitElements(const unsigned long number_of_64bit_elememts, const unsigned long bloom_size) {
		return number_of_64bit_elememts * bloom_size;
	}

	unsigned long inline getNumberOf64bitElememts(unsigned long number_of_blocks) {
		return number_of_blocks / 64 + ((number_of_blocks % 64)?1:0);
	}
	
	// Generate all Bloom Filters
	void generateBloomFilters_task(const uchar *text, bool **my_BF, const unsigned int bloom_size, const unsigned int start_block, const unsigned int end_block, const unsigned long long block_size, const unsigned int max_m, const unsigned int q_size, const unsigned int g, const int k) {
		
		char* qgram = new char[q_size + 1];
		qgram[q_size] = '\0';
		for (int i = start_block; i < end_block; ++i) {
			for (int j = 0; j < block_size + max_m - 1; j+=g) {
				memcpy(qgram, text + i * block_size + j, q_size);
				for (int h = 0; h < k; ++h) {
					my_BF[i][bloom::hashFunction(h, (unsigned char *)qgram) % bloom_size] = true;
				}
			}
		}
		delete [] qgram;
	}
	
	// Make an interlace (not packed)
	void makeInterlacedBF_task(bool **my_interlaced, bool **my_BF, const unsigned int bloom_size, const unsigned int start_block, const unsigned int end_block) {
		int bloom_parts = bloom_size / PART_SIZE;
		int curr_start = 0;
		for (int temp = 0; temp < bloom_parts; temp++) {
			for (int i = start_block; i < end_block; ++i) {
				for(int j = curr_start; j < curr_start + PART_SIZE; j++) {
					if(my_BF[i][j]==true) {
						my_interlaced[j][i] = true;
					}
				}
			}
			curr_start += PART_SIZE;
		}
	
		// ost. czesc moze byc krotsza, wiec osobno
		for (int i = start_block; i < end_block; ++i) {
			for(int j = curr_start; j < bloom_size; j++) {
				if(my_BF[i][j]==true) {
					my_interlaced[j][i] = true;
				}
			}
		}
		
	}
	
	// Pack to 64-bit portions
	void packInterlacedBF_task(uint64_t *bloom_filters, bool **my_interlaced, const unsigned int bloom_size, 
		const unsigned int startPoint, const unsigned int endPoint, const unsigned long number_of_64bit_elememts, const unsigned long number_of_blocks
	) {
		for(int j = 0; j < bloom_size; j++) {
			for (int i = 0; i < number_of_blocks; ++i) {
				if(my_interlaced[j][i]) {
					unsigned int bNum = j * number_of_64bit_elememts + i / 64;
					bloom_filters[bNum] |= (uint64_t)1 << i % 64;
				}
			}
		}
	}
	
	/// Sets p->BF, p->number_of_64bit_elememts, p->all64bitElements, p->bFSize
	TBloomFilters *createInterlacedBloomFiltersEveryQGramParallel(TText *text, unsigned int q_size, unsigned int c, unsigned int g /* every g gram */) {
		TBloomFilters *res = new TBloomFilters();
		res->max_m = text->max_m;
		
		int k = get_k(c);
		unsigned long number_of_unique_qgrams = 0;
		
		unsigned int blocks_per_thread, blocks_per_last_thread;
		bool blocks_per_last_thread_different = false;
		if(text->number_of_blocks % NUMBER_OF_THREADS != 0) {
			blocks_per_last_thread_different = true;
			blocks_per_thread              = text->number_of_blocks / NUMBER_OF_THREADS;
			blocks_per_last_thread          = text->number_of_blocks % NUMBER_OF_THREADS;
		} else {
			blocks_per_thread              = text->number_of_blocks / NUMBER_OF_THREADS;
		}
		
		unsigned long long* sizes        = new unsigned long long[text->number_of_blocks];
		
		std::vector<std::thread> threads;
		
		for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
			threads.push_back(std::thread(countUniqueQGramsInBlockPrecise_task, (uchar*)text->text, sizes, (i*blocks_per_thread), (i*blocks_per_thread) + blocks_per_thread, text->block_size, text->max_m, q_size, g));
		}
		
		for (auto& th : threads) th.join();
		
		if (blocks_per_last_thread_different) {
			countUniqueQGramsInBlockPrecise_task( (uchar*)text->text, sizes, (NUMBER_OF_THREADS*blocks_per_thread), (NUMBER_OF_THREADS*blocks_per_thread) + blocks_per_last_thread, text->block_size, text->max_m, q_size, g);
		}
		
		threads.clear();
		
		for (int i = 0; i < text->number_of_blocks - 1; ++i) {
			number_of_unique_qgrams += sizes[i];
		}
		
		delete [] sizes;
		
		// Generate all Bloom Filters
		res->number_of_unique_qgrams = number_of_unique_qgrams;
		int n = (int)((float)number_of_unique_qgrams / (float)(text->number_of_blocks - 1));
		res->bloom_size = n * c;

		bool **my_BF = (bool**)calloc(text->number_of_blocks, sizeof(bool*));
		for(int i = 0; i < text->number_of_blocks; i++) {
			my_BF[i] = (bool*)calloc(res->bloom_size, sizeof(bool));
		}
		
		for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
			threads.push_back(std::thread(generateBloomFilters_task, (uchar*)text->text, my_BF, res->bloom_size, (i*blocks_per_thread)             , (i*blocks_per_thread) + blocks_per_thread                   , text->block_size, text->max_m, q_size, g, k));
		}
		
		for (auto& th : threads) th.join();
		threads.clear();
		
		if (blocks_per_last_thread_different) {
			generateBloomFilters_task( (uchar*)text->text, my_BF, res->bloom_size, (NUMBER_OF_THREADS*blocks_per_thread), (NUMBER_OF_THREADS*blocks_per_thread) + blocks_per_last_thread, text->block_size, text->max_m, q_size, g, k);
		}
		
		// Make an interlace (not packed)
		bool **my_interlaced = (bool**)calloc(res->bloom_size, sizeof(bool*));
		for(int i = 0; i < res->bloom_size; i++) {
			my_interlaced[i] = (bool*)calloc(text->number_of_blocks, sizeof(bool));
		}
		
		for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
			threads.push_back(std::thread(makeInterlacedBF_task, my_interlaced, my_BF, res->bloom_size, (i*blocks_per_thread)                , (i*blocks_per_thread) + blocks_per_thread                  ));
		}
		
		for (auto& th : threads) th.join();
		
		threads.clear();
		
		if (blocks_per_last_thread_different) {
			makeInterlacedBF_task( my_interlaced, my_BF, res->bloom_size, (NUMBER_OF_THREADS*blocks_per_thread), (NUMBER_OF_THREADS*blocks_per_thread) + blocks_per_last_thread);
		}

		// Free memory
		for(int i = 0; i < text->number_of_blocks; i++) { free(my_BF[i]); } free(my_BF);

		// Pack to 64-bit portions
		res->number_of_64bit_elememts = getNumberOf64bitElememts(text->number_of_blocks);	// number of 64-bit blocks.
		res->number_of_all_64bit_elements = getNumberOfAll64bitElements(res->number_of_64bit_elememts, res->bloom_size);
		res->bloom_filters = (uint64_t*)calloc(res->number_of_all_64bit_elements, sizeof(uint64_t));
		res->filters_size = sizeof(uint64_t)*res->number_of_all_64bit_elements;
		
		unsigned int cells_per_thread, cells_per_last_thread;
		bool cells_per_last_threadDifferent = false;
		if(res->bloom_size % NUMBER_OF_THREADS != 0) {
			cells_per_last_threadDifferent = true;
			cells_per_thread              = res->bloom_size / NUMBER_OF_THREADS;
			cells_per_last_thread          = res->bloom_size % NUMBER_OF_THREADS;
		} else {
			cells_per_thread              = res->bloom_size / NUMBER_OF_THREADS;
		}
		
		for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
			threads.push_back(std::thread(packInterlacedBF_task, res->bloom_filters, my_interlaced, res->bloom_size, (i*cells_per_thread)                  , (i*cells_per_thread) + cells_per_thread                      , res->number_of_64bit_elememts, text->number_of_blocks));
		}
		
		for (auto& th : threads) th.join();
		threads.clear();
		
		if (cells_per_last_threadDifferent) {
			packInterlacedBF_task( res->bloom_filters, my_interlaced, res->bloom_size, (NUMBER_OF_THREADS*cells_per_last_thread), (NUMBER_OF_THREADS*cells_per_last_thread) + cells_per_last_thread, res->number_of_64bit_elememts, text->number_of_blocks);
		}
		
		// Free memory
		for(int i = 0; i < res->bloom_size; i++) { free(my_interlaced[i]); } free(my_interlaced);
		
		return res;
	}

	inline uchar *getMinimizer(uchar* text, int w_size, int p) {
		uchar *min = text;
		for(uchar *j = text; j < text + w_size; j++) {
			if( memcmp(j, min, p) < 0) {
				min = j;
			}
		}
		return min;
	}

	std::vector<uchar*> getMinimizers(uchar *text, int n, int w_size, int p, int q) {
		std::vector<uchar*> min_pos;
		uchar *min = NULL;
		int min_q_p = q > p ? q : p;
		for (int i = 0; i < n - w_size - min_q_p; i++) {
			//min = getMinimizer(text+i, w_size, p, q);
			min = getMinimizer(text+i, w_size, p);
			min_pos.push_back( min );
		}

		std::sort( min_pos.begin(), min_pos.end() );
		min_pos.erase( unique( min_pos.begin(), min_pos.end() ), min_pos.end() );
		return min_pos;
	}
	
	unsigned int countUniqueQGramsBasedOnMinimizerInBlock(uchar *text, int n, int w_size, int p, int q) {
		std::vector<unsigned long long> min_pos;
		uchar *min = NULL;
		for (int i = 0; i < n - w_size; i++) {
			min = getMinimizer(text+i, w_size, p);
			min_pos.push_back( get_q_gram_wrapper(q, (uchar*)min) );
		}

		std::sort( min_pos.begin(), min_pos.end() );
		std::vector<unsigned long long int>::iterator it;
		it = unique( min_pos.begin(), min_pos.end() );
		return (it - min_pos.begin());
	}

	void countUniqueQGramsBasedOnMinimizerInBlock_task(uchar *text, unsigned long long* sizes, unsigned int start_block, unsigned int end_block, unsigned long long block_size, unsigned int max_m, const unsigned int q_size, const unsigned int p, const unsigned int wSize) {
		
		for (int i = start_block; i < end_block; ++i) {
			int start = i * block_size;
			sizes[i] = countUniqueQGramsBasedOnMinimizerInBlock(text + start, block_size + getSizeOfOverlay(max_m, q_size), wSize, p, q_size);
		}
		
	}

	// Generate all Bloom Filters
	void generateBloomFiltersOnMinimizers_task(uchar *text, bool **my_BF, const unsigned int bloom_size, const unsigned int start_block, const unsigned int end_block, const unsigned long long block_size, const unsigned int max_m, const unsigned int q_size, const int k, const unsigned int wSize, const unsigned int p) {
		
		uchar *min = NULL;
		uchar *qgram = new uchar[q_size + 1];
		qgram[q_size] = '\0';
		for (int b = start_block; b < end_block; b++) {
			for (int i = block_size*b; i < block_size*b + block_size - wSize + getSizeOfOverlay(max_m, q_size); i++) {
				min = getMinimizer(text+i, wSize, p);	// find minimizer in window
				memcpy(qgram, min, q_size);
				for (int h = 0; h < k; ++h) {
					my_BF[b][bloom::hashFunction(h, (uchar *)qgram) % bloom_size] = true;
				}
			}
		}
		free(qgram);
	}
	
	// Make an interlace (not packed)
	void makeInterlacedBFOnMinimizers_task(bool **my_interlaced, bool **my_BF, const unsigned int bloom_size, const unsigned int start_block, const unsigned int end_block) {

		 // mozna tez sprobowac np. 64, a moze i wiecej
		int bloom_parts = bloom_size / PART_SIZE;
		int curr_start = 0;
		for (int temp = 0; temp < bloom_parts; temp++) {
			for (int i = start_block; i < end_block; ++i) {
				for(int j = curr_start; j < curr_start + PART_SIZE; j++) {
					if(my_BF[i][j]==true) {
						my_interlaced[j][i] = true;
					}
				}
			}
			curr_start += PART_SIZE;
		}
	
		// ost. czesc moze byc krotsza, wiec osobno
		for (int i = start_block; i < end_block; ++i) {
			for(int j = curr_start; j < bloom_size; j++) {
				if(my_BF[i][j]==true) {
					my_interlaced[j][i] = true;
				}
			}
		}
		
	}
	
	// Pack to 64-bit portions
	void packInterlacedBFOnMinimizers_task(uint64_t *bloom_filters, bool **my_interlaced, const unsigned int bloom_size, 
		const unsigned int startPoint, const unsigned int endPoint, const unsigned long number_of_64bit_elememts, const unsigned long number_of_blocks
	) {
		for(int j = 0; j < bloom_size; j++) {
			for (int i = 0; i < number_of_blocks; ++i) {
				if(my_interlaced[j][i]) {
					unsigned int bNum = j * number_of_64bit_elememts + i / 64;
					bloom_filters[bNum] |= (uint64_t)1 << i % 64;
				}
			}
		}
	}
	
	// p - minimizer size
	// wSize - window size
	TBloomFilters *createInterlacedBloomFiltersOnMinimizersParallel(TText *text, unsigned int q_size, unsigned int c, unsigned int p , unsigned int wSize ) {
		
		if(text->max_m < wSize + (q_size - p) || text->max_m < p || text->max_m < q_size || text->max_m < wSize) { printf("Error: m < wSize + (q_size - p) || text->max_m < p || text->max_m < q_size || text->max_m < wSize { wSize=%d, m=%d, q_size=%d, p=%d }\n", wSize, text->max_m, q_size, p); exit(EXIT_FAILURE); }
		if(p<=0 || wSize<=0) { printf("Error: p = %d <= 0 || wSize = %d <= 0\n", p, wSize); exit(EXIT_FAILURE); }

		TBloomFilters *res = new TBloomFilters();
		res->max_m = text->max_m;
		int k = get_k(c);
		unsigned long number_of_unique_qgrams = 0;

		unsigned int blocks_per_thread, blocks_per_last_thread, numberOfThreads = NUMBER_OF_THREADS;
		bool blocks_per_last_thread_different = false;
		if(text->number_of_blocks % NUMBER_OF_THREADS != 0) {
			numberOfThreads              = NUMBER_OF_THREADS - 1;
			blocks_per_last_thread_different = true;
			blocks_per_thread              = text->number_of_blocks / numberOfThreads;
			blocks_per_last_thread          = text->number_of_blocks % numberOfThreads;
		} else {
			blocks_per_thread              = text->number_of_blocks / NUMBER_OF_THREADS;
		}
		
		unsigned long long* sizes        = new unsigned long long[text->number_of_blocks];
		
		std::vector<std::thread> threads;
		
		for (int i = 0; i < numberOfThreads; ++i) {
			threads.push_back(std::thread(countUniqueQGramsBasedOnMinimizerInBlock_task, (uchar*)text->text, sizes, (i*blocks_per_thread)              , (i*blocks_per_thread) + blocks_per_thread                  , text->block_size, text->max_m, q_size, p, wSize));
		}
		if (blocks_per_last_thread_different) {
			threads.push_back(std::thread(countUniqueQGramsBasedOnMinimizerInBlock_task, (uchar*)text->text, sizes, (numberOfThreads*blocks_per_thread), (numberOfThreads*blocks_per_thread) + blocks_per_last_thread, text->block_size, text->max_m, q_size, p, wSize));
		}
		
		for (auto& th : threads) th.join();
		
		threads.clear();
		
		for (int i = 0; i < text->number_of_blocks - 1; ++i) {
			number_of_unique_qgrams += sizes[i];
		}
		
		delete [] sizes;
		
		res->number_of_unique_qgrams = number_of_unique_qgrams;
		int n = (int)((float)number_of_unique_qgrams / (float)(text->number_of_blocks));
		res->bloom_size = n * c;

		bool **my_BF = (bool**)calloc(text->number_of_blocks, sizeof(bool*));
		for(int i = 0; i < text->number_of_blocks; i++) {
			my_BF[i] = (bool*)calloc(res->bloom_size, sizeof(bool));
		}

		// Generate all Bloom Filters
		for (int i = 0; i < numberOfThreads; ++i) {
			threads.push_back(std::thread(generateBloomFiltersOnMinimizers_task, (uchar*)text->text, my_BF, res->bloom_size, (i*blocks_per_thread)             , (i*blocks_per_thread) + blocks_per_thread                   , text->block_size, text->max_m, q_size, k, wSize, p));
		}
		if (blocks_per_last_thread_different) {
			threads.push_back(std::thread(generateBloomFiltersOnMinimizers_task, (uchar*)text->text, my_BF, res->bloom_size, (numberOfThreads*blocks_per_thread), (numberOfThreads*blocks_per_thread) + blocks_per_last_thread, text->block_size, text->max_m, q_size, k, wSize, p));
		}
		
		for (auto& th : threads) th.join();
		
		threads.clear();

		// Make an interlace (not packed)
		bool **my_interlaced = (bool**)calloc(res->bloom_size, sizeof(bool*));
		if(my_interlaced==NULL) { printf("Error: Cannot allocate memory for my_interlaced\n"); exit(EXIT_FAILURE); }
		for(int i = 0; i < res->bloom_size; i++) {
			my_interlaced[i] = (bool*)calloc(text->number_of_blocks, sizeof(bool));
			if(my_interlaced[i]==NULL) { printf("Error: Cannot allocate memory for my_interlaced[i]\n"); exit(EXIT_FAILURE); }
		}
		
		for (int i = 0; i < numberOfThreads; ++i) {
			threads.push_back(std::thread(makeInterlacedBFOnMinimizers_task, my_interlaced, my_BF, res->bloom_size, (i*blocks_per_thread)              , (i*blocks_per_thread) + blocks_per_thread                  ));
		}
		if (blocks_per_last_thread_different) {
			threads.push_back(std::thread(makeInterlacedBFOnMinimizers_task, my_interlaced, my_BF, res->bloom_size, (numberOfThreads*blocks_per_thread), (numberOfThreads*blocks_per_thread) + blocks_per_last_thread));
		}
		
		for (auto& th : threads) th.join();
		
		threads.clear();

		// Free memory
		for(int i = 0; i < text->number_of_blocks; i++) { free(my_BF[i]); } free(my_BF);
		
		// Pack to 64-bit portions
		res->number_of_64bit_elememts = getNumberOf64bitElememts(text->number_of_blocks);	// number of 64-bit blocks.
		res->number_of_all_64bit_elements = getNumberOfAll64bitElements(res->number_of_64bit_elememts, res->bloom_size);
		
		res->bloom_filters = (uint64_t*)calloc(res->number_of_all_64bit_elements, sizeof(uint64_t));
		if(res->bloom_filters==NULL) { printf("Error: Cannot allocate memory for res->bloom_filters\n"); exit(EXIT_FAILURE); }
		res->filters_size = sizeof(uint64_t)*res->number_of_all_64bit_elements;
		
		unsigned int cells_per_thread, cells_per_last_thread;
		numberOfThreads = NUMBER_OF_THREADS;
		bool cells_per_last_threadDifferent = false;
		if(res->bloom_size % NUMBER_OF_THREADS != 0) {
			numberOfThreads              = NUMBER_OF_THREADS - 1;
			cells_per_last_threadDifferent = true;
			cells_per_thread              = res->bloom_size / numberOfThreads;
			cells_per_last_thread          = res->bloom_size % numberOfThreads;
		} else {
			cells_per_thread              = res->bloom_size / NUMBER_OF_THREADS;
		}
		
		for (int i = 0; i < numberOfThreads; ++i) {
			threads.push_back(std::thread(packInterlacedBFOnMinimizers_task, res->bloom_filters, my_interlaced, res->bloom_size, (i*cells_per_thread)                  , (i*cells_per_thread) + cells_per_thread                      , res->number_of_64bit_elememts, text->number_of_blocks));
		}
		if (cells_per_last_threadDifferent) {
			threads.push_back(std::thread(packInterlacedBFOnMinimizers_task, res->bloom_filters, my_interlaced, res->bloom_size, (numberOfThreads*cells_per_last_thread), (numberOfThreads*cells_per_last_thread) + cells_per_last_thread, res->number_of_64bit_elememts, text->number_of_blocks));
		}
		
		for (auto& th : threads) th.join();
		
		threads.clear();

		// Free memory
		for(int i = 0; i < res->bloom_size; i++) { free(my_interlaced[i]); } free(my_interlaced);

		return res;
	}

	
	// Search in bloom
	TBloomResult* findBlocksMinimizers(const uchar * pattern, const unsigned int m, const unsigned int q_size, const unsigned int c, unsigned int number_of_blocks /* it shouldn't be here */, bloom::TBloomFilters *bf, int p, int w_size) {
		if(m < w_size + (q_size - p) || m < p || m < q_size || m < w_size) {
			printf("Error: m < w_size + (q_size - p) || m < p || m < q_size || m < w_size\n");
			exit(EXIT_FAILURE);
		}
		TBloomResult *res = new TBloomResult();
		unsigned int number_of_64bit_elements = getNumberOf64bitElememts(number_of_blocks);	// number of 64-bit blocks.
		unsigned int k = get_k(c);

		/* This piece of code should be optimized */
		std::vector<uchar*> min_pos = getMinimizers((uchar*)pattern, m, w_size, p, q_size);

		// Take all (m-q+1) q-grams from pattern
		int q_grams_number = min_pos.size();
		if(q_grams_number < 1) { printf("Error: findBlocksMinimizers -> q_grams_number (%d) is less than 1\n", q_grams_number); exit(EXIT_FAILURE); }
		uchar ** q_grams = (uchar **)malloc(q_grams_number * sizeof(uchar*));
		int i = 0;
		for(std::vector<uchar*>::iterator it = min_pos.begin(); it != min_pos.end(); it++, i++) {
			q_grams[i] = (uchar*)malloc((q_size + 1) * sizeof(uchar));
			memcpy(q_grams[i], *it, q_size);
			q_grams[i][q_size] = '\0';
		}

		// For each q_gram there is k hashes that points position in intelaced blocks
		unsigned int **positions = (unsigned int **)malloc(q_grams_number * sizeof(unsigned int *));
		for (int i = 0; i < q_grams_number; i++) {
			positions[i] = (unsigned int *)malloc(k * sizeof(unsigned int ));
		}

		// Calculate all hashes for all q-grams (for each q_gram there is k hashes)
		// Hash points to a block of i-elements of BFs
		for (int i = 0; i < q_grams_number; i++) {
			for (int h = 0; h < k; h++) {
				positions[i][h] = bloom::hashFunction(h, q_grams[i]) % bf->bloom_size;	// Position of intelanced blocks.
			}
		}
		

		/*

		Make AND operation on each block that is pointed by q-gram hashes as below

		BF[positions[q-gram][hash] + 64bitBlock] <- data structure

		blocks = 
			BF[positions[0][0] + 0] BF[positions[0][0] + 1] ... BF[positions[0][0] + numberOf64bitBlocks]
		&	BF[positions[0][1] + 0] BF[positions[0][1] + 1] ... BF[positions[0][1] + numberOf64bitBlocks]
		&	...
		&	BF[positions[0][k] + 0] BF[positions[0][k] + 1] ... BF[positions[0][k] + numberOf64bitBlocks]
		&	...
		&	BF[positions[q_grams_number-1][0] + 0] BF[positions[q_grams_number-1][0] + 1] ... BF[positions[q_grams_number-1][0] + numberOf64bitBlocks]
		&	BF[positions[q_grams_number-1][1] + 0] BF[positions[q_grams_number-1][1] + 1] ... BF[positions[q_grams_number-1][1] + numberOf64bitBlocks]
		&	...
		&	BF[positions[q_grams_number-1][k] + 0] BF[positions[q_grams_number-1][k] + 1] ... BF[positions[q_grams_number-1][k] + numberOf64bitBlocks]
		*/

		res->length = 0;
		res->blocks_to_verify = (long*)malloc(1 * sizeof(long));

		long j = 0;

		uint64_t *blocks = (uint64_t *)malloc( number_of_64bit_elements * sizeof(uint64_t) );
		// initialize first
		for (int b = 0; b < number_of_64bit_elements; b++) {
			blocks[b] = bf->bloom_filters[positions[0][0]*number_of_64bit_elements + b];
		}

		// do AND operation
		for ( int i = 1; i < q_grams_number; i++) {
			for (int h = 0; h < k; h++) {
				for ( int b = 0; b < number_of_64bit_elements; b++) {
					blocks[b] &= bf->bloom_filters[positions[i][h]*number_of_64bit_elements + b];
				}
			}
		}
		bool *blocks_to_verify = (bool*)calloc(number_of_blocks, sizeof(bool));
		if(blocks_to_verify==NULL) { printf("Error: findBlocksMinimizers -> Cannot allocate memory for blocks_to_verify\n"); exit(EXIT_FAILURE); }
		unsigned long bl;
		
		for ( int a = 0; a < number_of_64bit_elements; a++) {
			while(blocks[a]) {
				clz64(&bl, blocks[a]);
				blocks_to_verify[bl + a * 64] = true; // all that blocks have to be validated (searched)
				blocks[a] ^= ((uint64_t)((uint64_t)1 << bl)); // remove not needed 1
				++res->length;
			}
		}

		if(res->length * sizeof(long) > 0) {
			res->blocks_to_verify = (long*)realloc(res->blocks_to_verify, res->length * sizeof(long));
			if(res->blocks_to_verify==NULL) { printf("Error: findBlocksMinimizers -> Cannot allocate memory (%lu) for res->blocks_to_verify\n", res->length * sizeof(long) ); exit(1); }
		}

		for (long i = 0; i < number_of_blocks; i++) {
			if (blocks_to_verify[i]) {
				res->blocks_to_verify[j++] = i;
			}
		}
		free(blocks_to_verify);
		free(blocks);
		

		for(int i = 0; i < q_grams_number; i++) { free(q_grams[i]); }
		free(q_grams);
		for(int i = 0; i < q_grams_number; i++) { free(positions[i]); }
		free(positions);
		return res;
	}

	// Search in bloom
	TBloomResult* findBlocksEveryQGram(const uchar *pattern, const unsigned int m, const unsigned int q_size, const unsigned int c, unsigned int number_of_blocks /* it shouldn't be here */, bloom::TBloomFilters *bf, int g) {
		if(m < q_size+g) { printf("Error: m(%d) < q+g(%d + %d = %d)\n", m, q_size, g, q_size+g); exit(EXIT_FAILURE); }
		TBloomResult *result = new TBloomResult();
		unsigned int number_of_64bit_elements = getNumberOf64bitElememts(number_of_blocks);	// number of 64-bit blocks.
		unsigned int k = get_k(c);
		// Take all (m-q+1) q-grams from pattern
		int q_grams_number = m - q_size + 1;
		if(q_grams_number < 1) { printf("Error: findBlocks -> q_grams_numer (%d) is less than 1\n", q_grams_number); exit(EXIT_FAILURE); }
		uchar ** q_grams = (uchar **)malloc(q_grams_number * sizeof(uchar*));
		for(int i = 0; i < q_grams_number; i++) {
			q_grams[i] = (uchar*)malloc((q_size + 1) * sizeof(uchar));
			memcpy(q_grams[i], pattern + i, q_size);
			q_grams[i][q_size] = '\0';
		}

		// For each q_gram there is k hashes that points position in intelaced blocks
		unsigned int **positions = (unsigned int **)malloc(q_grams_number * sizeof(unsigned int *));
		for (int i = 0; i < q_grams_number; i++) {
			positions[i] = (unsigned int *)malloc(k * sizeof(unsigned int ));
		}

		// Calculate all hashes for all q-grams (for each q_gram there is k hashes)
		// Hash points to a block of i-elements of BFs
		for (int i = 0; i < q_grams_number; i++) {
			for (int h = 0; h < k; h++) {
				positions[i][h] = bloom::hashFunction(h, q_grams[i]) % bf->bloom_size;	// Position of intelanced blocks.
			}
		}
	
		

		/*

		Make AND operation on each block that is pointed by q-gram hashes as below

		BF[positions[q-gram][hash] + 64bitBlock] <- data structure

		blocks = 
			BF[positions[0][0] + 0] BF[positions[0][0] + 1] ... BF[positions[0][0] + numberOf64bitBlocks]
		&	BF[positions[0][1] + 0] BF[positions[0][1] + 1] ... BF[positions[0][1] + numberOf64bitBlocks]
		&	...
		&	BF[positions[0][k] + 0] BF[positions[0][k] + 1] ... BF[positions[0][k] + numberOf64bitBlocks]
		&	...
		&	BF[positions[q_grams_number-1][0] + 0] BF[positions[q_grams_number-1][0] + 1] ... BF[positions[q_grams_number-1][0] + numberOf64bitBlocks]
		&	BF[positions[q_grams_number-1][1] + 0] BF[positions[q_grams_number-1][1] + 1] ... BF[positions[q_grams_number-1][1] + numberOf64bitBlocks]
		&	...
		&	BF[positions[q_grams_number-1][k] + 0] BF[positions[q_grams_number-1][k] + 1] ... BF[positions[q_grams_number-1][k] + numberOf64bitBlocks]
		*/

		(result)->length = 0;
		(result)->blocks_to_verify = (long*)malloc(1 * sizeof(long));
		
		bool *blocks_to_verify = (bool*)calloc(number_of_blocks, sizeof(bool));
		if(blocks_to_verify==NULL && number_of_blocks != 0) { printf("Error: findBlocks -> Cannot allocate memory for blocks_to_verify\n"); exit(EXIT_FAILURE); }
		
		long j = 0;
		for (int qg = 0; qg < g && qg < q_grams_number; qg++) {

			uint64_t *blocks = (uint64_t *)malloc( number_of_64bit_elements * sizeof(uint64_t) );
			// initialize first
			for (int b = 0; b < number_of_64bit_elements; b++) {
				blocks[b] = bf->bloom_filters[positions[qg][0]*number_of_64bit_elements + b];
			}
			// do AND operation
			for ( int i = qg; i < q_grams_number; i+=g) {
				for (int h = 0; h < k; h++) {
					for ( int b = 0; b < number_of_64bit_elements; b++) {
						blocks[b] &= bf->bloom_filters[positions[i][h]*number_of_64bit_elements + b];
					}
				}
			}

			unsigned long bl;
			
			for ( int a = 0; a < number_of_64bit_elements; a++) {
				while(blocks[a]) {
					clz64(&bl, blocks[a]);
					blocks_to_verify[bl + a * 64] = true; // all that blocks have to be validated (searched)
					blocks[a] ^= ((uint64_t)((uint64_t)1 << bl)); // remove not needed 1
				}
			}
			
			free(blocks);

		}

		for (long i = 0; i < number_of_blocks; i++) {
			if (blocks_to_verify[i]) {
				++(result)->length;
				(result)->blocks_to_verify = (long*)realloc((result)->blocks_to_verify, (result)->length * sizeof(long));
				if((result)->blocks_to_verify==NULL && (result)->length * sizeof(long)!=0) { printf("Error: findBlocks -> Cannot allocate memory (%lu) for res->blocks_to_verify (number_of_64bit_elements=%u, number_of_blocks=%u, q_grams_number=%d)\n", (result)->length * sizeof(long), number_of_64bit_elements, number_of_blocks, q_grams_number ); exit(1); }
				(result)->blocks_to_verify[j++] = i;
			}
		}
		free(blocks_to_verify);

		for(int i = 0; i < q_grams_number; i++) { free(q_grams[i]); }
		free(q_grams);
		for(int i = 0; i < q_grams_number; i++) { free(positions[i]); }
		free(positions);

		return result;
	}

	TBloomResult* findBlocks(const uchar * pattern, const unsigned int m, const unsigned int q_size, const unsigned int c, unsigned int number_of_blocks /* it shouldn't be here */, bloom::TBloomFilters *bf) {
		if(m<q_size) { printf("Error: m<q\n"); exit(EXIT_FAILURE); }
		TBloomResult *res = new TBloomResult();
		unsigned int number_of_64bit_elements = getNumberOf64bitElememts(number_of_blocks);	// number of 64-bit blocks.
		unsigned int k = get_k(c);
		// Take all (m-q+1) q-grams from pattern
		int q_grams_number = m - q_size + 1;
		if(q_grams_number < 1) { printf("Error: findBlocks -> q_grams_numer (%d) is less than 1\n", q_grams_number); exit(EXIT_FAILURE); }
		uchar ** q_grams = (uchar **)malloc(q_grams_number * sizeof(uchar*));
		for(int i = 0; i < q_grams_number; i++) {
			q_grams[i] = (uchar*)malloc((q_size + 1) * sizeof(uchar));
			memcpy(q_grams[i], pattern + i, q_size);
			q_grams[i][q_size] = '\0';
		}

		// For each q_gram there is k hashes that points position in intelaced blocks
		unsigned int **positions = (unsigned int **)malloc(q_grams_number * sizeof(unsigned int *));
		for (int i = 0; i < q_grams_number; i++) {
			positions[i] = (unsigned int *)malloc(k * sizeof(unsigned int ));
		}

		// Calculate all hashes for all q-grams (for each q_gram there is k hashes)
		// Hash points to a block of i-elements of BFs
		for (int i = 0; i < q_grams_number; i++) {
			for (int h = 0; h < k; h++) {
				positions[i][h] = bloom::hashFunction(h, q_grams[i]) % bf->bloom_size;	// Position of intelanced blocks.
			}
		}
	
		uint64_t *blocks = (uint64_t *)malloc( number_of_64bit_elements * sizeof(uint64_t) );

		/*

		Make AND operation on each block that is pointed by q-gram hashes as below

		BF[positions[q-gram][hash] + 64bitBlock] <- data structure

		blocks = 
			BF[positions[0][0] + 0] BF[positions[0][0] + 1] ... BF[positions[0][0] + numberOf64bitBlocks]
		&	BF[positions[0][1] + 0] BF[positions[0][1] + 1] ... BF[positions[0][1] + numberOf64bitBlocks]
		&	...
		&	BF[positions[0][k] + 0] BF[positions[0][k] + 1] ... BF[positions[0][k] + numberOf64bitBlocks]
		&	...
		&	BF[positions[q_grams_number-1][0] + 0] BF[positions[q_grams_number-1][0] + 1] ... BF[positions[q_grams_number-1][0] + numberOf64bitBlocks]
		&	BF[positions[q_grams_number-1][1] + 0] BF[positions[q_grams_number-1][1] + 1] ... BF[positions[q_grams_number-1][1] + numberOf64bitBlocks]
		&	...
		&	BF[positions[q_grams_number-1][k] + 0] BF[positions[q_grams_number-1][k] + 1] ... BF[positions[q_grams_number-1][k] + numberOf64bitBlocks]
		*/

		// initialize first
		for (int b = 0; b < number_of_64bit_elements; b++) {
			blocks[b] = bf->bloom_filters[positions[0][0]*number_of_64bit_elements + b];
		}
		// do AND operation
		for ( int i = 0; i < q_grams_number; i++) {
			for (int h = 0; h < k; h++) {
				for ( int b = 0; b < number_of_64bit_elements; b++) {
					blocks[b] &= bf->bloom_filters[positions[i][h]*number_of_64bit_elements + b];
				}
			}
		}
		bool *blocks_to_verify = (bool*)calloc(number_of_blocks, sizeof(bool));
		if(blocks_to_verify==NULL) { printf("Error: findBlocks -> Cannot allocate memory for blocks_to_verify\n"); exit(EXIT_FAILURE); }
	
		unsigned long bl;
		res->length = 0;
		for ( int a = 0; a < number_of_64bit_elements; a++) {
			while(blocks[a]) {
				clz64(&bl, blocks[a]);
				blocks_to_verify[bl + a * 64] = true; // all that blocks have to be validated (searched)
				blocks[a] ^= ((uint64_t)((uint64_t)1 << bl)); // remove not needed 1
				++res->length;
			}
		}
		
		res->blocks_to_verify = (long*)malloc(res->length * sizeof(long));
		if(res->blocks_to_verify==NULL) { printf("Error: findBlocks -> Cannot allocate memory for res->blocks_to_verify\n"); exit(1); }

		for (long i = 0, j = 0; i < number_of_blocks; i++) {
			if (blocks_to_verify[i]) {
				res->blocks_to_verify[j++] = i;
			}
		}
		free(blocks_to_verify);
		free(blocks);
		for(int i = 0; i < q_grams_number; i++) { free(q_grams[i]); }
		free(q_grams);
		for(int i = 0; i < q_grams_number; i++) { free(positions[i]); }
		free(positions);
		return res;
	}

	// Utilities
	TText *readText(const char *filename, const unsigned long block_size, const unsigned int max_m) {
		TText *res = new TText();
		res->max_m = max_m;
		res->block_size = block_size;
		std::fstream txtFile(filename, std::ios::in | std::ios::binary | std::ios::ate);

		if (!txtFile) {
			std::cout << "Error: There was a problem opening file " << filename << " for reading." << std::endl;
			exit(EXIT_FAILURE);
		}
		//cout << "Opened " << p->textFilename << " for reading." << endl;
		res->length = txtFile.tellg();
		res->length_ext = (res->length) + block_size - (res->length % block_size) + max_m;
		if((res->text = (uchar*) malloc(res->length_ext)) == NULL) { printf("Error: readText -> cannot allocate memory\n"); exit(EXIT_FAILURE); }
		txtFile.seekg(0, std::ios::beg);
		txtFile.read((char*)res->text, res->length);
		txtFile.close();
		for (unsigned int i = res->length; i < res->length_ext; i++) {
			res->text[i] = (char)255;
		}
		res->number_of_blocks = res->length_ext / block_size;
		return res;
	}

	/// Returns instance of TPatterns
	TPatterns *readPatterns(const char *pattern_filename, const unsigned int m) {
		TPatterns *result = new TPatterns();
		result->m = m;
		result->length = read_file_content<char>(&(result->patterns), pattern_filename);
		result->number_of_patterns = result->length/m;
		return result;
	}

	/// Reads Bloom filters from file
	TBloomFilters *readBloomFilters(const char *filename) {
		TBloomFilters *res = new TBloomFilters();
		// Read BF from file
		FILE *file = fopen (filename, "rb");
		if (!file) { printf("Error: Cant open file %s\n", filename); exit(EXIT_FAILURE); }

		fread( &(res->filters_size), sizeof(res->filters_size), 1, file);
		fread( &(res->bloom_size), sizeof(res->bloom_size), 1, file);
		fread( &(res->max_m), sizeof(res->max_m), 1, file);
		fread( &(res->number_of_64bit_elememts), sizeof(res->number_of_64bit_elememts), 1, file);
		fread( &(res->number_of_all_64bit_elements), sizeof(res->number_of_all_64bit_elements), 1, file);
		if((res->bloom_filters = (uint64_t*)malloc (res->filters_size)) == NULL) { printf("Error: readBloomFilters -> Cannot allocate memory\n"); exit(EXIT_FAILURE); }
		
		if(fread( res->bloom_filters, 1, res->filters_size, file)!=res->filters_size) {
			printf("Error: wrong file (%s)\n", filename); exit(EXIT_FAILURE);
		}

		fclose (file);
		return res;
	}

	void writeBloomFilters(const char *filename, TBloomFilters *bf) {
		FILE *file = fopen(filename, "wb+");
		if(!file) {
			printf("Error: Cannot open file to write: %s\n", filename); exit(1);
		}
		fwrite((void*)&(bf->filters_size), sizeof(bf->filters_size), 1, file);
		fwrite((void*)&(bf->bloom_size), sizeof(bf->bloom_size), 1, file);
		fwrite((void*)&(bf->max_m), sizeof(bf->max_m), 1, file);
		fwrite((void*)&(bf->number_of_64bit_elememts), sizeof(bf->number_of_64bit_elememts), 1, file);
		fwrite((void*)&(bf->number_of_all_64bit_elements), sizeof(bf->number_of_all_64bit_elements), 1, file);
		fwrite((void*)bf->bloom_filters, 1, bf->filters_size, file);
		fclose(file);
	}


}


#endif
