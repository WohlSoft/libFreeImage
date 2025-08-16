// ==========================================================
// QOI Loader and Writer
//
// Design and implementation by
// - ds-sloth (ds-sloth@wohlnet.ru)
// - Dominic Szablewski (https://phoboslab.org)
//
// This file is part of FreeImage-Lite 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

/*

Includes material from qoi.h

Copyright (c) 2021, Dominic Szablewski - https://phoboslab.org
SPDX-License-Identifier: MIT

*/

#include "FreeImage.h"
#include "Utilities.h"

#include "../Metadata/FreeImageTag.h"

// ==========================================================
// Plugin Interface
// ==========================================================

static int s_format_id;


// ==========================================================
// Plugin Implementation
// ==========================================================

static const char * DLL_CALLCONV
Format() {
	return "QOI";
}

static const char * DLL_CALLCONV
Description() {
	return "Quite Okay Image";
}

static const char * DLL_CALLCONV
Extension() {
	return "qoi";
}

static const char * DLL_CALLCONV
RegExpr() {
	return "^.QOI\r";
}

static const char * DLL_CALLCONV
MimeType() {
	return "image/qoi";
}

static BOOL DLL_CALLCONV
Validate(FreeImageIO *io, fi_handle handle) {
	BYTE qoi_signature[4] = { 'q', 'o', 'i', 'f' };
	BYTE signature[4] = { 0, 0, 0, 0 };

	io->read_proc(&signature, 1, 4, handle);

	return (memcmp(qoi_signature, signature, 4) == 0);
}

static BOOL DLL_CALLCONV
SupportsExportDepth(int depth) {
	return (
			(depth == 32)
		);
}

static BOOL DLL_CALLCONV
SupportsExportType(FREE_IMAGE_TYPE type) {
	return (
		(type == FIT_BITMAP)
	);
}

static BOOL DLL_CALLCONV
SupportsNoPixels() {
	return TRUE;
}

// --------------------------------------------------------------------------

#ifndef QOI_MALLOC
    #define QOI_MALLOC(sz) malloc(sz)
    #define QOI_FREE(p)    free(p)
#endif
#ifndef QOI_ZEROARR
    #define QOI_ZEROARR(a) memset((a),0,sizeof(a))
#endif

#define QOI_SRGB   0
#define QOI_LINEAR 1
#define BUF_SIZE 4096

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned char channels;
    unsigned char colorspace;
} qoi_desc;

#include <stdlib.h>
#include <string.h>

#define QOI_OP_INDEX  0x00 /* 00xxxxxx */
#define QOI_OP_DIFF   0x40 /* 01xxxxxx */
#define QOI_OP_LUMA   0x80 /* 10xxxxxx */
#define QOI_OP_RUN    0xc0 /* 11xxxxxx */
#define QOI_OP_RGB    0xfe /* 11111110 */
#define QOI_OP_RGBA   0xff /* 11111111 */

#define QOI_MASK_2    0xc0 /* 11000000 */

#define QOI_COLOR_HASH(C) (C.rgba.r*3 + C.rgba.g*5 + C.rgba.b*7 + C.rgba.a*11)
#define QOI_COLOR_HASH_RGBQUAD(C) (C.rgbRed*3 + C.rgbGreen*5 + C.rgbBlue*7 + C.rgbReserved*11)
#define QOI_MAGIC \
    (((unsigned int)'q') << 24 | ((unsigned int)'o') << 16 | \
     ((unsigned int)'i') <<  8 | ((unsigned int)'f'))
#define QOI_HEADER_SIZE 14

/* 2GB is the max file size that this implementation can safely handle. We guard
against anything larger than that, assuming the worst case with 5 bytes per
pixel, rounded down to a nice clean value. 400 million pixels ought to be
enough for anybody. */
#define QOI_PIXELS_MAX ((unsigned int)400000000)

