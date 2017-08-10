#include "bv.h"
#include <stdlib.h>
#include <string.h>

bv_ret_t bv_create(uint32_t num_entries, bitvector* bv)
{
	if ( bv == NULL ) {
		return BV_NULL_ARG;
	}
	if ( num_entries < 1 ) {
		return BV_EMPTY;
	}
	int bytes_to_alloc;
	if ( (num_entries % 8) == 0 ) {
		bytes_to_alloc = num_entries / 8;
	} else {
		bytes_to_alloc = num_entries / 8 + 1;
	}
	bv->bits  = (uint8_t*)malloc(bytes_to_alloc);
	if ( bv->bits == NULL ) {
		return BV_OUT_OF_MEM;
	} else {
		memset(bv->bits, 0, sizeof(uint8_t));
	}
	bv->num_entries = num_entries;
	return BV_OK;
}

bv_ret_t bv_destroy(bitvector* bv)
{
	free(bv->bits);
	bv->bits = NULL;
	return BV_OK;
}

int bv_get_bit(bitvector* bv, uint32_t bit_index)
{
	if ( bv == NULL ) {
		return BV_NULL_ARG;
	} else if ( bv->bits == NULL ) {
		return BV_NOT_INIT;
	} else if ( bit_index >= bv->num_entries ) {
		return BV_IDX_OUT_OF_BOUNDS;
	}
	bv_ret_t ret; 
	uint8_t byte_to_examine = bv->bits[bit_index / 8];
	bit_index = bit_index % 8;
	uint8_t mask = 1 << (7 - bit_index);
	ret =  (byte_to_examine & mask) >> (7 - bit_index);
	return ret;
}

bv_ret_t bv_set_bit(bitvector* bv, uint32_t bit_index)
{
	if ( bv == NULL ) {
		return BV_NULL_ARG;
	} else if ( bv->bits == NULL ) {
		return BV_NOT_INIT;
	} else if ( bit_index >= bv->num_entries ) {
		return BV_IDX_OUT_OF_BOUNDS;
	}
	
	uint8_t byte_to_examine = bv->bits[bit_index / 8];
	//bit_index = bit_index % 8;
	uint8_t mask = 1 << (7 - (bit_index % 8));
	bv->bits[bit_index / 8] = (byte_to_examine | mask);
	return BV_OK;
}

int bv_test()
{
	int i;
	bitvector bv;
	bv_ret_t ret;
	ret = bv_create(9, &bv);
	if ( ret != BV_OK ) {
		return -1;
	}

	ret = bv_set_bit(&bv, 0);
	ret |= bv_set_bit(&bv, 8);

	if ( ret != BV_OK ) {
		return -2;
	}

	ret = bv_set_bit(&bv, 9);
	if ( ret != BV_IDX_OUT_OF_BOUNDS ) {
		return -3;
	}

	//introspect and make sure the two bytes look correct
	if ( bv.bits[0] != 0x80 ) {
		return -4;
	}

	if ( bv.bits[1] != 0x80 ) {
		return -5;
	}

	//test get
	if ( bv_get_bit(&bv, 0) != 1 ) {
		return -6;
	}

	if ( bv_get_bit(&bv, 8) != 1 ) {
		return -7;
	}

	for ( i = 1; i < 8; i++ ) {
		if ( bv_get_bit(&bv, i) != 0 ) {
			return -8;
		}
	}


}
