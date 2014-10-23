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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"

//-------------------------------------------------------------------------

#ifndef ALIGN_TO_16
#define ALIGN_TO_16(x)  ((x + 15) & ~15)
#endif

//-------------------------------------------------------------------------

void setPixel4BPP(IMAGE_T *image, int32_t x, int32_t y, int32_t num, int8_t index);
void setPixel8BPP(IMAGE_T *image, int32_t x, int32_t y, int32_t num, int8_t index);
void setPixelRGB565(IMAGE_T *image, int32_t x, int32_t y, int32_t num, const RGBA8_T *rgba);
void setPixelDitheredRGB565(IMAGE_T *image, int32_t x, int32_t y, int32_t num, const RGBA8_T *rgba);
void setPixelRGB888(IMAGE_T *image, int32_t x, int32_t y, int32_t num, const RGBA8_T *rgba);
void setPixelRGBA16(IMAGE_T *image, int32_t x, int32_t y, int32_t num, const RGBA8_T *rgba);
void setPixelDitheredRGBA16(IMAGE_T *image, int32_t x, int32_t y, int32_t num, const RGBA8_T *rgba);
void setPixelRGBA32(IMAGE_T *image, int32_t x, int32_t y, int32_t num, const RGBA8_T *rgba);

void setPixelAlphaRGBA16(IMAGE_T *image, int32_t x, int32_t y, int32_t num, const RGBA8_T *rgba);
void setPixelAlphaRGB888(IMAGE_T *image, int32_t x, int32_t y, int32_t num, const RGBA8_T *rgba);
void setPixelAlphaRGBA32(IMAGE_T *image, int32_t x, int32_t y, int32_t num, const RGBA8_T *rgba);

void getPixel4BPP(IMAGE_T *image, int32_t x, int32_t y, int8_t *index);
void getPixel8BPP(IMAGE_T *image, int32_t x, int32_t y, int8_t *index);
void getPixelRGB565(IMAGE_T *image, int32_t x, int32_t y, RGBA8_T *rgba);
void getPixelRGB888(IMAGE_T *image, int32_t x, int32_t y, RGBA8_T *rgba);
void getPixelRGBA16(IMAGE_T *image, int32_t x, int32_t y, RGBA8_T *rgba);
void getPixelRGBA32(IMAGE_T *image, int32_t x, int32_t y, RGBA8_T *rgba);

//-------------------------------------------------------------------------