typedef union {
    struct { unsigned char r, g, b, a; } rgba;
    unsigned int v;
} qoi_rgba_t;

static const unsigned char qoi_padding[8] = {0,0,0,0,0,0,0,1};

static void qoi_write_32(unsigned char *bytes, int *p, unsigned int v) {
    bytes[(*p)++] = (0xff000000 & v) >> 24;
    bytes[(*p)++] = (0x00ff0000 & v) >> 16;
    bytes[(*p)++] = (0x0000ff00 & v) >> 8;
    bytes[(*p)++] = (0x000000ff & v);
}

static unsigned int qoi_read_32(const unsigned char *bytes, int *p) {
    unsigned int a = bytes[(*p)++];
    unsigned int b = bytes[(*p)++];
    unsigned int c = bytes[(*p)++];
    unsigned int d = bytes[(*p)++];
    return a << 24 | b << 16 | c << 8 | d;
}

static void *qoi_encode(const void *data, const qoi_desc *desc, int *out_len) {
	int i, max_size, p, run;
	int px_last, px_pos;
	unsigned char *bytes;
	const unsigned char *pixels;
	qoi_rgba_t index[64];
	qoi_rgba_t px, px_prev;

	if (
		data == NULL || out_len == NULL || desc == NULL ||
		desc->width == 0 || desc->height == 0 ||
		desc->channels < 3 || desc->channels > 4 ||
		desc->colorspace > 1 ||
		desc->height >= QOI_PIXELS_MAX / desc->width
	) {
		return NULL;
	}

	max_size =
		desc->width * desc->height * 4 +
		QOI_HEADER_SIZE + sizeof(qoi_padding);

	p = 0;
	bytes = (unsigned char *) QOI_MALLOC(max_size);
	if (!bytes) {
		return NULL;
	}

	qoi_write_32(bytes, &p, QOI_MAGIC);
	qoi_write_32(bytes, &p, desc->width);
	qoi_write_32(bytes, &p, desc->height);
	bytes[p++] = desc->channels;
	bytes[p++] = desc->colorspace;


	pixels = (const unsigned char *)data;

	QOI_ZEROARR(index);

	run = 0;
	px_prev.rgba.r = 0;
	px_prev.rgba.g = 0;
	px_prev.rgba.b = 0;
	px_prev.rgba.a = 255;
	px = px_prev;

	size_t px_stride = desc->width * 4;
	px_last = px_stride - 4;

	for (size_t row = 0; row < desc->height; row += 1) {
		for (px_pos = px_stride * (desc->height - 1 - row); px_pos < px_stride * (desc->height - row); px_pos += 4) {
			px.rgba.r = pixels[px_pos + FI_RGBA_RED];
			px.rgba.g = pixels[px_pos + FI_RGBA_GREEN];
			px.rgba.b = pixels[px_pos + FI_RGBA_BLUE];
			px.rgba.a = pixels[px_pos + FI_RGBA_ALPHA];

			if (px.v == px_prev.v) {
				run++;
				if (run == 62 || px_pos == px_last) {
					bytes[p++] = QOI_OP_RUN | (run - 1);
					run = 0;
				}
			}
			else {
				int index_pos;

				if (run > 0) {
					bytes[p++] = QOI_OP_RUN | (run - 1);
					run = 0;
				}

				index_pos = QOI_COLOR_HASH(px) & (64 - 1);

				if (index[index_pos].v == px.v) {
					bytes[p++] = QOI_OP_INDEX | index_pos;
				}
				else {
					index[index_pos] = px;

					if (px.rgba.a == px_prev.rgba.a) {
						signed char vr = px.rgba.r - px_prev.rgba.r;
						signed char vg = px.rgba.g - px_prev.rgba.g;
						signed char vb = px.rgba.b - px_prev.rgba.b;

						signed char vg_r = vr - vg;
						signed char vg_b = vb - vg;

						if (
							vr > -3 && vr < 2 &&
							vg > -3 && vg < 2 &&
							vb > -3 && vb < 2
						) {
							bytes[p++] = QOI_OP_DIFF | (vr + 2) << 4 | (vg + 2) << 2 | (vb + 2);
						}
						else if (
							vg_r >  -9 && vg_r <  8 &&
							vg   > -33 && vg   < 32 &&
							vg_b >  -9 && vg_b <  8
						) {
							bytes[p++] = QOI_OP_LUMA     | (vg   + 32);
							bytes[p++] = (vg_r + 8) << 4 | (vg_b +  8);
						}
						else {
							bytes[p++] = QOI_OP_RGB;
							bytes[p++] = px.rgba.r;
							bytes[p++] = px.rgba.g;
							bytes[p++] = px.rgba.b;
						}
					}
					else {
						bytes[p++] = QOI_OP_RGBA;
						bytes[p++] = px.rgba.r;
						bytes[p++] = px.rgba.g;
						bytes[p++] = px.rgba.b;
						bytes[p++] = px.rgba.a;
					}
				}
			}
			px_prev = px;
		}
	}

	for (i = 0; i < (int)sizeof(qoi_padding); i++) {
		bytes[p++] = qoi_padding[i];
	}

	*out_len = p;
	return bytes;
}

