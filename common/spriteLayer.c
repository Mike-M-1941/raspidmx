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
#include <stdbool.h>

#include "element_change.h"
#include "image.h"
#include "loadpng.h"
#include "spriteLayer.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

//-------------------------------------------------------------------------

void initSpriteLayerPNG(
    SPRITE_LAYER_T *s,
    int32_t columns,
    int32_t rows,
    const char *file,
    int32_t layer)
{
    s->image = calloc(2, sizeof(IMAGE_T) );

    bool loaded = loadPng( s->image, file);

    if (loaded == false)
    {
        fprintf(stderr, "sprite: unable to load %s\n", file);
        exit(EXIT_FAILURE);
    }
    initSpriteLayerImage(s,columns,rows,NULL,layer);
}

void initSpriteLayerImage(
    SPRITE_LAYER_T *s,
    int32_t columns,
    int32_t rows,
    IMAGE_T *image,
    int32_t layer)
{
    if (image != NULL) s->image = image;
    assert(s->image != NULL);

    s->columns = columns;
    s->rows = rows;
    s->cur_column = 0;
    s->cur_row = 0;
    s->spriteWidth = s->image->width / s->columns;
    s->spriteHeight = s->image->height / s->rows;
    s->xOffsetMax = s->columns * s->spriteWidth;
    s->xOffset = 0;
    s->yOffsetMax = s->rows * s->spriteHeight;
    s->yOffset = 0;
    s->dstOffsetX = 0;
    s->dstOffsetY = 0;
    s->image_write_flag = 1;
    s->change_sprite_flag = 1;
    s->element = 0;

    //---------------------------------------------------------------------

    uint32_t vc_image_ptr = 1;

    s->layer = layer;

    s->frontResource =
        vc_dispmanx_resource_create(
            s->image->type,
            s->image->width | (s->image->pitch << 16),
            s->image->height | (s->image->alignedHeight << 16),
            &vc_image_ptr);
    assert(s->frontResource != 0);

    s->backResource =
        vc_dispmanx_resource_create(
            s->image->type,
            s->image->width | (s->image->pitch << 16),
            s->image->height | (s->image->alignedHeight << 16),
            &vc_image_ptr);
    assert(s->backResource != 0);

    int result = vc_dispmanx_rect_set( &(s->fullRect), 0, 0, s->image->width, s->image->height );
    assert(result == 0);

    writeFlagSpriteLayer( s );
}

//-------------------------------------------------------------------------

void
addElementSpriteLayerCentered(
    SPRITE_LAYER_T *s,
    DISPMANX_MODEINFO_T *info,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update)
{
    s->dstOffsetX = (info->width - s->spriteWidth) / 2;
    s->dstOffsetY = (info->height - s->spriteHeight) / 2;

    addElementSpriteLayer(s, display, update);
}

//-------------------------------------------------------------------------
void
addElementSpriteLayerOffset(
    SPRITE_LAYER_T *s,
    DISPMANX_MODEINFO_T *info,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update,
    int16_t xOffset, int16_t yOffset)
{
    s->dstOffsetX = xOffset;
    s->dstOffsetY = yOffset;

    addElementSpriteLayer(s, display, update);
}

//-------------------------------------------------------------------------

void
addElementSpriteLayer(
    SPRITE_LAYER_T *s,
    DISPMANX_DISPLAY_HANDLE_T display,
    DISPMANX_UPDATE_HANDLE_T update)
{
    int result = vc_dispmanx_rect_set( &(s->srcRect), s->xOffset << 16, s->yOffset << 16, s->spriteWidth << 16, s->spriteHeight << 16 );
    assert(result == 0);
    result = vc_dispmanx_rect_set( &(s->dstRect), s->dstOffsetX, s->dstOffsetY, s->spriteWidth, s->spriteHeight );
    assert(result == 0);

    VC_DISPMANX_ALPHA_T alpha =
    {
        DISPMANX_FLAGS_ALPHA_FROM_SOURCE, 
        255, /*alpha 0->255*/
        0
    };

    //---------------------------------------------------------------------

    s->element =
        vc_dispmanx_element_add(update,
                                display,
                                s->layer,
                                &(s->dstRect),
                                s->frontResource,
                                &(s->srcRect),
                                DISPMANX_PROTECTION_NONE,
                                &alpha,
                                NULL, // clamp
                                DISPMANX_NO_ROTATE);
    assert(s->element != 0);
}

//-------------------------------------------------------------------------

void
setCurrentSpriteXY(
    SPRITE_LAYER_T *s,
    int32_t cur_column,
    int32_t cur_row)
{
    int32_t old_row = s->cur_row;
    int32_t old_col = s->cur_column;

    if (cur_column >= s->columns) { cur_column = s->columns - 1; }
    if (cur_row >= s->rows)       { cur_row = s->rows - 1; }

    s->cur_column = cur_column;
    s->cur_row = cur_row;

    if (s->cur_row != old_row || s->cur_column != old_col) s->change_sprite_flag = 1;
}