bool initImage(
    IMAGE_T *image,
    VC_IMAGE_TYPE_T type,
    int32_t width,
    int32_t height,
    bool dither)
{
    switch (type)
    {
    case VC_IMAGE_4BPP:

        image->bitsPerPixel = 4;
        image->setPixelAlpha = NULL;
        image->setPixelDirect = NULL;
        image->getPixelDirect = NULL;
        image->setPixelIndexed = setPixel4BPP;
        image->getPixelIndexed = getPixel4BPP;

        break;

    case VC_IMAGE_8BPP:

        image->bitsPerPixel = 8;
        image->setPixelAlpha = NULL;
        image->setPixelDirect = NULL;
        image->getPixelDirect = NULL;
        image->setPixelIndexed = setPixel8BPP;
        image->getPixelIndexed = getPixel8BPP;

        break;

    case VC_IMAGE_RGB565:

        image->bitsPerPixel = 16;

        if (dither)
        {
            image->setPixelAlpha = setPixelDitheredRGB565;
            image->setPixelDirect = setPixelDitheredRGB565;
        }
        else
        {
            image->setPixelAlpha = setPixelDitheredRGB565;
            image->setPixelDirect = setPixelRGB565;
        }
        image->getPixelDirect = getPixelRGB565;
        image->setPixelIndexed = NULL;
        image->getPixelIndexed = NULL;

        break;

    case VC_IMAGE_RGB888:

        image->bitsPerPixel = 24;
        image->setPixelAlpha = setPixelAlphaRGB888;
        image->setPixelDirect = setPixelRGB888;
        image->getPixelDirect = getPixelRGB888;
        image->setPixelIndexed = NULL;
        image->getPixelIndexed = NULL;

        break;

    case VC_IMAGE_RGBA16:

        image->bitsPerPixel = 16;
        if (dither)
        {
            image->setPixelAlpha = setPixelDitheredRGBA16;
            image->setPixelDirect = setPixelDitheredRGBA16;
        }
        else
        {
            image->setPixelAlpha = setPixelAlphaRGBA16;
            image->setPixelDirect = setPixelRGBA16;
        }
        image->getPixelDirect = getPixelRGBA16;
        image->setPixelIndexed = NULL;
        image->getPixelIndexed = NULL;

        break;

    case VC_IMAGE_RGBA32:

        image->bitsPerPixel = 32;
        image->setPixelAlpha = setPixelAlphaRGBA32;
        image->setPixelDirect = setPixelRGBA32;
        image->getPixelDirect = getPixelRGBA32;
        image->setPixelIndexed = NULL;
        image->getPixelIndexed = NULL;

        break;

    default:

        fprintf(stderr, "image: unknown type (%d)\n", type);
        return false;

        break;
    }

    image->type = type;
    image->width = width;
    image->height = height;
    image->pitch = (ALIGN_TO_16(width) * image->bitsPerPixel) / 8;
    image->alignedHeight = ALIGN_TO_16(height);
    image->size = image->pitch * image->alignedHeight;

    image->buffer = calloc(1, image->size);

    if (image->buffer == NULL)
    {
        fprintf(stderr, "image: memory exhausted\n");
        exit(EXIT_FAILURE);
    }

    return true;
}

//-------------------------------------------------------------------------

void
clearImageIndexed(
    IMAGE_T *image,
    int8_t index)
{
    if (image->setPixelIndexed != NULL)
    {
	image->setPixelIndexed(image, 0, 0, image->height * image->width, index);
    }
}

//-------------------------------------------------------------------------

void
clearImageRGB(
    IMAGE_T *image,
    const RGBA8_T *rgb)
{
    if (image->setPixelDirect != NULL)
    {
	image->setPixelDirect(image, 0, 0, image->height * image->width, rgb);
    }
}

//-------------------------------------------------------------------------

bool
setPixelIndexed(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    int8_t index)
{
    bool result = false;
    int32_t origin = (y * image->width) + x;
    int32_t max = image->width * image->height;
    if (origin + num > max) num = max - origin;

    if ((image->setPixelIndexed != NULL) &&
        (x >= 0) && (x < image->width) &&
        (y >= 0) && (y < image->height))
    {
        result = true;
        image->setPixelIndexed(image, x, y, num, index);
    }

    return result;
}

//-------------------------------------------------------------------------

bool
setPixelRGB(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    const RGBA8_T *rgb)
{
    bool result = false;
    int32_t origin = (y * image->width) + x;
    int32_t max = image->width * image->height;
    if (origin + num > max) num = max - origin;

    if ((image->setPixelDirect != NULL) &&
        (x >= 0) && (x < image->width) &&
        (y >= 0) && (y < image->height))
    {
        result = true;
        image->setPixelDirect(image, x, y, num, rgb);
    }

    return result;
}

//-------------------------------------------------------------------------

bool
setPixelRGBA(		// mix src and dest using alpha.
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    const RGBA8_T *rgb)
{
    bool result = false;
    int32_t origin = (y * image->width) + x;
    int32_t max = image->width * image->height;
    if (origin + num > max) num = max - origin;

    if ((image->setPixelDirect != NULL) &&
        (x >= 0) && (x < image->width) &&
        (y >= 0) && (y < image->height))
    {
        result = true;
        image->setPixelAlpha(image, x, y, num, rgb);
    }

    return result;
}

//-------------------------------------------------------------------------

