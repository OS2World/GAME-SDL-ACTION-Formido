/*
    Formido - a shooting game (see README for additional information)
    Copyright (C) 2002, 2003 Mika Halttunen

    You can contact me by email: lsoft@mbnet.fi
    -----------------------------------------------------------------

    This file is part of Formido.

    Formido is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Formido is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Formido; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include "SDL.h"
#include "blend.h"
#include "init.h"


// Color macros from SDL_blit.h
#define RGB_FROM_PIXEL(pixel, fmt, r, g, b)				\
{									\
	r = (((pixel&fmt->Rmask)>>fmt->Rshift)<<fmt->Rloss); 		\
	g = (((pixel&fmt->Gmask)>>fmt->Gshift)<<fmt->Gloss); 		\
	b = (((pixel&fmt->Bmask)>>fmt->Bshift)<<fmt->Bloss); 		\
}

#define PIXEL_FROM_RGB(pixel, fmt, r, g, b)				\
{									\
	pixel = ((r>>fmt->Rloss)<<fmt->Rshift)|				\
		((g>>fmt->Gloss)<<fmt->Gshift)|				\
		((b>>fmt->Bloss)<<fmt->Bshift);				\
}


///////////////////////
// LOOK-UP TABLES
///////////////////////

// Y * screen->pitch table
int y_table[SCREEN_H];

// X * screen->format->BytesPerPixel table
int x_table[SCREEN_W];

// Alpha tables
// a_table[color][alpha] = color * (alpha/32),
// where color is 0-255 and alpha is 0-32
// Table is computed when init_blend() is called
int a_table[256][33];

// Huge alpha table for 8-bit
// Notice that there is only 8 alpha levels in 8-bit
Uint8 a_table8[256][256][8];

// Function pointers for BPP-specifig get_pix/put_pix
Uint32 (*get_pix)(Uint8 *p);
void (*put_pix)(Uint8 *p, Uint32 color);


// Getpixel and putpixel functions for each color depth
inline Uint32 get_pix8(Uint8 *p) {
	return *p;
}

inline Uint32 get_pix16(Uint8 *p) {
	return *(Uint16*)p;
}

inline Uint32 get_pix24(Uint8 *p) {
	if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
		return (p[0] << 16 | p[1] << 8 | p[2]);
	else
		return (p[0] | p[1] << 8 | p[2] << 16);
}

inline Uint32 get_pix32(Uint8 *p) {
	return *(Uint32*)p;
}


inline void put_pix8(Uint8 *p, Uint32 color) {
	*p = color;
}

inline void put_pix16(Uint8 *p, Uint32 color) {
	*(Uint16*)p = color;
}

inline void put_pix24(Uint8 *p, Uint32 color) {
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        p[0] = (color >> 16) & 0xff;
        p[1] = (color >> 8) & 0xff;
        p[2] = color & 0xff;
    } else {
        p[0] = color & 0xff;
        p[1] = (color >> 8) & 0xff;
        p[2] = (color >> 16) & 0xff;
    }
}

inline void put_pix32(Uint8 *p, Uint32 color) {
	*(Uint32*)p = color;
}


/*
inline Uint32 get_pix(SDL_Surface *src, Uint8 *p, int bpp) {

	//Uint8 *p = (Uint8*)src->pixels + y * src->pitch + x*bpp;

	switch(bpp) {
		case 1:
			return *p;
		case 2:
			return *(Uint16*)p;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return (p[0] << 16 | p[1] << 8 | p[2]);
			else
				return (p[0] | p[1] << 8 | p[2] << 16);

		case 4:
			return *(Uint32*)p;

		default:
			return 0;
	}

}


inline void put_pix(SDL_Surface *dst, Uint8 *p, int bpp, Uint32 color) {

    //Uint8 *p = (Uint8 *)dst->pixels + y * dst->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = color;
        break;

    case 2:
        *(Uint16 *)p = color;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (color >> 16) & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = color & 0xff;
        } else {
            p[0] = color & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = (color >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = color;
        break;
    }
}*/