//-------------------------------------------------------------------------

void
setCurrentSpriteINC(
    SPRITE_LAYER_T *s)
{
    if (s->columns == 1 && s->rows > 1) {
	s->cur_row++;
	if (s->cur_row >= s->rows) s->cur_row = 0;
    } else if (s->columns > 1 && s->rows == 1) {
	s->cur_column++;
	if (s->cur_column >= s->columns) s->cur_column = 0;
    } else if (s->columns > 1 && s->rows > 1) {
	s->cur_column++;
	if (s->cur_column >= s->columns) {
	    s->cur_column = 0;
	    s->cur_row++;
	    if (s->cur_row >= s->rows) s->cur_row = 0;
	}
    }
    s->change_sprite_flag = 1;
}

void
setCurrentSpriteNUM(
    SPRITE_LAYER_T *s,
    int32_t sprite_num)
{
    int32_t old_row = s->cur_row;
    int32_t old_col = s->cur_column;
    s->cur_row = 0;
    s->cur_column = 0;

    if (s->columns == 1 && s->rows > 1) {
	s->cur_row = sprite_num % s->rows;
    } else if (s->columns > 1 && s->rows == 1) {
	s->cur_column = sprite_num % s->columns;
    } else if (s->columns > 1 && s->rows > 1) {
	s->cur_row = sprite_num / s->columns;
	s->cur_column = sprite_num % s->columns;
    }
    if (s->cur_row != old_row || s->cur_column != old_col) s->change_sprite_flag = 1;
}


// indicate that we updated the image buffer so we
// need to push out those changes on the next screen update.
// This does not include switching sprite frames but rather the
// underlying image data for the sprite.
void writeFlagSpriteLayer(
    SPRITE_LAYER_T *s)
{
    int result = vc_dispmanx_resource_write_data( s->backResource, s->image->type, s->image->pitch, s->image->buffer, &(s->fullRect) );
    assert(result == 0);

    result = vc_dispmanx_resource_write_data( s->frontResource, s->image->type, s->image->pitch, s->image->buffer, &(s->fullRect) );
    assert(result == 0);

    if (s->element != 0) {
	DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
	assert(update != 0);
	result = vc_dispmanx_element_modified(update, s->element, &(s->dstRect) );
	assert(result == 0);
	result = vc_dispmanx_update_submit_sync(update);
	assert(result == 0);
    }

    s->image_write_flag = 1;
}

/*    result = vc_dispmanx_rect_set(&(s->fullRect),
                         0,
                         0,
                         s->image->width,
                         s->image->height);
    assert(result == 0);

    result = vc_dispmanx_resource_write_data(s->frontResource,
                                             s->image->type,
                                             s->image->pitch,
                                             s->image->buffer,
                                             &(s->fullRect));
    assert(result == 0);
    result = vc_dispmanx_resource_write_data(s->backResource,
                                             s->image->type,
                                             s->image->pitch,
                                             s->image->buffer,
                                             &(s->fullRect));
    assert(result == 0);
*/

void
updateSpriteLayer(
    SPRITE_LAYER_T *s,
    DISPMANX_UPDATE_HANDLE_T update)
{
    int result = 0;

    if (s->image_write_flag == 0 && s->change_sprite_flag == 0) return;
    s->image_write_flag = 0;
    s->change_sprite_flag = 0;

    result = vc_dispmanx_element_change_source( update, s->element, s->backResource );
    assert(result == 0);

    s->xOffset = s->cur_column * s->spriteWidth;
    s->yOffset = s->cur_row * s->spriteHeight;

    result = vc_dispmanx_rect_set( &(s->srcRect), s->xOffset << 16, s->yOffset << 16, s->spriteWidth << 16, s->spriteHeight << 16);
    assert(result == 0);

    result = vc_dispmanx_element_change_attributes( update, s->element, ELEMENT_CHANGE_SRC_RECT,
		0, 255, &(s->dstRect), &(s->srcRect), 0, DISPMANX_NO_ROTATE);
    assert(result == 0);

    DISPMANX_RESOURCE_HANDLE_T tmp = s->frontResource;
    s->frontResource = s->backResource;
    s->backResource = tmp;
}

//-------------------------------------------------------------------------

void
destroySpriteLayer(
    SPRITE_LAYER_T *s)
{
    int result = 0;

    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);
    result = vc_dispmanx_element_remove(update, s->element);
    assert(result == 0);
    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);

    //---------------------------------------------------------------------

    result = vc_dispmanx_resource_delete(s->frontResource);
    assert(result == 0);
    result = vc_dispmanx_resource_delete(s->backResource);
    assert(result == 0);

    //---------------------------------------------------------------------

    destroyImage(s->image);
}