bool
getPixelIndexed(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int8_t *index)
{
    bool result = false;

    if ((image->getPixelIndexed != NULL) && 
        (x >= 0) && (x < image->width) &&
        (y >= 0) && (y < image->height))
    {
        result = true;
        image->getPixelIndexed(image, x, y, index);
    }

    return result;
}

//-------------------------------------------------------------------------

bool
getPixelRGB(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    RGBA8_T *rgb)
{
    bool result = false;

    if ((image->getPixelDirect != NULL) &&
        (x >= 0) && (x < image->width) &&
        (y >= 0) && (y < image->height))
    {
        result = true;
        image->getPixelDirect(image, x, y, rgb);
    }

    return result;
}

//-------------------------------------------------------------------------

void
destroyImage(
    IMAGE_T *image)
{
    if (image->buffer)
    {
        free(image->buffer);
    }

    image->type = VC_IMAGE_MIN;
    image->width = 0;
    image->height = 0;
    image->pitch = 0;
    image->alignedHeight = 0;
    image->bitsPerPixel = 0;
    image->size = 0;
    image->buffer = NULL;
    image->setPixelDirect = NULL;
    image->getPixelDirect = NULL;
    image->setPixelIndexed = NULL;
    image->getPixelIndexed = NULL;
}

//-------------------------------------------------------------------------

void memfill(void *dest, size_t destsize, size_t elemsize) {
        char    *nextdest = (char *) dest + elemsize;
        size_t  movesize, donesize = elemsize;

        destsize -= elemsize;
        while (destsize) {
                movesize = (donesize < destsize) ? donesize : destsize;
                memcpy(nextdest, dest, movesize);
                nextdest += movesize;
                destsize -= movesize;
                donesize += movesize;
        }
}

//-----------------------------------------------------------------------

void
setPixel4BPP(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    int8_t index)
{
    index &= 0x0F;

    uint8_t *value = (uint8_t*)(image->buffer + (x/2) + (y * image->pitch));

    if (num == 0 || num == 1) {
	if (x % 2) {
	    *value = (*value & 0xF0) | (index);		// odd
	} else {
	    *value = (*value & 0x0F) | (index << 4);	// even
	}
    } else if (num == 2) {
	if (x % 2) {  // start odd
	    *value = (*value & 0xF0) | (index);
	    value++;
	    *value = (*value & 0x0F) | (index << 4);
	} else {				// start even 1 byte
	    *value = (index) | (index << 4);
	}
    } else if (num == 3) {
	if (x % 2) {	// start odd
	    *value = (*value & 0xF0) | (index);	// odd
	    value++;
	    *value = (index) | (index << 4);	// even odd
	} else {
	    *value = (index) | (index << 4);	// even odd
	    value++;
	    *value = (*value & 0x0F) | (index << 4); // even
	}
    } else if (num == 4) {
	if (x % 2) {	// start odd
	    *value = (*value & 0xF0) | (index);	// odd
	    value++;
	    *value = (index) | (index << 4);	// even odd
	    value++;
	    *value = (*value & 0x0F) | (index << 4); // even
	} else {
	    *value = (index) | (index << 4);	// even odd
	    value++;
	    *value = (index) | (index << 4);	// even odd
	}
    } else if (num >= 5) {
	int byt = num / 2;
	int odd = num % 2;
	if (x % 2) {	// start odd
	    *value = (*value & 0xF0) | (index);	// odd
	    value++;
	    *value = (index) | (index << 4);	// even odd
	    memfill( value, sizeof(uint8_t) * byt, sizeof(uint8_t) );	// even odd even odd
	    if (odd == 0) {
		value += byt;
		*value = (*value & 0x0F) | (index << 4); // even
	    }
	} else {
	    *value = (index) | (index << 4);	// even odd
	    memfill( value, sizeof(uint8_t) * byt, sizeof(uint8_t) );	// even odd even odd
	    if (odd == 1) {
		value += byt;
		*value = (*value & 0xF0) | (index); // odd
	    }
	}
    }
}

//-----------------------------------------------------------------------

