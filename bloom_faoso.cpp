#include "bloom_faoso.h"
#include "bloom.h"

/* MAIN */

// Available variants
#define STANDARD 1
#define EVERY_G_QGRAM 2
#define ON_MINIMIZERS 3

// Choosen Variant
// #define VARIANT EVERY_G_QGRAM

int main(int argc, char *argv[]) {
	/* This piece of code should be optimized */
	if ( argc < 9 ) return EXIT_FAILURE;
	t_timemes t_faoso = {0}, t_bloom = {0}, t_build = {0};
	t_init(t_faoso); t_init(t_bloom); t_init(t_build);
	
	const unsigned int U = atoi(argv[4]);
	const int k = atoi(argv[5]);	// q
	const int q_size = atoi(argv[6]);
	const int sigma = atoi(argv[7]); // not used in this program
	const unsigned long block_size = atoi(argv[8]);
	const unsigned int c = atoi(argv[9]);

#if VARIANT==STANDARD
	if(argc<10) { printf("Error: std variant needs at least 9 arguments\n"); exit(1); }
	const unsigned int g = 1;
	const unsigned int p = 0;
	const unsigned int w_size = 0;
#elif VARIANT==EVERY_G_QGRAM
	if(argc<11) { printf("Error: eq variant needs at least 10 arguments\n"); exit(1); }
	const unsigned int g = atoi(argv[10]);	// Every g q-gram (set to 0 if not corresponding)
	const unsigned int p = 0;
	const unsigned int w_size = 0;
#elif VARIANT==ON_MINIMIZERS
	if(argc<13) { printf("Error: min variant needs at least 12 arguments\n"); exit(1); }
	const unsigned int g = 0;
	const unsigned int p = atoi(argv[11]);	// Minimizers of p size - optional
	const unsigned int w_size = atoi(argv[12]);	// window size (only for minimizers) - optional
#else
#error You have set to VARIANT one of following values STANDARD, EVERY_G_QGRAM, ON_MINIMIZERS
#endif
	
	char *text_filename = argv[3];
	unsigned int 
		m = pattern_size = atoi(argv[2]);
	char bf_filename[500];
	sprintf(bf_filename, "%s_q%d_bs%d_c%d_m%d_g%d_p%d_w%d.bf", text_filename, q_size, block_size, c, bloom::getSizeOfOverlay(m, q_size), g, p, w_size);
	char *patterns_filename = argv[1];

	// Read data from files
	bloom::TPatterns *patt = bloom::readPatterns(patterns_filename, m);		// read patterns file
	bloom::TText *t = bloom::readText(text_filename, block_size, m);		// read text from disk
	bloom::TBloomFilters *bf = NULL;
	t_start(t_build);
#if VARIANT == STANDARD
	bf = bloom::createInterlacedBloomFiltersEveryQGramParallel(t, q_size, c, 1);	// create bloom filters
#elif VARIANT == EVERY_G_QGRAM
	bf = bloom::createInterlacedBloomFiltersEveryQGramParallel(t, q_size, c, g);	// create bloom filters
#elif VARIANT == ON_MINIMIZERS
	bf = bloom::createInterlacedBloomFiltersOnMinimizersParallel(t, q_size, c, p, w_size);	// create bloom filters
#else
#error You have set to VARIANT one of following values STANDARD, EVERY_G_QGRAM, ON_MINIMIZERS
#endif
	t_stop(t_build);
	if (t->text == NULL) { printf("Error: text==NULL\n"); return EXIT_FAILURE;}
	if (patt->patterns == NULL) { printf("Error: patterns==NULL\n"); return EXIT_FAILURE;}
	if (bf->bloom_filters == NULL) { printf("Error: BF==NULL\n"); return EXIT_FAILURE;}

	long int matches = 0;
	long int number_of_verified_blocks = 0;
	
	for(int tmp_i = 0; tmp_i < patt->length; tmp_i += m) {
		// Find blocks where pattern may exists
		unsigned int m_prim = m > bloom::getSizeOfOverlay(m, q_size) ? bloom::getSizeOfOverlay(m, q_size) : m;
		t_start(t_bloom);
#if VARIANT == STANDARD
		bloom::TBloomResult *res = bloom::findBlocks((uchar*)patt->patterns + tmp_i, m_prim, q_size, c, t->number_of_blocks, bf);
#elif VARIANT == EVERY_G_QGRAM
		bloom::TBloomResult *res = bloom::findBlocksEveryQGram((uchar*)patt->patterns + tmp_i, m_prim, q_size, c, t->number_of_blocks, bf, g);
#elif VARIANT == ON_MINIMIZERS
		bloom::TBloomResult *res = bloom::findBlocksMinimizers((uchar*)patt->patterns + tmp_i, m_prim, q_size, c, t->number_of_blocks, bf, p, w_size);
#else
#error You have set to VARIANT one of following values STANDARD, EVERY_G_QGRAM, ON_MINIMIZERS
#endif
		t_stop(t_bloom);
		
		// for each block in blocks_to_verify execute search
		long symbols_to_search = block_size + m;
		t_start(t_faoso);
		for(int i = 0; i < res->length; i++) {
			long start = 0;
			start = res->blocks_to_verify[i] * block_size;
			faoso::search(U, k, (uchar*)patt->patterns + tmp_i, m, (uchar*)t->text + start, symbols_to_search, start);
		}
		t_stop(t_faoso);
		number_of_verified_blocks += res->length;
		delete res;
	
	}
	
	matches += indexes.size();
#ifdef PRINT_RESULTS
	
	std::set<unsigned long> indexes_ordered( indexes.begin(), indexes.end() );
	for(std::set<unsigned long>::iterator itr = indexes_ordered.begin(); itr != indexes_ordered.end(); itr++) {
		printf("%ld\n", *itr); // vector 
	}
	indexes_ordered.clear();

#endif
	
	indexes.clear();
	
#ifdef PRINT_RESULTS
	//for(unsigned long l = 0; l < p->text_file_size; ++l) if( indexes[l] ) printf("%lu\t%.*s\n", l, pattern_size, p->text+l);
	//bloom_faoso::print_result(t_get_seconds(t_bloom), t_get_seconds(t_faoso), v_count, matches, p->block_size, p->number_of_blocks, number_of_verified_blocks, p->text_file_size, p->bloom_file_size);
#else
	bloom_faoso::print_result(t_get_seconds(t_bloom), t_get_seconds(t_faoso), t_get_seconds(t_build), v_count, matches, block_size, t->number_of_blocks, number_of_verified_blocks, t->length, bf->filters_size, bf->number_of_unique_qgrams);
#endif

	delete patt;
	delete t;
	delete bf;
	return EXIT_SUCCESS;
}

