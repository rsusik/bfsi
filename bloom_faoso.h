#include "faoso.h"
#include "bloom.h"
#include <iostream>

namespace bloom_faoso {
	void print_result(double bloom_time, double faoso_time, double build_time,
		long ver_count, long matches, long bsize, long bnumber, long verified_blocks, 
		long text_file_size, long bloom_filter_size, long number_of_unique_qgrams
	) {
		double inmb = text_file_size / 1000.0 / 1000.0;
		double full_time = bloom_time + faoso_time;
		std::cout << bloom_filter_size << "\t" << bsize << "\t" << bnumber << "\t";
		std::cout << verified_blocks << "\t" << text_file_size << "\t";
		std::cout << (double)(text_file_size+bloom_filter_size)/(double)text_file_size << "\t";				// data information
		std::cout << ver_count << "\t" << matches << "\t";													// search information
		std::cout << build_time << "\t" << bloom_time << "\t" << faoso_time << "\t" << full_time << "\t" ;	// times
		std::cout << inmb/build_time << "\t" << inmb/bloom_time << "\t" << inmb/faoso_time << "\t" << inmb/full_time << "\t";// speed
		//std::cout << number_of_unique_qgrams << "\t";
		std::cout << std::endl;
	}
}