void
setPixel8BPP(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    int8_t index)
{
    uint8_t *value;  value = (uint8_t*) (image->buffer + x + (y * image->pitch));
    *value = index;
    if (num >= 2)
	memfill( value, sizeof(uint8_t) * num, sizeof(uint8_t) );
}

//-----------------------------------------------------------------------

void
setPixelRGB565(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    const RGBA8_T *rgba)
{
    uint8_t r5 = rgba->red >> 3;
    uint8_t g6 = rgba->green >> 2;
    uint8_t b5 = rgba->blue >> 3;

    uint16_t pixel = (r5 << 11) | (g6 << 5) | b5;
    uint16_t *value;  value = (uint16_t*) (image->buffer + (x * 2) + (y *image->pitch));

    *value = pixel;
    if (num >= 2)
	memfill( value, sizeof(uint16_t) * num, sizeof(uint16_t) );
}

//-----------------------------------------------------------------------

void
setPixelDitheredRGB565(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    const RGBA8_T *rgba)
{
    static int16_t dither8[64] =
    {
        1, 6, 2, 7, 1, 6, 2, 7,
        4, 2, 5, 4, 4, 3, 6, 4,
        1, 7, 1, 6, 2, 7, 1, 7,
        5, 3, 5, 3, 5, 4, 5, 3,
        1, 6, 2, 7, 1, 6, 2, 7,
        4, 3, 6, 4, 4, 2, 6, 4,
        2, 7, 1, 7, 2, 7, 1, 6,
        5, 3, 5, 3, 5, 3, 5, 3,
    };

    static int16_t dither4[64] =
    {
        1, 3, 1, 3, 1, 3, 1, 3,
        2, 1, 3, 2, 2, 1, 3, 2,
        1, 3, 1, 3, 1, 3, 1, 3,
        2, 2, 2, 1, 3, 2, 2, 2,
        1, 3, 1, 3, 1, 3, 1, 3,
        2, 1, 3, 2, 2, 1, 3, 2,
        1, 3, 1, 3, 1, 3, 1, 3,
        3, 2, 2, 2, 2, 2, 2, 2,
    };

    int32_t i;
    for (i=0; i < num; i++) {
	int32_t index = (x & 7) | ((y & 7) << 3);

	int16_t r = rgba->red + dither8[index];
	if (r > 255) r = 255;

	int16_t g = rgba->green + dither4[index];
	if (g > 255) g = 255;

	int16_t b = rgba->blue + dither8[index];
	if (b > 255) b = 255;

	RGBA8_T dithered = { r, g, b, rgba->alpha };

	setPixelRGB565(image, x, y, 1, &dithered);

	x++;
	if (x > image->width) {
	    x = 0; y++;
	    if (y > image->height) return;
	}
    }
}

//-------------------------------------------------------------------------

void
setPixelRGB888(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    const RGBA8_T *rgba)
{
    uint8_t *line;  line = (uint8_t *) (image->buffer) + (y * image->pitch) + (3 * x);
    line[0] = rgba->red;
    line[1] = rgba->green;
    line[2] = rgba->blue;
    if (num >= 2)
	memfill( line, sizeof(uint8_t) * 3 * num, sizeof(uint8_t) * 3 );
}

//-------------------------------------------------------------------------

void
setPixelAlphaRGB888(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    const RGBA8_T *rgba)
{
    uint8_t *line;  line = (uint8_t *) (image->buffer) + (y * image->pitch) + (3 * x);
    int ad = 255 - rgba->alpha;
    int as = rgba->alpha;
    int tmp;

    tmp = ((line[0] * ad)/255) + ((rgba->red * as)/255);
    line[0] = tmp < 255 ? (tmp >= 0 ? tmp : 0) : 255;
    tmp = ((line[1] * ad)/255) + ((rgba->green * as)/255);
    line[1] = tmp < 255 ? (tmp >= 0 ? tmp : 0) : 255;
    tmp = ((line[2] * ad)/255) + ((rgba->blue * as)/255);
    line[2] = tmp < 255 ? (tmp >= 0 ? tmp : 0) : 255;

    if (num >= 2) {
	if (x < image->width) {
		setPixelAlphaRGB888( image, x + 1, y, num - 1, rgba );
	} else {
		setPixelAlphaRGB888( image, 0, y + 1, num - 1, rgba );
	}
    }
}

