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

#ifndef SCROLLING_LAYER_H
#define SCROLLING_LAYER_H

#include <stdbool.h>

#include "image.h"

#include "bcm_host.h"

//-------------------------------------------------------------------------

typedef struct
{
    IMAGE_T *image;
    uint8_t image_write_flag;
    uint8_t scroll_step_flag;
    int32_t viewWidth;
    int32_t viewHeight;
    int32_t xOffsetMax;
    int32_t xOffset;
    int32_t yOffsetMax;
    int32_t yOffset;
    int32_t dstOffsetX;
    int32_t dstOffsetY;
    int16_t xStepper;
    int16_t yStepper;
    VC_RECT_T srcRect;
    VC_RECT_T dstRect;
    VC_RECT_T fullRect;
    int32_t layer;
    DISPMANX_RESOURCE_HANDLE_T frontResource;
    DISPMANX_RESOURCE_HANDLE_T backResource;
    DISPMANX_ELEMENT_HANDLE_T element;
} SCROLLING_LAYER_T;

//-------------------------------------------------------------------------

void
initScrollingLayerPNG(SCROLLING_LAYER_T *sl,
    const char* file,
    int32_t layer);

void
initScrollingLayerImage(SCROLLING_LAYER_T *sl,
    IMAGE_T *image,
    int32_t max_x,
    int32_t max_y,
    int32_t layer);

//-------------------------------------------------------------------------

void
addElementScrollingLayerCentered(
    SCROLLING_LAYER_T *sl,
    DISPMANX_MODEINFO_T *info,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update);

void
addElementScrollingLayerOffsetView(
    SCROLLING_LAYER_T *sl,
    DISPMANX_MODEINFO_T *info,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update,
    int32_t src_x_offset, int32_t src_y_offset,
    int32_t src_x_max,    int32_t src_y_max,
    int32_t dst_x_offset, int32_t dst_y_offset,
    int32_t dst_width,    int32_t dst_height);

void
addElementScrollingLayer(
    SCROLLING_LAYER_T *sl,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update);

//-------------------------------------------------------------------------

void setDirectionScrollingLayer(
    SCROLLING_LAYER_T *sl,
    int16_t x_step,
    int16_t y_step);

void setScrollingLayer(
    SCROLLING_LAYER_T *sl);

//-------------------------------------------------------------------------

void writeFlagScrollingLayer(
    SCROLLING_LAYER_T *sl);

void
updateScrollingLayer(
    SCROLLING_LAYER_T *sl,
    DISPMANX_UPDATE_HANDLE_T update);

void destroyScrollingLayer(SCROLLING_LAYER_T *sl);

//-------------------------------------------------------------------------

#endif
