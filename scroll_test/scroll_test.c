//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
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

#define _GNU_SOURCE

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "backgroundLayer.h"
#include "image.h"
#include "imageLayer.h"
#include "imageGraphics.h"
#include "scrollingLayer.h"
#include "key.h"

#include "bcm_host.h"

//-------------------------------------------------------------------------

#define NDEBUG

//-------------------------------------------------------------------------

int	s_width;
int	s_height;
int	margin = 15;
int	i = 0;

IMAGE_LAYER_T		the_border;
SCROLLING_LAYER_T	horz_scroll;
SCROLLING_LAYER_T	vert_scroll;
IMAGE_T			horz_image;
IMAGE_T			vert_image;

RGBA8_T black_color  = {  0,   0,   0, 255};
RGBA8_T white_color  = {255, 255, 255, 255};
RGBA8_T red_color    = {255,   0,   0, 255};
RGBA8_T green_color  = {  0, 255,   0, 255};
RGBA8_T blue_color   = {  0,   0, 255, 255};
RGBA8_T yellow_color = {255, 255,   0, 255};
RGBA8_T purple_color = {128,   0, 128, 255};
RGBA8_T clear_color  = {  0,   0,   0,   0};


//-------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    srand(time(NULL));

    bcm_host_init();

    DISPMANX_DISPLAY_HANDLE_T display = vc_dispmanx_display_open(0);
    assert(display != 0);

    DISPMANX_MODEINFO_T info;
    int result = vc_dispmanx_display_get_info(display, &info);
    assert(result == 0);
    s_width = info.width;
    s_height = info.height;

    //---------------------------------------------------------------------

    BACKGROUND_LAYER_T backgroundLayer;
    initBackgroundLayer(&backgroundLayer, 0x000F, 0);

    initImageLayer( &the_border, s_width, s_height, VC_IMAGE_RGBA32 );
    createResourceImageLayer( &the_border, 10);
    clearImageRGB( &(the_border.image), &clear_color);
    imageBoxFilledRGB( &(the_border.image), 0, 0, s_width, margin, &green_color );
    imageBoxFilledRGB( &(the_border.image), 0, s_height - margin, s_width, s_height, &green_color );
    imageBoxFilledRGB( &(the_border.image), 0, 0, margin, s_height, &green_color );
    imageBoxFilledRGB( &(the_border.image), s_width - margin, 0, s_width, s_height, &green_color );
    changeSourceAndUpdateImageLayer( &the_border );


    initImage( &horz_image, VC_IMAGE_RGBA32, s_width * 3, s_height - margin - margin, false );
    clearImageRGB( &horz_image, &clear_color );
    for (i=0; i < 100; i++) {
	RGBA8_T rgb;
	rgb.red =  (rand() % 16) << 4;
	rgb.green =  (rand() % 16) << 4;
	rgb.blue =  (rand() % 16) << 4;
	rgb.alpha =  (rand() % 16) << 4;
	int x = rand() % 30;
	int y = rand() % 10;
	int x1 = ((s_width * 3) / 30) * x;
	int x2 = ((s_width * 3) / 30) * (x + 1);
	int y1 = ((s_height - margin - margin) / 10) * y;
	int y2 = ((s_height - margin - margin) / 10) * (y + 1);
	imageBoxFilledRGB( &horz_image, x1, y1, x2, y2, &rgb );
    }
    initScrollingLayerImage( &horz_scroll, &horz_image, s_width - margin - margin, s_height - margin - margin, 5);
    setDirectionScrollingLayer( &horz_scroll, 4, 0 );


    initImage( &vert_image, VC_IMAGE_RGBA32, s_width - margin - margin, s_height * 3, false );
    clearImageRGB( &vert_image, &clear_color );
    for (i=0; i < 100; i++) {
	RGBA8_T rgb;
	rgb.red =  (rand() % 16) << 4;
	rgb.green =  (rand() % 16) << 4;
	rgb.blue =  (rand() % 16) << 4;
	rgb.alpha =  (rand() % 16) << 4;
	int x = rand() % 10;
	int y = rand() % 30;
	int x1 = ((s_width - margin - margin) / 10) * x;
	int x2 = ((s_width - margin - margin) / 10) * (x + 1);
	int y1 = ((s_height * 3) / 30) * y;
	int y2 = ((s_height * 3) / 30) * (y + 1);
	imageBoxFilledRGB( &vert_image, x1, y1, x2, y2, &rgb );
    }
    expand_and_duplicate_Image( &vert_image, 0, s_height );
    initScrollingLayerImage( &vert_scroll, &vert_image, s_width - margin - margin, s_height - margin - margin, 3);
    setDirectionScrollingLayer( &vert_scroll, 0, 3 );

    //---------------------------------------------------------------------

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);

    addElementBackgroundLayer( &backgroundLayer, display, update );
    addElementImageLayerOffset( &the_border, 0, 0, display, update );

    addElementScrollingLayerOffsetView(&horz_scroll, &info, display, update,
	0, 0, s_width * 3, s_height - margin - margin,
	margin, margin, s_width - margin - margin, s_height - margin - margin);

    addElementScrollingLayerOffsetView(&vert_scroll, &info, display, update,
	0, 0, s_width - margin - margin, s_height * 3,
	margin, margin, s_width - margin - margin, s_height - margin - margin);

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    //---------------------------------------------------------------------

    while (keyPressed(NULL) == false) {
	DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
	assert(update != 0);

	setScrollingLayer( &horz_scroll );
	updateScrollingLayer( &horz_scroll, update );

	setScrollingLayer( &vert_scroll );
	updateScrollingLayer( &vert_scroll, update );

	int result = vc_dispmanx_update_submit_sync(update);
	assert(result == 0);

        //usleep(100000);
    }

    keyboardReset();

    //destroyBackgroundLayer(&backgroundLayer);
    //destroyImageLayer(&imageLayer);

    result = vc_dispmanx_display_close(display);
    assert(result == 0);

    return 0;
}