//-----------------------------------------------------------------------

void
setPixelRGBA16(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    const RGBA8_T *rgba)
{
    uint8_t r4 = rgba->red  >> 4;
    uint8_t g4 = rgba->green >> 4;
    uint8_t b4 = rgba->blue >> 4;
    uint8_t a4 = rgba->alpha >> 4;

    uint16_t pixel = (r4 << 12) | (g4 << 8) | (b4 << 4) | a4;
    uint16_t *value;  value = (uint16_t*) (image->buffer + (x * 2) + (y * image->pitch));

    *value = pixel;
    if (num >= 2)
	memfill( value, sizeof(uint16_t) * num, sizeof(uint16_t) );
}

//-----------------------------------------------------------------------

void
setPixelAlphaRGBA16(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    const RGBA8_T *rgba)
{
    uint16_t *value;  value = (uint16_t*) (image->buffer + (x * 2) + (y *image->pitch));
    uint8_t src_r4 = (*value & 0xF000) >> 8;
    uint8_t src_g4 = (*value & 0x0F00) >> 4;
    uint8_t src_b4 = (*value & 0x00F0);
    uint8_t src_a4 = (*value & 0x000F);

    int ad = 255 - rgba->alpha;
    int as = rgba->alpha;
    int tmp;

    tmp = ((src_r4 * ad)/255) + ((rgba->red * as)/255);
    src_r4 = tmp < 255 ? (tmp >= 0 ? tmp : 0) : 255;
    tmp = ((src_g4 * ad)/255) + ((rgba->green * as)/255);
    src_g4 = tmp < 255 ? (tmp >= 0 ? tmp : 0) : 255;
    tmp = ((src_b4 * ad)/255) + ((rgba->blue * as)/255);
    src_b4 = tmp < 255 ? (tmp >= 0 ? tmp : 0) : 255;

    *value = ((src_r4 >> 4) << 12) | ((src_g4 >> 4) << 8) | ((src_b4 >> 4) << 4) | src_a4;

    if (num >= 2) {
	if (x < image->width) {
		setPixelAlphaRGBA16( image, x + 1, y, num - 1, rgba );
	} else {
		setPixelAlphaRGBA16( image, 0, y + 1, num - 1, rgba );
	}
    }
}

//-----------------------------------------------------------------------

void
setPixelDitheredRGBA16(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    const RGBA8_T *rgba)
{
    static int16_t dither16[64] =
    {
         1,  12,   4,  15,   1,  13,   4,  15,
         8,   4,  11,   7,   9,   5,  12,   8,
         3,  14,   2,  13,   3,  15,   2,  14,
        10,   6,   9,   5,  11,   7,  10,   6,
         1,  12,   4,  15,   1,  12,   4,  15,
         9,   5,  12,   8,   8,   5,  11,   8,
         3,  14,   2,  13,   3,  14,   2,  13,
        11,   7,  10,   6,  10,   7,   9,   6,
    };

    int32_t i;
    for (i=0; i < num; i++) {
	int32_t index = (x & 7) | ((y & 7) << 3);

	int16_t r = rgba->red + dither16[index];
	if (r > 255) r = 255;

	int16_t g = rgba->green + dither16[index];
	if (g > 255) g = 255;

	int16_t b = rgba->blue + dither16[index];
	if (b > 255) b = 255;

	int16_t a = rgba->alpha + dither16[index];
	if (b > 255) b = 255;

	RGBA8_T dithered = { r, g, b, a };

	setPixelRGBA16(image, x, y, 1, &dithered);

	x++;
	if (x > image->width) {
	    x = 0; y++;
	    if (y > image->height) return;
	}
    }
}