// Initialize look-up tables
void init_blend() {

	// Set the getpix/putpix pointers
	switch(screen->format->BytesPerPixel) {
		case 2:
			get_pix = get_pix16;
			put_pix = put_pix16;
			break;
		case 3:
			get_pix = get_pix24;
			put_pix = put_pix24;
			break;
		case 4:
			get_pix = get_pix32;
			put_pix = put_pix32;
			break;
	}


	int f;

	// Y * screen->pitch table
	if(SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);
	for(f=0; f<SCREEN_H; f++)
		y_table[f] = f * screen->pitch;
	if(SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	// X * screen->format->BytesPerPixel table
	for(f=0; f<SCREEN_W; f++)
		x_table[f] = f * screen->format->BytesPerPixel;

	// High- / True-color tables
	if(!screen->format->palette) {
		for(int c=0; c<256; c++) {
			for(int a=0; a<33; a++) {
				a_table[c][a] = (int)(c * ((float)a/32.0f));
			}
		}
	}
	// 8-bit tables
	else {
		// First, try to load tables from 'data/alpha8.tab'
		FILE *fin = fopen("data/alpha8.tab", "rb");
		if(fin) {
			// Load the tables
			fread(a_table8, sizeof(a_table8), 1, fin);
			fclose(fin);
		}
		else {
			// Compute the table and save it
			int c1,c2;

			for(c1=0; c1<screen->format->palette->ncolors; c1++) {
				for(c2=0; c2<screen->format->palette->ncolors; c2++) {

					for(int a=0; a<8; a++) {
						int r,g,b;
						float aa = (float)a/7.0f;
						r = (int)(screen->format->palette->colors[c1].r * aa);
						g = (int)(screen->format->palette->colors[c1].g * aa);
						b = (int)(screen->format->palette->colors[c1].b * aa);

						r += screen->format->palette->colors[c2].r;
						g += screen->format->palette->colors[c2].g;
						b += screen->format->palette->colors[c2].b;
						if(r > 255) r = 255;
						if(g > 255) g = 255;
						if(b > 255) b = 255;

						a_table8[c1][c2][a] = SDL_MapRGB(screen->format, r,g,b);
					}
				}
			}

			// Save the table to disk
			FILE *fout = fopen("data/alpha8.tab", "wb");
			if(fout) {
				fwrite(a_table8, sizeof(a_table8), 1, fout);
				fclose(fout);
			}
		}
	}
}




// Simple additive blender
// Destination surface must be locked!
// Doesn't work with 8-bit modes!
void add_blit(SDL_Surface *src, SDL_Surface *dst, int x, int y) {

	// Source color depth
	int bpps = src->format->BytesPerPixel;

	if(SDL_MUSTLOCK(src))
		SDL_LockSurface(src);

	// Clipping
	int cx=0, cy=0;
	int sw = src->w, sh = src->h;
	if(x < 0) {
		cx -= x;
	}
	if(y < 0) {
		cy -= y;
	}
	if(x+sw >= dst->w) {
		sw += (dst->w-(x+sw));
	}
	if(y+sh >= dst->h) {
		sh += (dst->h-(y+sh));
	}

	int fyy, fxx;
	int fypitch;

	// Loop through the source and add each pixel value
	for(int fy=cy; fy < sh; fy++) {

		fyy = y + fy;
		fypitch = fy  * src->pitch;

		for(int fx=cx; fx < sw; fx++) {

			fxx = x + fx;

			// Grab the color and components from source
		    Uint8 *p;
			p = (Uint8 *)src->pixels + fypitch + fx * bpps;
			Uint32 src_color = get_pix(p);
			if(src_color == 0)
				continue;

			Uint8 sr,sg,sb;
			RGB_FROM_PIXEL(src_color, src->format, sr,sg,sb);

			// Grab the color and components from destination
			p = (Uint8 *)dst->pixels + y_table[fyy] + x_table[fxx];
			Uint32 dst_color = get_pix(p);
			Uint8 dr,dg,db;
			RGB_FROM_PIXEL(dst_color, dst->format, dr,dg,db);

			// Add them together
			int r,g,b;
			r = (dr + sr);
			g = (dg + sg);
			b = (db + sb);
			if(r > 255) r = 255;
			if(g > 255) g = 255;
			if(b > 255) b = 255;

			Uint32 final_color;
			PIXEL_FROM_RGB(final_color, dst->format, r,g,b);

			// Write the new pixel to destination
			p = (Uint8 *)dst->pixels + y_table[fyy] + x_table[fxx];
			put_pix(p, final_color);

		}
	}

	if(SDL_MUSTLOCK(src))
		SDL_UnlockSurface(src);

}


// Same as above, just with variable alpha (0-255)
void add_blit(SDL_Surface *src, SDL_Surface *dst, int x, int y, int alpha) {

	// Source color depth
	int bpps = src->format->BytesPerPixel;

	// Adjust the alpha to 0-32 range
	alpha = alpha >> 3;

	if(alpha <= 0)
		return;

	if(SDL_MUSTLOCK(src))
		SDL_LockSurface(src);

	// Clipping
	int cx=0, cy=0;
	int sw = src->w, sh = src->h;
	if(x < 0) {
		cx -= x;
	}
	if(y < 0) {
		cy -= y;
	}
	if(x+sw >= dst->w) {
		sw += (dst->w-(x+sw));
	}
	if(y+sh >= dst->h) {
		sh += (dst->h-(y+sh));
	}

	int fyy, fxx;
	int fypitch;

	// Loop through the source and add each pixel value
	for(int fy=cy; fy < sh; fy++) {

		fyy = y + fy;
		fypitch = fy * src->pitch;

		for(int fx=cx; fx < sw; fx++) {

			fxx = x + fx;

			// Grab the color and components from source
		    Uint8 *p;
			p = (Uint8 *)src->pixels + fypitch + fx * bpps;
			Uint32 src_color = get_pix(p);
			if(src_color == 0)
				continue;

			Uint8 sr,sg,sb;
			RGB_FROM_PIXEL(src_color, src->format, sr,sg,sb);

			// Grab the color and components from destination
			p = (Uint8 *)dst->pixels + y_table[fyy] + x_table[fxx];
			Uint32 dst_color = get_pix(p);
			Uint8 dr,dg,db;
			RGB_FROM_PIXEL(dst_color, dst->format, dr,dg,db);

			// Add them together
			int r,g,b;
			r = (dr + a_table[sr][alpha]);
			g = (dg + a_table[sg][alpha]);
			b = (db + a_table[sb][alpha]);
			if(r > 255) r = 255;
			if(g > 255) g = 255;
			if(b > 255) b = 255;

			Uint32 final_color;
			PIXEL_FROM_RGB(final_color, dst->format, r,g,b);

			// Write the new pixel to destination
			p = (Uint8 *)dst->pixels + y_table[fyy] + x_table[fxx];
			put_pix(p, final_color);

		}
	}

	if(SDL_MUSTLOCK(src))
		SDL_UnlockSurface(src);


}


// Additive blended pixel
// Destination surface must be locked!
void add_pixel(SDL_Surface *dst, int x, int y, Uint8 sr, Uint8 sg, Uint8 sb, int alpha) {

	// Clipping
	if(x < 0 || y < 0 || x >= dst->w || y >= dst->h)
		return;

	if(sr == 0 && sg == 0 && sb == 0)
		return;

	// Adjust the alpha to 0-32 range
	alpha = alpha >> 3;

	if(alpha <= 0)
		return;

	// Grab the color and components from destination
	Uint8 *p = (Uint8 *)dst->pixels + y_table[y] + x_table[x];
	Uint32 dst_color = get_pix(p);
	Uint8 dr,dg,db;
	RGB_FROM_PIXEL(dst_color, dst->format, dr,dg,db);

	// Add them together
	int r,g,b;
	r = (dr + a_table[sr][alpha]);
	g = (dg + a_table[sg][alpha]);
	b = (db + a_table[sb][alpha]);
	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;

	Uint32 final_color;
	PIXEL_FROM_RGB(final_color, dst->format, r,g,b);

	// Write the new pixel to destination
	p = (Uint8 *)dst->pixels + y_table[y] + x_table[x];
	put_pix(p, final_color);

}


/////////////////////
// 8-bit versions
/////////////////////

void add_blit8(SDL_Surface *src, SDL_Surface *dst, int x, int y) {

	// Source color depth
	int bpps = src->format->BytesPerPixel;

	if(SDL_MUSTLOCK(src))
		SDL_LockSurface(src);

	// Clipping
	int cx=0, cy=0;
	int sw = src->w, sh = src->h;
	if(x < 0) {
		cx -= x;
	}
	if(y < 0) {
		cy -= y;
	}
	if(x+sw >= dst->w) {
		sw += (dst->w-(x+sw));
	}
	if(y+sh >= dst->h) {
		sh += (dst->h-(y+sh));
	}

	int fyy, fxx;
	int fypitch;

	// Loop through the source and add each pixel value
	for(int fy=cy; fy < sh; fy++) {

		fyy = y + fy;
		fypitch = fy * src->pitch;

		for(int fx=cx; fx < sw; fx++) {

			fxx = x + fx;

			// Grab the color from source
		    Uint8 *p;
			p = (Uint8 *)src->pixels + fypitch + fx * bpps;
			Uint32 src_color = get_pix8(p);

			// Grab the color from destination
			p = (Uint8 *)dst->pixels + y_table[fyy] + x_table[fxx];
			Uint32 dst_color = get_pix8(p);

			// Add them together
			Uint32 final_color;
			final_color = a_table8[src_color][dst_color][7];

			// Write the new pixel to destination
			p = (Uint8 *)dst->pixels + y_table[fyy] + x_table[fxx];
			put_pix8(p, final_color);

		}
	}

	if(SDL_MUSTLOCK(src))
		SDL_UnlockSurface(src);

}


void add_blit8(SDL_Surface *src, SDL_Surface *dst, int x, int y, int alpha) {


	// Source color depth
	int bpps = src->format->BytesPerPixel;

	// Adjust the alpha to 0-7 range
	alpha = alpha >> 5;
	if(alpha > 7)
		alpha = 7;
	if(alpha <= 0)
		return;

	if(SDL_MUSTLOCK(src))
		SDL_LockSurface(src);

	// Clipping
	int cx=0, cy=0;
	int sw = src->w, sh = src->h;
	if(x < 0) {
		cx -= x;
	}
	if(y < 0) {
		cy -= y;
	}
	if(x+sw >= dst->w) {
		sw += (dst->w-(x+sw));
	}
	if(y+sh >= dst->h) {
		sh += (dst->h-(y+sh));
	}

	int fyy, fxx;
	int fypitch;

	// Loop through the source and add each pixel value
	for(int fy=cy; fy < sh; fy++) {

		fyy = y + fy;
		fypitch = fy * src->pitch;

		for(int fx=cx; fx < sw; fx++) {

			fxx = x + fx;

			// Grab the color from source
		    Uint8 *p;
			p = (Uint8 *)src->pixels + fypitch + fx * bpps;
			Uint32 src_color = get_pix8(p);

			// Grab the color from destination
			p = (Uint8 *)dst->pixels + y_table[fyy] + x_table[fxx];
			Uint32 dst_color = get_pix8(p);

			// Add them together
			Uint32 final_color;
			final_color = a_table8[src_color][dst_color][alpha];

			// Write the new pixel to destination
			p = (Uint8 *)dst->pixels + y_table[fyy] + x_table[fxx];
			put_pix8(p, final_color);

		}
	}

	if(SDL_MUSTLOCK(src))
		SDL_UnlockSurface(src);

}


void add_pixel8(SDL_Surface *dst, int x, int y, Uint8 sr, Uint8 sg, Uint8 sb, int alpha) {

	// Clipping
	if(x < 0 || y < 0 || x >= dst->w || y >= dst->h)
		return;

	if(sr == 0 && sg == 0 && sb == 0)
		return;

	// Adjust the alpha to 0-7 range
	alpha = alpha >> 5;
	if(alpha > 7)
		alpha = 7;
	if(alpha <= 0)
		return;

	// Grab the color from destination
	Uint8 *p = (Uint8 *)dst->pixels + y_table[y] + x_table[x];
	Uint32 dst_color = get_pix8(p);

	// Add them together
	Uint32 final_color;
	final_color = a_table8[SDL_MapRGB(screen->format, sr,sg,sb)][dst_color][alpha];

	// Write the new pixel to destination
	p = (Uint8 *)dst->pixels + y_table[y] + x_table[x];
	put_pix8(p, final_color);

}


