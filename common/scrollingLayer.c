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
#include <ctype.h>
#include <stdbool.h>

#include "element_change.h"
#include "image.h"
#include "loadpng.h"
#include "scrollingLayer.h"

#include "bcm_host.h"

//-------------------------------------------------------------------------

void
initScrollingLayerPNG(SCROLLING_LAYER_T *sl,
    const char* file,
    int32_t layer)
{
    sl->image = calloc(1, sizeof(IMAGE_T) );

    bool loaded = loadPng( sl->image, file);

    int32_t max_x = sl->image->width;
    int32_t max_y = sl->image->height;
    if (loaded) expand_and_duplicate_Image( sl->image, sl->image->width, sl->image->height );

    if (loaded == false)
    {
        fprintf(stderr, "scrolling: unable to load %s\n", file);
        exit(EXIT_FAILURE);
    }
    initScrollingLayerImage( sl, NULL, max_x, max_y, layer);
}


void
initScrollingLayerImage(SCROLLING_LAYER_T *sl,
    IMAGE_T *image,
    int32_t max_x,
    int32_t max_y,
    int32_t layer)
{
    if (image != NULL) sl->image = image;
    assert(sl->image != NULL);

    sl->xOffsetMax = max_x;
    sl->yOffsetMax = max_y;
    sl->viewWidth = sl->viewHeight = 0;
    sl->xOffset = sl->yOffset = 0;
    sl->xStepper = sl->yStepper = 0;
    sl->image_write_flag = 1;
    sl->scroll_step_flag = 1;
    sl->element = 0;

    uint32_t vc_image_ptr = 1;

    sl->layer = layer;

    sl->frontResource =
        vc_dispmanx_resource_create(
            sl->image->type,
            sl->image->width | (sl->image->pitch << 16),
            sl->image->height | (sl->image->alignedHeight << 16),
            &vc_image_ptr);
    assert(sl->frontResource != 0);

    sl->backResource =
        vc_dispmanx_resource_create(
            sl->image->type,
            sl->image->width | (sl->image->pitch << 16),
            sl->image->height | (sl->image->alignedHeight << 16),
            &vc_image_ptr);
    assert(sl->backResource != 0);

    //---------------------------------------------------------------------

    writeFlagScrollingLayer( sl );
}

//-------------------------------------------------------------------------

void
addElementScrollingLayerCentered(
    SCROLLING_LAYER_T *sl,
    DISPMANX_MODEINFO_T *info,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update)
{
    sl->viewWidth = sl->xOffsetMax;
    sl->viewHeight = sl->yOffsetMax;

    if (sl->viewWidth > info->width)   sl->viewWidth = info->width;
    if (sl->viewHeight > info->height) sl->viewHeight = info->height;

    sl->dstOffsetX = (info->width - sl->viewWidth) / 2;
    sl->dstOffsetY = (info->height - sl->viewHeight) / 2;

    addElementScrollingLayer(sl, display, update);
}

//-------------------------------------------------------------------------
void
addElementScrollingLayerOffsetView(
    SCROLLING_LAYER_T *sl,
    DISPMANX_MODEINFO_T *info,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update,
    int32_t src_x_offset, int32_t src_y_offset,
    int32_t src_x_max,    int32_t src_y_max,
    int32_t dst_x_offset, int32_t dst_y_offset,
    int32_t dst_width,    int32_t dst_height)
{
    sl->xOffsetMax = src_x_max;
    sl->xOffset = src_x_offset;

    sl->yOffsetMax = src_y_max;
    sl->yOffset = src_y_offset;

    sl->viewWidth  = dst_width;
    sl->viewHeight = dst_height;

    sl->dstOffsetX = dst_x_offset;
    sl->dstOffsetY = dst_y_offset;

    if (sl->viewWidth > info->width) sl->viewWidth = info->width;
    if (sl->viewHeight > info->height) sl->viewHeight = info->height;

    addElementScrollingLayer(sl, display, update);
}

//-------------------------------------------------------------------------

