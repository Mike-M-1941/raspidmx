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
#include <stdlib.h>
#include <math.h>

#include "image.h"
#include "imageGraphics.h"

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

void
imageBoxIndexed(
    IMAGE_T *image,
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    int8_t index)
{
    imageVerticalLineIndexed(image, x1, y1, y2, index);
    imageHorizontalLineIndexed(image, x1, x2, y1, index);
    imageVerticalLineIndexed(image, x2, y1, y2, index);
    imageHorizontalLineIndexed(image, x1, x2, y2, index);
}

//-------------------------------------------------------------------------

void
imageBoxRGB(
    IMAGE_T *image,
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    const RGBA8_T *rgb)
{
    imageVerticalLineRGB(image, x1, y1, y2, rgb);
    imageHorizontalLineRGB(image, x1, x2, y1, rgb);
    imageVerticalLineRGB(image, x2, y1, y2, rgb);
    imageHorizontalLineRGB(image, x1, x2, y2, rgb);
}

//-------------------------------------------------------------------------

void
imageBoxFilledIndexed(
    IMAGE_T *image,
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    int8_t index)
{
    int32_t y = y1;
    if (y1 > y2) {
	y = y2;
	y2 = y1;
    }
    int32_t x = x1;
    if (x1 > x2) {
	x = x2;
	x2 = x1;
    }

    if (x == 0 && x2 == image->width) {
	setPixelIndexed(image, x, y, (y2-y) * image->width, index);
	return;
    }

    while (y <= y2) {
        imageHorizontalLineIndexed(image, x, x2, y, index);
        y++;
    }
}

//-------------------------------------------------------------------------

void
imageBoxFilledRGB(
    IMAGE_T *image,
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    const RGBA8_T *rgb)
{
    int32_t y = y1;
    if (y1 > y2) {
	y = y2;
	y2 = y1;
    }
    int32_t x = x1;
    if (x1 > x2) {
	x = x2;
	x2 = x1;
    }

    if (x == 0 && x2 == image->width) {
	setPixelRGB(image, x, y, (y2-y) * image->width, rgb);
	return;
    }

    while (y <= y2) {
        imageHorizontalLineRGB(image, x, x2, y, rgb);
        y++;
    }
}

//-------------------------------------------------------------------------

void
imageLineIndexed(
    IMAGE_T *image,
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    int8_t index)
{
    if (y1 == y2)
    {
        imageHorizontalLineIndexed(image, x1, x2, y1, index);
    }
    else if (x1 == x2)
    {
        imageVerticalLineIndexed(image, x1, y1, y2, index);
    }
    else
    {
        int32_t dx = abs(x2 - x1);
        int32_t dy = abs(y2 - y1);

        int32_t sign_x = (x1 <= x2) ? 1 : -1;
        int32_t sign_y = (y1 <= y2) ? 1 : -1;

        int32_t x = x1;
        int32_t y = y1;

        setPixelIndexed(image, x, y, 1, index);

        if (dx > dy)
        {
            int32_t d = 2 * dy - dx;
            int32_t incrE = 2 * dy;
            int32_t incrNE = 2 * (dy - dx);

            while (x != x2)
            {
                x += sign_x;

                if (d <= 0)
                {
                    d += incrE;
                }
                else
                {
                    d += incrNE;
                    y += sign_y;
                }

                setPixelIndexed(image, x, y, 1, index);
            }
        }
        else
        {
            int32_t d = 2 * dx - dy;
            int32_t incrN = 2 * dx;
            int32_t incrNE = 2 * (dx - dy);

            while (y != y2)
            {
                y += sign_y;

                if (d <= 0)
                {
                    d += incrN;
                }
                else
                {
                    d += incrNE;
                    x += sign_x;
                }

                setPixelIndexed(image, x, y, 1, index);
            }
        }
    }
}

//-------------------------------------------------------------------------

void
imageLineRGB(
    IMAGE_T *image,
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    const RGBA8_T *rgb)
{
    if (y1 == y2)
    {
        imageHorizontalLineRGB(image, x1, x2, y1, rgb);
    }
    else if (x1 == x2)
    {
        imageVerticalLineRGB(image, x1, y1, y2, rgb);
    }
    else
    {
        int32_t dx = abs(x2 - x1);
        int32_t dy = abs(y2 - y1);

        int32_t sign_x = (x1 <= x2) ? 1 : -1;
        int32_t sign_y = (y1 <= y2) ? 1 : -1;

        int32_t x = x1;
        int32_t y = y1;

        setPixelRGB(image, x, y, 1, rgb);

        if (dx > dy)
        {
            int32_t d = 2 * dy - dx;
            int32_t incrE = 2 * dy;
            int32_t incrNE = 2 * (dy - dx);

            while (x != x2)
            {
                x += sign_x;

                if (d <= 0)
                {
                    d += incrE;
                }
                else
                {
                    d += incrNE;
                    y += sign_y;
                }

                setPixelRGB(image, x, y, 1, rgb);
            }
        }
        else
        {
            int32_t d = 2 * dx - dy;
            int32_t incrN = 2 * dx;
            int32_t incrNE = 2 * (dx - dy);

            while (y != y2)
            {
                y += sign_y;

                if (d <= 0)
                {
                    d += incrN;
                }
                else
                {
                    d += incrNE;
                    x += sign_x;
                }

                setPixelRGB(image, x, y, 1, rgb);
            }
        }
    }
}

//-------------------------------------------------------------------------

