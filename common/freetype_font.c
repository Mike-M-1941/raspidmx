/* example1.c                                                      */
/*                                                                 */
/* This small program shows how to print a rotated string with the */
/* FreeType 2 library.                                             */

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
//#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include "image.h"


void draw_bitmap_Indexed( FT_Bitmap* bitmap, FT_Int x, FT_Int y, IMAGE_T *image, int ddx, int ddy, int8_t index ) {
	FT_Int  i, j, p, q;
	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;
	int	mx = image->width;
	int	my = image->height;

	for ( j = y, q = 0; j < y_max; j++, q++ ) {
		int dy = ddy - (y_max - j) + (bitmap->rows - y);
		for ( i = x, p = 0; i < x_max; i++, p++ ) {
			if ( i < 0 || j < 0 || i >= mx || j >= my ) continue;
			uint8_t  src = bitmap->buffer[q * bitmap->width + p];
			if (src > 127) setPixelIndexed( image, ddx + i, ddy + j, 1, index );
		}
	}
}

void draw_bitmap_RGB( FT_Bitmap* bitmap, FT_Int x, FT_Int y, IMAGE_T *image, int ddx, int ddy, const RGBA8_T *rgb ) {
	FT_Int  i, j, p, q;
	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;
	int	mx = image->width;
	int	my = image->height;
	RGBA8_T my_rgb = *rgb;

	for ( j = y, q = 0; j < y_max; j++, q++ ) {
		int dy = ddy - (y_max - j) + (bitmap->rows - y);
		for ( i = x, p = 0; i < x_max; i++, p++ ) {
			if ( i < 0 || dy < 0 || i >= mx || dy >= my ) continue;
			uint8_t  src = bitmap->buffer[q * bitmap->width + p];
			if (src == 0) continue;

			my_rgb.alpha = src;
			setPixelRGBA( image, ddx + i, dy, 1, &my_rgb ); // use alpha blend
		}
	}
}


static FT_Library	library;
static FT_Face		*font_face = NULL;
static int		fonts = 0;
static int		max_fonts = 0;

void init_Freetype_Render( void ) {
	FT_Error      error;

	error = FT_Init_FreeType( &library );              /* initialize library */
	assert(error == 0);
}

uint8_t load_Freetype_Font( const char *font_name ) {
	struct stat	st;
	int		result;

	result = stat( font_name, &st );
	assert(result == 0);

	if (fonts == 0) {
		font_face = calloc(10, sizeof(FT_Face) );
		FT_New_Face( library, font_name, 0, &( font_face[0] ) );
		fonts++;
		max_fonts = 10;
		return 0;
	}
	if (fonts == max_fonts) {
		max_fonts += 10;
		font_face = realloc( font_face, sizeof(FT_Face) * max_fonts );
	}
	FT_New_Face( library, font_name, 0, &( font_face[fonts] ) );
	fonts++;
	return(fonts - 1);
}

/* CHAR *******************************************************************************************************/
/* CHAR *******************************************************************************************************/

int measure_FT_Char( uint8_t c, uint8_t font, uint16_t size ) {
	FT_GlyphSlot	g_slot;
	FT_Vector	pen;
	FT_Error	error;

	assert(font < fonts);
	g_slot = font_face[font]->glyph;

	error = FT_Set_Char_Size( font_face[font], size * 18, size * 27, 256, 256 );
	assert(error == 0);

	pen.x = 20; pen.y = 20;

	FT_Set_Transform( font_face[font], 0, &pen );
	error = FT_Load_Char( font_face[font], c, FT_LOAD_RENDER );
	if ( error )  return;                 /* ignore errors */

	return( g_slot->bitmap_left + g_slot->bitmap.width );
}

void draw_FT_CharIndexed( int x, int y, uint8_t c, uint8_t font, uint16_t size, int8_t index, IMAGE_T *image) {
	FT_GlyphSlot	g_slot;
	FT_Vector	pen;
	FT_Error	error;

	assert(font < fonts);
	g_slot = font_face[font]->glyph;

	error = FT_Set_Char_Size( font_face[font], size * 18, size * 27, 256, 256 );
	assert(error == 0);

	pen.x = 20; pen.y = 20;

	FT_Set_Transform( font_face[font], 0, &pen );
	error = FT_Load_Char( font_face[font], c, FT_LOAD_RENDER );
	if ( error )  return;                 /* ignore errors */

	draw_bitmap_Indexed( &g_slot->bitmap, g_slot->bitmap_left, g_slot->bitmap_top, image, x, y, index );
}

