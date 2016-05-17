#include "common.h"

// This is FAOSO algorithm which is based on orginal implementation by Kimmo Fredriksson.

namespace faoso {

	void print_resultn(double search_time, long ver_count, long matches, long n) {
		double inmb = n / 1000.0 / 1000.0;
		printf("%ld\t%ld\t%ld\t%f\t%f\n", n, ver_count, matches, search_time,  inmb / (double)search_time);
	}

	static int* build_verify_table( int m, int k, int r, int l ) {
		int	mk, h, mpk = m / k, c, f, * t;
		t = (int*)malloc( MAX_WORD_SIZE * sizeof( int ));

		for( f = 0; f < r; f++ ) {
			c = ( 1 - f - mpk ) * k;
			mk = ( mpk * l - 1 + f * l );

			for( h = 0; h < k; h++, c-- ) {
				t[ mk ] = c; 
				mk += mpk * l + r - 1;
			}
		}
		return t;
	}

	void faoso_verify_opt( uchar * t, uchar * p, int m, word d, word mm, int pos, int * vt, int myn, int offset ) {
		++v_count;
		int	s, c;
		d = ( d & mm ) ^ mm;
		while( d ) {
			s = ilog2( d );
			c = vt[ s ] + pos;
			// Verification
			if(memcmp(t+c, p, m)==0) {
				indexes.insert(offset + c);
			}
			d &= ~(( word )1 << s );
		}
	}

	/// Returns maximum available m.
	/// In case of error prints message and terminates program
	int inline get_max_m(int m, int u, int k) {
		//	int max_m = k * (MAX_WORD_SIZE - u);	// konf.
		int max_m = (MAX_WORD_SIZE) - k * (u - 1);
		if( m > max_m ) {
			m = max_m;
		}
		// Assume that we search at least 2 chars, so m should be >= 2*k
		if( m < 2*k ) { printf("Error: m < 2*k (%d < %d)", m, 2*k); exit(EXIT_FAILURE); }
		return m;
		/*
		int max_m = ( MAX_WORD_SIZE / k + 1 - u ) * k - 2;
		if( m + u > max_m ) {
			m = max_m;
		}
		if( m <= k) { printf("Error: m <= k (%d > %d)", m, k); exit(EXIT_FAILURE); }
		return m;
		*/
	}

	/*
	Expands to:
		d = ( d << 1 ) | b[ get_Q_gram( (tt + 0  * K * Q), sigma)];
		d = ( d << 1 ) | b[ get_Q_gram( (tt + 1  * K * Q), sigma)];
		d = ( d << 1 ) | b[ get_Q_gram( (tt + 2  * K * Q), sigma)];
		d = ( d << 1 ) | b[ get_Q_gram( (tt + 3  * K * Q), sigma)];
		... U times
	*/
	template <unsigned int U_i_PARAM, unsigned int U_PARAM, unsigned int K_PARAM>
	struct faoso_ { 
		static void get_d(word &d, uchar *tt, word *b) {
			d = ( d << 1 ) | b[ tt[ (U_PARAM - U_i_PARAM) * K_PARAM]];
			faoso_<U_i_PARAM-1, U_PARAM, K_PARAM>::get_d(d, tt, b);
		}
	};

	template <unsigned int U_PARAM, unsigned int K_PARAM>
	struct faoso_<0, U_PARAM, K_PARAM> {
		static void get_d(word &d, uchar *tt, word *b) {
			return;
		}
	};

	template<unsigned int U_PARAM, unsigned int K_PARAM> void 
	faoso( uchar * p, int m, uchar * text, unsigned long n, unsigned long offset ) {
		word		b[ 256 ];
		word		d;
		word		mm;
		int		i, j, h, r, *vt, foo = U_PARAM * K_PARAM, m_orginal = m;
		uchar		* tt;
		
		// h = k * ( m / k ) + k * u - 2   =>   h = k * ( u - 1 ) + m
		m = get_max_m(m, U_PARAM, K_PARAM);	// cutting m

		for( i = 0; i < 256; i++ ) b[ i ] = ~( word )0; 

		for( j = h = 0, mm = ( word )0; j < K_PARAM; j++ ) {
			for( i = 0; i < m/K_PARAM; i++ ) 
				b[ p[ i * K_PARAM + j ]] &= ~(( word )1 << ( h++ ));
			for( r = 0; r < U_PARAM; r++, h++ )
				mm = mm | (( word )1 << ( h - 1 )); 
			--h;
		}

		if( h > MAX_WORD_SIZE ) { printf("Error: h=%d >= %d (WSIZE), {\"Cannot shift more than WSIZE-1\", m = %d, k = %d, U = %d }\n", h, ( int )MAX_WORD_SIZE, m, K_PARAM, U_PARAM ); exit(EXIT_FAILURE); }
		for( i = 0; i < 256; i++ ) b[ i ] &= ~( mm & ( mm << 1 ));
		vt = build_verify_table( m, K_PARAM, U_PARAM, 1 );
		d = ~mm; i = n; tt = text;

		do {
			faoso_<U_PARAM, U_PARAM, K_PARAM>::get_d(d, tt, b);
			if(( d & mm ) != mm ) faoso_verify_opt( text, p, m_orginal, d, mm, tt - text - K_PARAM + foo, vt, n, offset );
			d &= ~mm;
			tt += foo;

		} while(( i -= foo ) > 0 );
		free(vt);
	}

