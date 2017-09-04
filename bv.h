#include <stdint.h>
#include <stdbool.h>

#ifndef __BV_H__
#define __BV_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	BV_OK = 0,
	BV_NULL_ARG = -100,
	BV_BAD_ARG, 
	BV_EMPTY,
	BV_NOT_INIT,
	BV_IDX_OUT_OF_BOUNDS,
	BV_OUT_OF_MEM
} bv_ret_t;

typedef struct _bitvector {
	uint32_t num_entries; /**< number of bits in the vector */
	uint32_t sz_of_bits; /**< size of bits array (the internal array) in bytes */
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
bv_ret_t bv_create(bitvector* bv, uint32_t num_entries); 

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

/**
 * Clear a given bit in the vector, 0 indexed
 *
 * @param bv the bitvector to modify
 * @param bit_index to clear, zero indexed, so clearing bit_index 0 will clear the first bit in the vector
 * @return 0 on success, non-zero on error (eg, bit_index larger than bitvector size)
 */
bv_ret_t bv_clear_bit(bitvector* bv, uint32_t bit_index);

/**
 * Clear all set bits in a bitvector
 *
 * @return the return status
 */
bv_ret_t bv_clear(bitvector* bv);

/**
 * Serialize a given bitvector into a newly allocated uint8_t array, caller takes 
 * control of serialized_bv
 *
 * @param bv the bitvector to serialize
 * @param serialized_bv pointer to the uint8_t* that will be allocated and populated with the serialized version of bv
 * @param serialized_bv_sz the size of serialized_bv
 * @return return status
 * @note caller takes control of memory allocated for serialized_bv
 */
bv_ret_t bv_serialize(bitvector* bv, uint8_t** serialized_bv, uint32_t* serialized_bv_sz);

/**
 * Deserialize a given uint8_t array into a newly allocated bitvector
 *
 * @param bv the bitvector that will be allocated and populated with the data in serialized_bv
 * @param serialized_bv the serialized bv that is to be deserialized
 * @return return status
 * @note the caller is responsible for the newly allocated bv. It should be destroyed via bv_destroy and then free'd/deleted. 
 */
bv_ret_t bv_deserialize(bitvector** bv, uint8_t* serialized_bv);

//self-test
int bv_test(void);

#ifdef __cplusplus
}
#endif

#endif // __BV_H__
