#include <stdint.h>

#ifndef __BV_H__
#define __BV_H__

typedef enum {
	BV_OK = 0,
	BV_NULL_ARG = -100,
	BV_EMPTY,
	BV_NOT_INIT,
	BV_IDX_OUT_OF_BOUNDS,
	BV_OUT_OF_MEM
} bv_ret_t;


typedef struct _bitvector {
	uint32_t num_entries; /**< number of bits in the vector */
	uint8_t* bits; /**< the array of bytes containing the bits of the vector */
} bitvector;

/**
 * Create and initialize a bitvector that describes num_entries of bits
 *
 * @param num_entries number of bits in the bitvector
 * @param bv the bitvector to initialize
 * @note bv_destroy must be called to prevent memory leaks
 * @note the bitvector will be initialized to zero
 * @return 0 on success, non-zero on failure
 */
bv_ret_t bv_create(uint32_t num_entries, bitvector* bv); 

/**
 * Destroy a bitvector
 *
 * @param bv bitvector to destroy
 * @return 0 on success, non-zero on failure
 */
bv_ret_t bv_destroy(bitvector* bv);

/**
 * Get the value of a given bit in the vector, 0 indexed
 *
 * @param bv the bitvector to query
 * @param bit_index to query, zero indexed, so querying bit_index 0 will return the value of the first bit in the vector
 * @return 0 if unset, 1 if set, negative if error (eg, bit_index larger than bitvector size)
 */
int bv_get_bit(bitvector* bv, uint32_t bit_index);

/**
 * Set the value of a given bit in the vector, 0 indexed
 *
 * @param bv the bitvector to modify
 * @param bit_index to set, zero indexed, so setting bit_index 0 will set the first bit in the vector
 * @return 0 on success, non-zero on error (eg, bit_index larger than bitvector size)
 */
bv_ret_t bv_set_bit(bitvector* bv, uint32_t bit_index);

//self-test
int bv_test();

#endif // __BV_H__