//-----------------------------------------------------------------------

void
setPixelRGBA32(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    const RGBA8_T *rgba)
{
    uint8_t *line = (uint8_t *)(image->buffer) + (y*image->pitch) + (4*x);

    line[0] = rgba->red;
    line[1] = rgba->green;
    line[2] = rgba->blue;
    line[3] = rgba->alpha;
    if (num >= 2)
	memfill( line, sizeof(uint8_t) * 4 * num, sizeof(uint8_t) * 4 );
}

//-----------------------------------------------------------------------

void
setPixelAlphaRGBA32(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int32_t num,
    const RGBA8_T *rgba)
{
    uint8_t *line; line = (uint8_t *)(image->buffer) + (y * image->pitch) + (4 * x);
    int ad = 255 - rgba->alpha;
    int as = rgba->alpha;
    int tmp;

    tmp = ((line[0] * ad)/255) + ((rgba->red * as)/255);
    line[0] = tmp < 255 ? (tmp >= 0 ? tmp : 0) : 255;
    tmp = ((line[1] * ad)/255) + ((rgba->green * as)/255);
    line[1] = tmp < 255 ? (tmp >= 0 ? tmp : 0) : 255;
    tmp = ((line[2] * ad)/255) + ((rgba->blue * as)/255);
    line[2] = tmp < 255 ? (tmp >= 0 ? tmp : 0) : 255;

    if (num >= 2) {
	if (x < image->width) {
		setPixelAlphaRGBA32( image, x + 1, y, num - 1, rgba );
	} else {
		setPixelAlphaRGBA32( image, 0, y + 1, num - 1, rgba );
	}
    }
}

//-----------------------------------------------------------------------

void
getPixel4BPP(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int8_t *index)
{
    uint8_t *value = (uint8_t*)(image->buffer + (x/2) + (y *image->pitch));

    if (x % 2)
    {
        *index = (*value) & 0x0F;
    }
    else
    {
        *index = (*value) >> 4;
    }
}

//-----------------------------------------------------------------------

void
getPixel8BPP(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    int8_t *index)
{
    *index = *(uint8_t*)(image->buffer + x + (y * image->pitch));
}

//-----------------------------------------------------------------------

void
getPixelRGB565(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    RGBA8_T *rgba)
{
    uint16_t pixel = *(uint16_t*)(image->buffer + (x * 2) + (y * image->pitch));

    uint8_t r5 = (pixel >> 11) & 0x1F;
    uint8_t g6 = (pixel >> 5) & 0x3F;
    uint8_t b5 = pixel & 0x1F;

    rgba->red = (r5 << 3) | (r5 >> 2);
    rgba->green = (g6 << 2) | (g6 >> 4);
    rgba->blue = (b5 << 3) | (b5 >> 2);
    rgba->alpha = 255;
}

//-------------------------------------------------------------------------

void
getPixelRGB888(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    RGBA8_T *rgba)
{
    uint8_t *line = (uint8_t *)(image->buffer) + (y*image->pitch) + (3*x);
    rgba->red = line[0];
    rgba->green = line[1];
    rgba->blue = line[2];
    rgba->alpha = 255;
}

//-----------------------------------------------------------------------

void
getPixelRGBA16(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    RGBA8_T *rgba)
{
    uint16_t pixel = *(uint16_t*)(image->buffer+(x*2)+(y*image->pitch));
    uint8_t r4 = (pixel >> 12) & 0xF;
    uint8_t g4 = (pixel >> 8) & 0xF;
    uint8_t b4 = (pixel >> 4) & 0xF;
    uint8_t a4 = (pixel & 0xF);

    rgba->red = (r4 << 4) | r4;
    rgba->green = (g4 << 4) | g4;
    rgba->blue = (b4 << 4) | b4;
    rgba->alpha = (a4 << 4) | a4;
}

//-----------------------------------------------------------------------