void
addElementScrollingLayer(
    SCROLLING_LAYER_T *sl,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update)
{
    int result = vc_dispmanx_rect_set(&sl->srcRect,
                         sl->xOffset << 16,
                         sl->yOffset << 16,
                         sl->viewWidth << 16,
                         sl->viewHeight << 16);
    assert(result == 0);

    result = vc_dispmanx_rect_set(&(sl->dstRect),
                         sl->dstOffsetX,
                         sl->dstOffsetY,
                         sl->viewWidth,
                         sl->viewHeight);
    assert(result == 0);

    VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FROM_SOURCE, 255, 0 };

    sl->element = vc_dispmanx_element_add(update,
                                          display,
                                          sl->layer,
                                          &(sl->dstRect),
                                          sl->frontResource,
                                          &(sl->srcRect),
                                          DISPMANX_PROTECTION_NONE,
                                          &alpha,
                                          NULL,
                                          DISPMANX_NO_ROTATE);
    assert(sl->element != 0);
}

//-------------------------------------------------------------------------

void
setDirectionScrollingLayer(
    SCROLLING_LAYER_T *sl,
    int16_t x_step, int16_t y_step)
{
    sl->xStepper = x_step;
    sl->yStepper = y_step;
}

//-------------------------------------------------------------------------

void
setScrollingLayer(
    SCROLLING_LAYER_T *sl)
{
    sl->xOffset += sl->xStepper;

    if (sl->xOffset < 0) {
        sl->xOffset = sl->xOffsetMax + sl->xOffset;
    } else if (sl->xOffset > sl->xOffsetMax) {
        sl->xOffset = sl->xOffset - sl->xOffsetMax;
    }

    sl->yOffset += sl->yStepper;

    if (sl->yOffset < 0) {
        sl->yOffset = sl->yOffsetMax + sl->yOffset;
    } else if (sl->yOffset > sl->yOffsetMax) {
        sl->yOffset = sl->yOffset - sl->yOffsetMax;
    }
    sl->scroll_step_flag = 1;
}

//-------------------------------------------------------------------------

void writeFlagScrollingLayer(
    SCROLLING_LAYER_T *sl)
{
    int result = vc_dispmanx_rect_set(&(sl->fullRect),
                         0,
                         0,
                         sl->image->width,
                         sl->image->height);
    assert(result == 0);

    result = vc_dispmanx_resource_write_data(sl->frontResource,
                                             sl->image->type,
                                             sl->image->pitch,
                                             sl->image->buffer,
                                             &(sl->fullRect));
    assert(result == 0);

    result = vc_dispmanx_resource_write_data(sl->backResource,
                                             sl->image->type,
                                             sl->image->pitch,
                                             sl->image->buffer,
                                             &(sl->fullRect));
    assert(result == 0);

    if (sl->element != 0) {
	DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
	assert(update != 0);
	result = vc_dispmanx_element_modified(update, sl->element, &(sl->dstRect) );
	assert(result == 0);
	result = vc_dispmanx_update_submit_sync(update);
	assert(result == 0);
    }

    sl->image_write_flag = 1;
}

//-------------------------------------------------------------------------

void
updateScrollingLayer(
    SCROLLING_LAYER_T *sl,
    DISPMANX_UPDATE_HANDLE_T update)
{
    if (sl->image_write_flag == 0 && sl->scroll_step_flag == 0) return;
    sl->image_write_flag = 0;
    sl->scroll_step_flag = 0;

    int result = vc_dispmanx_element_change_source( update, sl->element, sl->backResource);
    assert(result == 0);

    result = vc_dispmanx_rect_set(&(sl->srcRect),
                         sl->xOffset << 16,
                         sl->yOffset << 16,
                         sl->viewWidth << 16,
                         sl->viewHeight << 16);
    assert(result == 0);

    result = 
    vc_dispmanx_element_change_attributes(update,
                                          sl->element,
                                          ELEMENT_CHANGE_SRC_RECT,
                                          0,
                                          255,
                                          &(sl->dstRect),
                                          &(sl->srcRect),
                                          0,
                                          DISPMANX_NO_ROTATE);
    assert(result == 0);

    //---------------------------------------------------------------------

    DISPMANX_RESOURCE_HANDLE_T tmp = sl->frontResource;
    sl->frontResource = sl->backResource;
    sl->backResource = tmp;
}

//-------------------------------------------------------------------------

void
destroyScrollingLayer(
    SCROLLING_LAYER_T *sl)
{
    int result = 0;

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);
    result = vc_dispmanx_element_remove(update, sl->element);
    assert(result == 0);
    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    //---------------------------------------------------------------------

    result = vc_dispmanx_resource_delete(sl->frontResource);
    assert(result == 0);
    result = vc_dispmanx_resource_delete(sl->backResource);
    assert(result == 0);

    //---------------------------------------------------------------------

    destroyImage(sl->image);
}

//-------------------------------------------------------------------------