void
imageHorizontalLineIndexed(
    IMAGE_T *image,
    int32_t x1,
    int32_t x2,
    int32_t y,
    int8_t index)
{
    int32_t x = x1;
    if (x1 > x2) {
	x = x2;
	x2 = x1;
    }

    setPixelIndexed(image, x, y, (x2 - x), index);
}

//-------------------------------------------------------------------------

void
imageHorizontalLineRGB(
    IMAGE_T *image,
    int32_t x1,
    int32_t x2,
    int32_t y,
    const RGBA8_T *rgb)
{
    int32_t x = x1;
    if (x1 > x2) {
	x = x2;
	x2 = x1;
    }

    setPixelRGB(image, x, y, (x2 - x), rgb);
}

//-------------------------------------------------------------------------

void
imageVerticalLineIndexed(
    IMAGE_T *image,
    int32_t x,
    int32_t y1,
    int32_t y2,
    int8_t index)
{
    int32_t sign_y = (y1 <= y2) ? 1 : -1;
    int32_t y = y1;

    setPixelIndexed(image, x, y, 1, index);

    while (y != y2)
    {
        y += sign_y;
        setPixelIndexed(image, x, y, 1, index);
    }
}

//-------------------------------------------------------------------------

void
imageVerticalLineRGB(
    IMAGE_T *image,
    int32_t x,
    int32_t y1,
    int32_t y2,
    const RGBA8_T *rgb)
{
    int32_t sign_y = (y1 <= y2) ? 1 : -1;
    int32_t y = y1;

    setPixelRGB(image, x, y, 1, rgb);

    while (y != y2)
    {
        y += sign_y;
        setPixelRGB(image, x, y, 1, rgb);
    }
}

//-------------------------------------------------------------------------

void
imagePolygonFilledIndexed(
    IMAGE_T *image,
    POLYGON_T *poly,
    int8_t index)
{

}

//-------------------------------------------------------------------------

void
imagePolygonFilledRGB(
    IMAGE_T *image,
    POLYGON_T *poly,
    const RGBA8_T *rgb)
{
    int  min_y =  9999999;
    int  max_y = -9999999;
    int  nodes, pixelY, i, j;
    double nodeX[200];

    for (i=0; i < poly->points; i++) {
	if (poly->p[i].y > max_y) max_y = poly->p[i].y;
	if (poly->p[i].y < min_y) min_y = poly->p[i].y;
    }

    //  Loop through the rows of the image.
    for (pixelY=min_y; pixelY < max_y; pixelY++) {
	double line_y = pixelY;
	//  Build a list of nodes.
	nodes = 0;
	j = poly->points - 1;
	for (i=0; i < poly->points; i++) {
	    double px1 = poly->p[i].x;
	    double py1 = poly->p[i].y;
	    double px2 = poly->p[j].x;
	    double py2 = poly->p[j].y;
	    if ((py1 < line_y && py2 >= line_y) || (py2 < line_y && py1 >= line_y)) {
		    nodeX[nodes++]= (px1 + (line_y - py1) / (py2 - py1) * (px2 - px1) );
		    //printf( "line %d  %g \n", pixelY, nodeX[nodes - 1] );
	    }
	    j = i;
	}

	//  Sort the nodes, via a simple “Bubble” sort.
	i = 0;
	while (i < (nodes - 1)) {
	    if (nodeX[i] > nodeX[i+1]) {
		double swap = nodeX[i];
		nodeX[i] = nodeX[i+1];
		nodeX[i+1] = swap;
		if (i) i--;
	    } else {
		i++;
	    }
	}

	//  Fill the pixels between node pairs.
	//   3.2	6.6
	//   .8 4------6 .6
	for (i=0; i < nodes; i+= 2) {
	    RGBA8_T fp = *rgb;
	    fp.alpha = (1.0 - (nodeX[i] - floor(nodeX[i]))) * 255.0;
	    int32_t x0 = floor(nodeX[i]);
	    int32_t x1 = ceil(nodeX[i]);

	    RGBA8_T lp = *rgb;
	    lp.alpha =  ((nodeX[i+1] - floor(nodeX[i+1]))) * 255.0;
	    int32_t x2 = ceil(nodeX[i+1]);
	    int32_t x9 = x2 + 1;

	//    printf( "draw line YY %d   %g %g     x0 %d = %d  x1 %d -- x2 %d  x9 %d = %d\n",
	//	pixelY, nodeX[i], nodeX[i+1], x0, fp.alpha, x1, x2, x9, lp.alpha );
	    setPixelRGBA( image, x0, pixelY, 1, &fp );
	    setPixelRGBA( image, x9, pixelY, 1, &lp );
	    //setPixelRGBA( image, x0 + 10, pixelY, 1, rgb ); //&fp );
	    //setPixelRGBA( image, x9 + 10, pixelY, 1, rgb ); //&lp );
	    imageHorizontalLineRGB( image, x1, x2, pixelY, rgb );
	}
    }
}

//-------------------------------------------------------------------------

void
setPolygonNodes( POLYGON_T *poly, int num, ...)
{
	assert(poly != NULL);
	assert(num > 0);

	va_list arguments;
	va_start( arguments, num );

	if (poly->p != NULL) free( poly->p );
	poly->p = calloc(num, sizeof(POLYPOINT_T) );
	poly->points = 0;

	int i;
	for (i=0; i < num; i++ ) {
		poly->p[i].x = va_arg ( arguments, int );
		poly->p[i].y = va_arg ( arguments, int );
		poly->points++;
	}
	va_end ( arguments );
}