void
getPixelRGBA32(
    IMAGE_T *image,
    int32_t x,
    int32_t y,
    RGBA8_T *rgba)
{
    uint8_t *line = (uint8_t *)(image->buffer) + (y*image->pitch) + (4*x);

    rgba->red = line[0];
    rgba->green = line[1];
    rgba->blue = line[2];
    rgba->alpha = line[3];
}

//-----------------------------------------------------------------------

#define IMAGE_INFO_ENTRY(t, ha, ii) \
    { .name=(#t), \
      .type=(VC_IMAGE_ ## t), \
      .hasAlpha=(ha), \
      .isIndexed=(ii) }

IMAGE_TYPE_INFO_T imageTypeInfo[] =
{
    IMAGE_INFO_ENTRY(4BPP, false, true),
    IMAGE_INFO_ENTRY(8BPP, false, true),
    IMAGE_INFO_ENTRY(RGB565, false, false),
    IMAGE_INFO_ENTRY(RGB888, false, false),
    IMAGE_INFO_ENTRY(RGBA16, true, false),
    IMAGE_INFO_ENTRY(RGBA32, true, false)
};

static size_t imageTypeInfoEntries = sizeof(imageTypeInfo)/
                                     sizeof(imageTypeInfo[0]);

//-----------------------------------------------------------------------

bool
findImageType(
    IMAGE_TYPE_INFO_T *typeInfo,
    const char *name,
    IMAGE_TYPE_SELECTOR_T selector)
{
    IMAGE_TYPE_INFO_T *entry = NULL;
    bool found = false;

    size_t i = 0;
    for (i = 0 ; i < imageTypeInfoEntries ; i++)
    {
        if (strcasecmp(name, imageTypeInfo[i].name) == 0)
        {
            entry = &(imageTypeInfo[i]);
            break;
        }
    }

    if (entry != NULL)
    {
        bool matchedAlpha = false;
        bool matchedColour = false;

        if ((selector & IMAGE_TYPES_WITH_ALPHA) &&
            (entry->hasAlpha == true))
        {
            matchedAlpha = true;
        }
        else if ((selector & IMAGE_TYPES_WITHOUT_ALPHA) &&
                 (entry->hasAlpha == false))
        {
            matchedAlpha = true;
        }

        if ((selector & IMAGE_TYPES_DIRECT_COLOUR) &&
                 (entry->isIndexed == false))
        {
            matchedColour = true;
        }
        else if ((selector & IMAGE_TYPES_INDEXED_COLOUR) &&
                 (entry->isIndexed == true))
        {
            matchedColour = true;
        }

        if (matchedAlpha && matchedColour)
        {
            found = true;
            memcpy(typeInfo, entry, sizeof(IMAGE_TYPE_INFO_T));
        }
    }

    return found;
}

//-----------------------------------------------------------------------

void
printImageTypes(
    FILE *fp,
    const char *before,
    const char *after,
    IMAGE_TYPE_SELECTOR_T selector)
{
    IMAGE_TYPE_INFO_T *entry = NULL;

    size_t i = 0;
    for (i = 0 ; i < imageTypeInfoEntries ; i++)
    {
        entry = &(imageTypeInfo[i]);
        bool matchedAlpha = false;
        bool matchedColour = false;

        if ((selector & IMAGE_TYPES_WITH_ALPHA) &&
            (entry->hasAlpha == true))
        {
            matchedAlpha = true;
        }
        else if ((selector & IMAGE_TYPES_WITHOUT_ALPHA) &&
                 (entry->hasAlpha == false))
        {
            matchedAlpha = true;
        }

        if ((selector & IMAGE_TYPES_DIRECT_COLOUR) &&
                 (entry->isIndexed == false))
        {
            matchedColour = true;
        }
        else if ((selector & IMAGE_TYPES_INDEXED_COLOUR) &&
                 (entry->isIndexed == true))
        {
            matchedColour = true;
        }

        if (matchedAlpha && matchedColour)
        {
            fprintf(fp, "%s%s%s", before, entry->name, after);
        }
    }
}