	void search(byte U, byte k, uchar * p, int m, uchar * text, unsigned long n, unsigned long offset) {
		if (0) {
		} else if( U==1 && k==1 ) {
			faoso<1, 1>(p, m, text, n, offset);
		} else if( U==1 && k==2 ) {
			faoso<1, 2>(p, m, text, n, offset);
		} else if( U==1 && k==4 ) {
			faoso<1, 4>(p, m, text, n, offset);
		} else if( U==1 && k==6 ) {
			faoso<1, 6>(p, m, text, n, offset);
		} else if( U==1 && k==8 ) {
			faoso<1, 8>(p, m, text, n, offset);
		} else if( U==1 && k==12 ) {
			faoso<1, 12>(p, m, text, n, offset);
		} else if( U==1 && k==16 ) {
			faoso<1, 16>(p, m, text, n, offset);
		} else if( U==2 && k==1 ) {
			faoso<2, 1>(p, m, text, n, offset);
		} else if( U==2 && k==2 ) {
			faoso<2, 2>(p, m, text, n, offset);
		} else if( U==2 && k==4 ) {
			faoso<2, 4>(p, m, text, n, offset);
		} else if( U==2 && k==6 ) {
			faoso<2, 6>(p, m, text, n, offset);
		} else if( U==2 && k==8 ) {
			faoso<2, 8>(p, m, text, n, offset);
		} else if( U==2 && k==12 ) {
			faoso<2, 12>(p, m, text, n, offset);
		} else if( U==2 && k==16 ) {
			faoso<2, 16>(p, m, text, n, offset);
		} else if( U==4 && k==1 ) {
			faoso<4, 1>(p, m, text, n, offset);
		} else if( U==4 && k==2 ) {
			faoso<4, 2>(p, m, text, n, offset);
		} else if( U==4 && k==4 ) {
			faoso<4, 4>(p, m, text, n, offset);
		} else if( U==4 && k==6 ) {
			faoso<4, 6>(p, m, text, n, offset);
		} else if( U==4 && k==8 ) {
			faoso<4, 8>(p, m, text, n, offset);
		} else if( U==4 && k==12 ) {
			faoso<4, 12>(p, m, text, n, offset);
		} else if( U==4 && k==16 ) {
			faoso<4, 16>(p, m, text, n, offset);
		} else if( U==8 && k==1 ) {
			faoso<8, 1>(p, m, text, n, offset);
		} else if( U==8 && k==2 ) {
			faoso<8, 2>(p, m, text, n, offset);
		} else if( U==8 && k==4 ) {
			faoso<8, 4>(p, m, text, n, offset);
		} else if( U==8 && k==6 ) {
			faoso<8, 6>(p, m, text, n, offset);
		} else if( U==8 && k==8 ) {
			faoso<8, 8>(p, m, text, n, offset);
		} else if( U==8 && k==12 ) {
			faoso<8, 12>(p, m, text, n, offset);
		} else if( U==8 && k==16 ) {
			faoso<8, 16>(p, m, text, n, offset);
		} else if( U==16 && k==1 ) {
			faoso<16, 1>(p, m, text, n, offset);
		} else if( U==16 && k==2 ) {
			faoso<16, 2>(p, m, text, n, offset);
		} else if( U==16 && k==4 ) {
			faoso<16, 4>(p, m, text, n, offset);
		} else if( U==16 && k==6 ) {
			faoso<16, 6>(p, m, text, n, offset);
		} else if( U==16 && k==8 ) {
			faoso<16, 8>(p, m, text, n, offset);
		} else if( U==16 && k==12 ) {
			faoso<16, 12>(p, m, text, n, offset);
		} else if( U==16 && k==16 ) {
			faoso<16, 16>(p, m, text, n, offset);
		} else {
			printf("Error: faoso::search do not support given parameters (U=%d, k=%d)", U, k); exit(1);
		}
	}
}