static FIBITMAP * DLL_CALLCONV
Load(FreeImageIO *io, fi_handle handle, int /*page*/, int flags, void * /*data*/) {
	if (!handle)
		return NULL;

    FIBITMAP *dib = NULL;
    FITAG* tag = NULL;
    unsigned char *bytes = NULL;

	// qoi_decode: local declarations
    qoi_desc _desc;
    qoi_desc* const desc = &_desc;

	try {
		// allocate a read buffer
		bytes = (unsigned char*)QOI_MALLOC(BUF_SIZE);
		if (!bytes) {
			throw FI_MSG_ERROR_MEMORY;
		}

		BOOL header_only = (flags & FIF_LOAD_NOPIXELS) == FIF_LOAD_NOPIXELS;

		// read header into buffer
		if(io->read_proc(bytes, 1, QOI_HEADER_SIZE, handle) != QOI_HEADER_SIZE)
			throw FI_MSG_ERROR_PARSING;

		// qoi_decode: header read
		int p = 0;
		unsigned int header_magic = qoi_read_32(bytes, &p);
		desc->width = qoi_read_32(bytes, &p);
		desc->height = qoi_read_32(bytes, &p);
		desc->channels = bytes[p++];
		desc->colorspace = bytes[p++];

		if (
			desc->width == 0 || desc->height == 0 ||
			desc->channels < 3 || desc->channels > 4 ||
			desc->colorspace > 1 ||
			header_magic != QOI_MAGIC ||
			desc->height >= QOI_PIXELS_MAX / desc->width
		) {
			throw FI_MSG_ERROR_PARSING;
		}

		// allocate bitmap
		dib = FreeImage_AllocateHeaderT(header_only, FIT_BITMAP, desc->width, desc->height, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);

		if (!dib) {
			throw FI_MSG_ERROR_DIB_MEMORY;
		}

		FreeImage_SetTransparent(dib, TRUE);

		// add tags for channels and colorspace
		tag = FreeImage_CreateTag();
		if (!tag) {
			throw FI_MSG_ERROR_MEMORY;
		}

		FreeImage_SetTagLength(tag, 1);
		FreeImage_SetTagCount(tag, 1);
		FreeImage_SetTagType(tag, FIDT_BYTE);

		FreeImage_SetTagKey(tag, "channels");
		FreeImage_SetTagValue(tag, &desc->channels);
		if (!FreeImage_SetMetadata(FIMD_COMMENTS, dib, FreeImage_GetTagKey(tag), tag)) {
			throw FI_MSG_ERROR_MEMORY;
		}

		FreeImage_SetTagKey(tag, "colorspace");
		FreeImage_SetTagValue(tag, &desc->colorspace);
		if (!FreeImage_SetMetadata(FIMD_COMMENTS, dib, FreeImage_GetTagKey(tag), tag)) {
			throw FI_MSG_ERROR_MEMORY;
		}

		FreeImage_DeleteTag(tag);
		tag = NULL;

		if (header_only)
		{
			QOI_FREE(bytes);
			return dib;
		}

		// qoi_decode: load pixel data (prep)
		unsigned char *bytes_end = bytes + io->read_proc(bytes, 1, BUF_SIZE, handle);
		unsigned char *bytes_stale = bytes + BUF_SIZE - 8;
		unsigned char *read_ptr = bytes;

		unsigned char *pixels;
		int run = 0;

		RGBQUAD index[64];
		RGBQUAD px;

		QOI_ZEROARR(index);
		px.rgbRed = 0;
		px.rgbGreen = 0;
		px.rgbBlue = 0;
		px.rgbReserved = 255;

		auto* pixels_base = FreeImage_GetBits(dib);
		auto pixels_stride = FreeImage_GetPitch(dib);

		for (uint32_t row = 0; row < desc->height; row++) {
			// load scanline into buffer

			// get dest
			RGBQUAD* pixels = reinterpret_cast<RGBQUAD*>(pixels_base + pixels_stride * (desc->height - 1 - row));
			RGBQUAD* pixels_end = pixels + desc->width;

			// apply an inter-line run first
			if (run > 0) {
				if (run > pixels_end - pixels) {
					run -= pixels_end - pixels;

					while (pixels < pixels_end) {
						*(pixels++) = px;
					}
				}
				else {
					for (; run > 0; run--) {
						*(pixels++) = px;
					}
				}
			}

			// decode scanline
			while (pixels < pixels_end) {
				// if we're going to overrun bytes array, that's a bug!
				if (read_ptr >= bytes_end) {
					throw FI_MSG_ERROR_PARSING;
				}
				// refill buffer if we're in the last eight bytes
				else if (read_ptr >= bytes_stale) {
					memcpy(bytes, bytes_stale, 8);
					bytes_end -= (BUF_SIZE - 8);
					read_ptr -= (BUF_SIZE - 8);

					if (bytes_end == bytes + 8) {
						bytes_end += io->read_proc(bytes + 8, 1, BUF_SIZE - 8, handle);
					}
				}

				int b1 = *(read_ptr++);

				if (b1 == QOI_OP_RGB) {
					px.rgbRed   = *(read_ptr++);
					px.rgbGreen = *(read_ptr++);
					px.rgbBlue  = *(read_ptr++);
				}
				else if (b1 == QOI_OP_RGBA) {
					px.rgbRed      = *(read_ptr++);
					px.rgbGreen    = *(read_ptr++);
					px.rgbBlue     = *(read_ptr++);
					px.rgbReserved = *(read_ptr++);
				}
				else if ((b1 & QOI_MASK_2) == QOI_OP_INDEX) {
					px = index[b1];
				}
				else if ((b1 & QOI_MASK_2) == QOI_OP_DIFF) {
					px.rgbRed   += ((b1 >> 4) & 0x03) - 2;
					px.rgbGreen += ((b1 >> 2) & 0x03) - 2;
					px.rgbBlue  += ( b1       & 0x03) - 2;
				}
				else if ((b1 & QOI_MASK_2) == QOI_OP_LUMA) {
					int b2 = *(read_ptr++);
					int vg = (b1 & 0x3f) - 32;
					px.rgbRed   += vg - 8 + ((b2 >> 4) & 0x0f);
					px.rgbGreen += vg;
					px.rgbBlue  += vg - 8 +  (b2       & 0x0f);
				}
				else if ((b1 & QOI_MASK_2) == QOI_OP_RUN) {
					run = (b1 & 0x3f) + 1; // add one because we will subtract one for this pixel below

					if (run > pixels_end - pixels) {
						run -= pixels_end - pixels;

						while (pixels < pixels_end) {
							*(pixels++) = px;
						}
					}
					else {
						for (; run > 0; run--) {
							*(pixels++) = px;
						}
					}

					continue;
				}

				index[QOI_COLOR_HASH_RGBQUAD(px) & (64 - 1)] = px;

				*(pixels++) = px;
			}
		}

		// if there aren't 8 bytes (padding) left, or the padding doesn't match, that's a bug!
		if (read_ptr > bytes_end - 8 || memcmp(read_ptr, qoi_padding, 8) != 0) {
			throw FI_MSG_ERROR_PARSING;
		}

		QOI_FREE(bytes);
		return dib;
	} catch (const char *text) {
		if (NULL != text) {
			FreeImage_OutputMessageProc(s_format_id, text);
		}
	}

	if (dib) {
		FreeImage_Unload(dib);
	}

	if (tag) {
		FreeImage_DeleteTag(tag);
	}

	if (bytes) {
		QOI_FREE(bytes);
	}

	return NULL;
}

