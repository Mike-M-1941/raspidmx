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

#ifndef SPRITE_LAYER_H
#define SPRITE_LAYER_H

#include "image.h"

#include "bcm_host.h"

//-------------------------------------------------------------------------

typedef struct
{
    IMAGE_T *image;
    uint8_t image_write_flag;
    uint8_t change_sprite_flag;
    int32_t spriteWidth;
    int32_t spriteHeight;
    int32_t columns;
    int32_t rows;
    int32_t cur_column;
    int32_t cur_row;
    int32_t xOffsetMax;
    int32_t xOffset;
    int32_t yOffsetMax;
    int32_t yOffset;
    int32_t dstOffsetX;
    int32_t dstOffsetY;
    VC_RECT_T srcRect;
    VC_RECT_T dstRect;
    VC_RECT_T fullRect;
    int32_t layer;
    DISPMANX_RESOURCE_HANDLE_T frontResource;
    DISPMANX_RESOURCE_HANDLE_T backResource;
    DISPMANX_ELEMENT_HANDLE_T element;
} SPRITE_LAYER_T;

//-------------------------------------------------------------------------

void initSpriteLayerPNG(
    SPRITE_LAYER_T *s,
    int32_t columns,
    int32_t rows,
    const char *file,
    int32_t layer);


void initSpriteLayerImage(
    SPRITE_LAYER_T *s,
    int32_t columns,
    int32_t rows,
    IMAGE_T *image,
    int32_t layer);

//-------------------------------------------------------------------------

void
addElementSpriteLayerCentered(
    SPRITE_LAYER_T *s,
    DISPMANX_MODEINFO_T *info,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update);

void
addElementSpriteLayerOffset(
    SPRITE_LAYER_T *s,
    DISPMANX_MODEINFO_T *info,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update,
    int16_t xOffset, int16_t yOffset);

void
addElementSpriteLayer(
    SPRITE_LAYER_T *s,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update);

//-------------------------------------------------------------------------

void
setCurrentSpriteXY(
    SPRITE_LAYER_T *s,
    int32_t cur_column,
    int32_t cur_row);

void
setCurrentSpriteINC(
    SPRITE_LAYER_T *s);

void
setCurrentSpriteNUM(
    SPRITE_LAYER_T *s,
    int32_t sprite_num);

//-------------------------------------------------------------------------

void writeFlagSpriteLayer(
    SPRITE_LAYER_T *s);

void
updateSpriteLayer(
    SPRITE_LAYER_T *s,
    DISPMANX_UPDATE_HANDLE_T update);

void destroySpriteLayer(SPRITE_LAYER_T *s);

//-------------------------------------------------------------------------

#endif
