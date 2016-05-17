#ifndef HASH_H
#define HASH_H

#ifndef HASH_FUNCTION
	#define HASH_FUNCTION hashFunction_h1h2
	//#define HASH_FUNCTION hashFunction_rand
	//#define HASH_FUNCTION hashFunction_old
#endif

namespace bloom {
	//hash methods

	uint32_t sdbm(unsigned char* str) {
	  uint32_t hash = 0;
	  int c;

	  while (c = *str++)
		hash = c + (hash << 6) + (hash << 16) - hash;

	  return hash;
	}

	uint32_t djb2(unsigned char *str) {
		uint32_t hash = 5381;
		int c;

		while (c = *str++)
			hash = ((hash << 5) + hash) + c;

		return hash;
	}

	uint32_t murmur3_32(unsigned char *key, uint32_t len, uint32_t seed) {
		static const uint32_t c1 = 0xcc9e2d51;
		static const uint32_t c2 = 0x1b873593;
		static const uint32_t r1 = 15;
		static const uint32_t r2 = 13;
		static const uint32_t m = 5;
		static const uint32_t n = 0xe6546b64;
 
		uint32_t hash = seed;
 
		const int nblocks = len / 4;
		const uint32_t *blocks = (const uint32_t *) key;
		int i;
		for (i = 0; i < nblocks; i++) {
			uint32_t k = blocks[i];
			k *= c1;
			k = (k << r1) | (k >> (32 - r1));
			k *= c2;
 
			hash ^= k;
			hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
		}
 
		const uint8_t *tail = (const uint8_t *) (key + nblocks * 4);
		uint32_t k1 = 0;
 
		switch (len & 3) {
		case 3:
			k1 ^= tail[2] << 16;
		case 2:
			k1 ^= tail[1] << 8;
		case 1:
			k1 ^= tail[0];
 
			k1 *= c1;
			k1 = (k1 << r1) | (k1 >> (32 - r1));
			k1 *= c2;
			hash ^= k1;
		}
 
		hash ^= len;
		hash ^= (hash >> 16);
		hash *= 0x85ebca6b;
		hash ^= (hash >> 13);
		hash *= 0xc2b2ae35;
		hash ^= (hash >> 16);
 
		return hash;
	}

	uint32_t rc_crc32(uint32_t crc, const char *buf, size_t len) {
		static uint32_t table[256];
		static int have_table = 0;
		uint32_t rem;
		uint8_t octet;
		int i, j;
		const char *p, *q;
 
		// This check is not thread safe; there is no mutex.
		if (have_table == 0) {
			// Calculate CRC table.
			for (i = 0; i < 256; i++) {
				rem = i;  // remainder from polynomial division
				for (j = 0; j < 8; j++) {
					if (rem & 1) {
						rem >>= 1;
						rem ^= 0xedb88320;
					} else
						rem >>= 1;
				}
				table[i] = rem;
			}
			have_table = 1;
		}
 
		crc = ~crc;
		q = buf + len;
		for (p = buf; p < q; p++) {
			octet = *p;  // Cast to unsigned octet. 
			crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
		}
		return ~crc;
	}

	unsigned int nTweak[] = {
		3747792262, 
		277465737, 
		2649041618,
		2725349669,
		3666310223,
		2103759709,
		2189115937,
		3360402841,
		48120576,
		2829295877,
		1853509082,
		2697014627,
		2210226197,
		1053528617,
		1496000689,
		2669956093,
		1111215420,
		365787805,
		1405486467,
		3318119699,
		1188501971,
		2216149549
	};

	unsigned int get_k(const unsigned int c) { // c -> m/n
		unsigned int k = 0;
		switch(c) {
			case 2:
				k = 1;
				break;
			case 3:
				k = 2;
				break;
			case 4:
			case 5:
				k = 3;
				break;
			case 6:
				k = 4;
				break;
			case 7:
				k = 5;
				break;
			case 8:
			case 9:
				k = 6;
				break;
			case 10:
				k = 7;
				break;
			case 11:
			case 12:
				k = 8;
				break;
			case 13:
				k = 9;
				break;
			case 14:
			case 15:
				k = 10;
				break;
			case 16:
				k = 11;
				break;
			default:
				k = 5;
				break;
		}
		return k;
	}

	uint32_t hashFunction_old(int i, unsigned char* str) {
		switch(i) {
			case 0: return sdbm(str);
			case 1: return djb2(str);
			case 2: return murmur3_32(str, strlen((const char *)str), 0);
			case 3: return rc_crc32(0, (const char *)str, strlen((const char *)str));
			case 4: return XXH32(str, strlen((const char *)str), 0);
			default: return sdbm(str);
		}
	}

	uint32_t hashFunction_rand(int i, unsigned char* str) {
		return murmur3_32(str, strlen((const char *)str), i * 0xFBA4C795 + nTweak[i]);
	}


	uint32_t hashFunction_h1h2(int i, unsigned char* str) {
		uint32_t h1 = murmur3_32(str, strlen((const char *)str), 0);
		uint32_t h2 = murmur3_32(str, strlen((const char *)str), h1);
		return abs(h1 + i * h2);
	}

	inline uint32_t hashFunction(int i, unsigned char* str) {
		return HASH_FUNCTION(i, str);
	}

}

#endif