// --------------------------------------------------------------------------

static BOOL DLL_CALLCONV
Save(FreeImageIO *io, FIBITMAP *dib, fi_handle handle, int /*page*/, int flags, void * /*data*/)
{
	if ((dib) && (handle) && FreeImage_GetBPP(dib) == 32 && FreeImage_GetWidth(dib) * 4 == FreeImage_GetPitch(dib)) {
		try {
			qoi_desc _desc;

			// set width/height
			_desc.width = FreeImage_GetWidth(dib);
			_desc.height = FreeImage_GetHeight(dib);

			// load metadata tags from comments
			FITAG *tag;

			if(FreeImage_GetMetadata(FIMD_COMMENTS, dib, "colorspace", &tag))
				_desc.colorspace = *(const uint8_t*)FreeImage_GetTagValue(tag);
			else
				_desc.colorspace = 0;

			if(FreeImage_GetMetadata(FIMD_COMMENTS, dib, "channels", &tag))
				_desc.channels = *(const uint8_t*)FreeImage_GetTagValue(tag);
			else
				_desc.channels = 0;

			// convert using (roughly vanilla) qoi_encode -- the difference is that it uses FreeImage RGBA offsets, takes a flipped image, and expects RGBA even if channels is 3
			int encoded_byte_count = 0;
			uint8_t* encoded = (uint8_t*)qoi_encode(FreeImage_GetBits(dib), &_desc, &encoded_byte_count);

			if (!encoded || !encoded_byte_count)
				return FALSE;

			// write here
			bool succ = (io->write_proc(encoded, 1, encoded_byte_count, handle) == encoded_byte_count);

			QOI_FREE(encoded);

			return succ;
		} catch (const char *text) {
			FreeImage_OutputMessageProc(s_format_id, text);
		}
	}

	return FALSE;
}

// ==========================================================
//   Init
// ==========================================================

void DLL_CALLCONV
InitQOI(Plugin *plugin, int format_id) {
	s_format_id = format_id;

	plugin->format_proc = Format;
	plugin->description_proc = Description;
	plugin->extension_proc = Extension;
	plugin->regexpr_proc = RegExpr;
	plugin->open_proc = NULL;
	plugin->close_proc = NULL;
	plugin->pagecount_proc = NULL;
	plugin->pagecapability_proc = NULL;
	plugin->load_proc = Load;
	plugin->save_proc = Save;
	plugin->validate_proc = Validate;
	plugin->mime_proc = MimeType;
	plugin->supports_export_bpp_proc = SupportsExportDepth;
	plugin->supports_export_type_proc = SupportsExportType;
	plugin->supports_icc_profiles_proc = NULL;
	plugin->supports_no_pixels_proc = SupportsNoPixels;
}
