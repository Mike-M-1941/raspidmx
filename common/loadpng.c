//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2013 Andrew Duncan
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//-------------------------------------------------------------------------

#include <assert.h>
#include <png.h>
#include <stdlib.h>

#include "bcm_host.h"

#include "image.h"
#include "loadpng.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

//-------------------------------------------------------------------------

#ifndef ALIGN_TO_16
#define ALIGN_TO_16(x)  ((x + 15) & ~15)
#endif

#ifndef ALIGN_TO_32
#define ALIGN_TO_32(x)  ((x + 31) & ~31)
#endif

//-------------------------------------------------------------------------

bool
loadPng(
    IMAGE_T* image,
    const char *file)
{
    assert(image != NULL);

    FILE* fpin = fopen(file, "rb");

    if (fpin == NULL) {
        fprintf(stderr, "loadpng: can't open file for reading\n");
        return false;
    }

    //---------------------------------------------------------------------

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
	fclose(fpin);
	printf("Failed opening '%s' for reading! png_create_read_struct \n", file);
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
	fclose(fpin);
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
	printf("Failed opening '%s' for reading! png_create_info_struct \n", file);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
	fclose(fpin);
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
	printf("Failed opening '%s' for reading! setjmp \n", file);
        return false;
    }

    //---------------------------------------------------------------------

    png_init_io(png_ptr, fpin);

    png_set_sig_bytes(png_ptr, 0);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_SHIFT, NULL);

    png_uint_32		width, height;
    int			bit_depth, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

    int png_Bpp = png_get_rowbytes(png_ptr, info_ptr) / width;
    //int dstride = width * 4;
    //int memsize = height * dstride;
    
    initImage(image, VC_IMAGE_RGBA32, width, height, false);

    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    int i,j;
    RGBA8_T rgb;

    if (png_Bpp == 3) {
	for (i=0; i < height; i++) {
	    for (j=0; j < width; j++) {
		png_bytep byt_s = row_pointers[i] + (j * 3);
		rgb.red   = byt_s[0];
		rgb.green = byt_s[1];
		rgb.blue  = byt_s[2];
		rgb.alpha = 255;
		setPixelRGB( image, j, i, 1, &rgb );
	    }
	}
    } else
    if (png_Bpp == 4) {
	for (i=0; i < height; i++) {
	    for (j=0; j < width; j++) {
		png_bytep byt_s = row_pointers[i] + (j * 4);
		rgb.red   = byt_s[0];
		rgb.green = byt_s[1];
		rgb.blue  = byt_s[2];
		rgb.alpha = byt_s[3];
		setPixelRGB( image, j, i, 1, &rgb );
	    }
	}
    }

    //---------------------------------------------------------------------

    fclose(fpin);

    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    return true;
}

