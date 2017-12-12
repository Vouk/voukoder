#pragma once

char** _packed_to_planar_8i(char *buffer, int size, int stride, int pnum = 4)
{
	char *planes[8];
	for (int i = 0; i < 8; i++)
	{
		planes[i] = (char*)malloc(size / pnum);
	}

	int p = 0;
	while (p < size)
	{
		planes[p % pnum][p / pnum] = buffer[p];
		p++;
	}

	return planes;
}

/*



uint8_t
unsigned char * source = (unsigned char *)_mm_malloc(source_size, 16);
unsigned char * destination = (unsigned char *)_mm_malloc(destination_size, 8);
__m128i mask = _mm_set_epi8(                        //shuffling control mask (constant)
-1, -1, -1, -1, -1, -1, -1, -1, 14, 12, 10, 8, 6, 4, 2, 0
);
for (int i = 0; i < imgSize; i += 16)
{
__m128i reg = *(const __m128i*)source;              //load 16-bit register
__m128i comp = _mm_shuffle_epi8(reg, mask);         //do the bytes compaction
_mm_storel_epi64((__m128i*)destination, comp);      //store lower 64 bits




}

*/