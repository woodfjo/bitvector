#include "bv.h"
#include <stdlib.h>
#include <string.h>

bv_ret_t bv_create(bitvector* bv, uint32_t num_entries)
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
		bv->sz_of_bits = bytes_to_alloc;
		memset(bv->bits, 0, sizeof(uint8_t));
	}
	bv->num_entries = num_entries;
	return BV_OK;
}

bv_ret_t bv_destroy(bitvector* bv)
{
	free(bv->bits);
	bv->num_entries = 0;
	bv->sz_of_bits = 0;
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

static bv_ret_t bv_set_bit_to_val(bitvector* bv, uint32_t bit_index, uint32_t val)
{
	if ( bv == NULL ) {
		return BV_NULL_ARG;
	} else if ( bv->bits == NULL ) {
		return BV_NOT_INIT;
	} else if ( bit_index >= bv->num_entries ) {
		return BV_IDX_OUT_OF_BOUNDS;
	} else if ( val != 0 && val != 1 ) {
		return BV_BAD_ARG;
	}
	
	uint8_t byte_to_examine = bv->bits[bit_index / 8];
	//bit_index = bit_index % 8;
	uint8_t mask = 1 << (7 - (bit_index % 8));

	if ( val == false ) {
		bv->bits[bit_index / 8] = (byte_to_examine & ~mask);
	} else {
		bv->bits[bit_index / 8] = (byte_to_examine | mask);
	}
	return BV_OK;
}

bv_ret_t bv_clear_bit(bitvector* bv, uint32_t bit_index)
{
	return bv_set_bit_to_val(bv, bit_index, 0);
}

bv_ret_t bv_set_bit(bitvector* bv, uint32_t bit_index)
{
	return bv_set_bit_to_val(bv, bit_index, 1);
}

bv_ret_t bv_clear(bitvector* bv)
{
	if ( bv->bits != NULL ) {
		memset(bv->bits, 0, bv->sz_of_bits);
	} else {
		return BV_NULL_ARG;
	}

	return BV_OK;
}

bv_ret_t bv_serialize(bitvector* bv, uint8_t** serialized_bv, uint32_t* serialized_bv_sz)
{
	if ( !bv || !serialized_bv || !serialized_bv_sz ) {
		return BV_NULL_ARG;
	}
	
	uint32_t sz_to_alloc = sizeof(bitvector) + bv->sz_of_bits;
	*serialized_bv = (uint8_t*)malloc(sz_to_alloc);
	if ( !(*serialized_bv) ) {
		return BV_OUT_OF_MEM;
	}

	//assume the serializer and deserializer are the same endianness for now
	*((uint32_t*)(*serialized_bv)) = bv->num_entries;
	*((uint32_t*)(*serialized_bv + 4)) = bv->sz_of_bits;
	memcpy((*serialized_bv + 8), bv->bits, bv->sz_of_bits);

	*serialized_bv_sz = 8 + bv->sz_of_bits;
	return BV_OK;
}

bv_ret_t bv_deserialize(bitvector** bv, uint8_t* serialized_bv)
{
	bv_ret_t ret = BV_OK; 
	if ( !serialized_bv || !bv ) {
		return BV_NULL_ARG;
	}

	*bv = (bitvector*)malloc(sizeof(bitvector));
	if ( *bv == NULL ) {
		return BV_OUT_OF_MEM;
	}

	//assume the serializer and deserializer are the same endianness for now	
	ret = bv_create(*bv, *((uint32_t*)serialized_bv));
	if ( ret != BV_OK ) {
		return ret;
	}
		
	(*bv)->sz_of_bits = *((uint32_t*)(serialized_bv + 4));
	(*bv)->bits = (uint8_t*)malloc((*bv)->sz_of_bits);
	memcpy((*bv)->bits, serialized_bv + 8, (*bv)->sz_of_bits);

	return BV_OK;
}

int bv_test(void)
{
	int i;
	bitvector bv;
	bv_ret_t ret;
	ret = bv_create(&bv, 9);
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

	bv_destroy(&bv);

	//test serialize/deserialize
	bv_create(&bv, 128);
	for ( i = 0; i < 128; i++ ) {
		if ( i % 2 == 0 ) {
			bv_set_bit(&bv, i);
		}
	}

	uint8_t* serialized_bv = NULL;
	uint32_t sz; 
	ret = bv_serialize(&bv, &serialized_bv, &sz);
	if ( ret != BV_OK ) {
		return -9;
	}

	bitvector* deserialized_bv = { 0 };
	
	ret = bv_deserialize(&deserialized_bv, serialized_bv);
	if ( ret != BV_OK ) {
		return -9;
	}

	if ( bv.num_entries != deserialized_bv->num_entries ) {
		return -10;
	}
	
	if ( bv.sz_of_bits != deserialized_bv->sz_of_bits ) {
		return -11;
	}
	
	if ( memcmp(bv.bits, deserialized_bv->bits, bv.sz_of_bits) ) {
		return -12;
	}
	
	return 0;
}