void draw_FT_CharRGB( int x, int y, uint8_t c, uint8_t font, uint16_t size, const RGBA8_T *rgb, IMAGE_T *image) {
	FT_GlyphSlot	g_slot;
	FT_Vector	pen;
	FT_Error	error;

	assert(font < fonts);
	g_slot = font_face[font]->glyph;

	error = FT_Set_Char_Size( font_face[font], size * 18, size * 27, 256, 256 );
	assert(error == 0);

	pen.x = 20; pen.y = 20;

	FT_Set_Transform( font_face[font], 0, &pen );
	error = FT_Load_Char( font_face[font], c, FT_LOAD_RENDER );
	if ( error )  return;                 /* ignore errors */

	draw_bitmap_RGB( &g_slot->bitmap, g_slot->bitmap_left, g_slot->bitmap_top, image, x, y, rgb );
}

/* STRING *******************************************************************************************************/
/* STRING *******************************************************************************************************/


int measure_FT_String( const char *string, uint8_t font, uint16_t size) {
	FT_GlyphSlot	g_slot;
	FT_Vector	pen;
	FT_Error	error;
	int		n, nc;

	assert(font < fonts);
	g_slot = font_face[font]->glyph;

	nc = strlen( string );
	if (nc == 0) return;

	error = FT_Set_Char_Size( font_face[font], size * 18, size * 27, 256, 256 );
	assert(error == 0);

	pen.x = 20; pen.y = 20;
	int max_x = 0;

	for (n = 0; n < nc; n++ ) {
		FT_Set_Transform( font_face[font], 0, &pen );
		error = FT_Load_Char( font_face[font], string[n], FT_LOAD_RENDER );
		if ( error )  return(max_x);                 /* ignore errors */

		max_x = g_slot->bitmap.width + g_slot->bitmap_left;
		pen.x += g_slot->advance.x;
		pen.y += g_slot->advance.y;
	}
	return( max_x );
}

void draw_FT_StringIndexed( int x, int y, const char *string, uint8_t font, uint16_t size, int8_t index, IMAGE_T *image) {
	FT_GlyphSlot	g_slot;
	FT_Vector	pen;
	FT_Error	error;
	int		n, nc;

	assert(font < fonts);
	g_slot = font_face[font]->glyph;

	nc = strlen( string );
	if (nc == 0) return;

	error = FT_Set_Char_Size( font_face[font], size * 18, size * 27, 256, 256 );
	assert(error == 0);

	pen.x = 20; pen.y = 20;

	for (n = 0; n < nc; n++ ) {
		FT_Set_Transform( font_face[font], 0, &pen );
		error = FT_Load_Char( font_face[font], string[n], FT_LOAD_RENDER );
		if ( error )  return;                 /* ignore errors */

		draw_bitmap_Indexed( &g_slot->bitmap, g_slot->bitmap_left, g_slot->bitmap_top, image, x, y, index );
		pen.x += g_slot->advance.x;
		pen.y += g_slot->advance.y;
	}
}

void draw_FT_StringRGB( int x, int y, const char *string, uint8_t font, uint16_t size, const RGBA8_T *rgb, IMAGE_T *image) {
	FT_GlyphSlot	g_slot;
	FT_Vector	pen;
	FT_Error	error;
	int		n, nc;

	assert(font < fonts);
	g_slot = font_face[font]->glyph;

	nc = strlen( string );
	if (nc == 0) return;

	error = FT_Set_Char_Size( font_face[font], size * 18, size * 27, 256, 256 );
	assert(error == 0);

	pen.x = 20; pen.y = 20;

	for (n = 0; n < nc; n++ ) {
		FT_Set_Transform( font_face[font], 0, &pen );
		error = FT_Load_Char( font_face[font], string[n], FT_LOAD_RENDER );
		if ( error )  return;                 /* ignore errors */

		draw_bitmap_RGB( &g_slot->bitmap, g_slot->bitmap_left, g_slot->bitmap_top, image, x, y, rgb );
		pen.x += g_slot->advance.x;
		pen.y += g_slot->advance.y;
	}
}
