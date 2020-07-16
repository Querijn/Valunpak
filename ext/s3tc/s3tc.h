#pragma once

unsigned long pack_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void decompress_block_dxt1(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image);
void block_decompress_image_dxt1(unsigned long width, unsigned long height, const unsigned char *blockStorage, unsigned long *image);
void decompress_block_dxt5(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image);
void block_decompress_image_dxt5(unsigned long width, unsigned long height, const unsigned char *blockStorage, unsigned long *image);
