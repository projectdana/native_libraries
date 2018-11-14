#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <png.h>

static CoreAPI *api;

static GlobalTypeLink *byteArrayGT = NULL;

/*
the below read_png_file and write_png_file functions are based on code by Guillaume Cottenceau and Yoshimasa Niwa
*/

static png_bytep* read_png_file(char *filename, size_t *width, size_t *height)
	{
	png_bytep *row_pointers;
	png_byte color_type;
	png_byte bit_depth;

	FILE *fp = fopen(filename, "rb");
	
	if (fp == NULL) return NULL;

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png)
		{
		fclose(fp);
		return NULL;
		}

	//NOTE: this is not ideal and could use a better solution at some point...
	// - essentially, there are a bunch of PNGs which are "fine" but which libpng dumps a console warning for
	png_set_option(png, PNG_SKIP_sRGB_CHECK_PROFILE, PNG_OPTION_ON);

	png_infop info = png_create_info_struct(png);
	if (!info)
		{
		fclose(fp);
		return NULL;
		}

	if (setjmp(png_jmpbuf(png)))
		{
		fclose(fp);
		return NULL;
		}

	png_init_io(png, fp);

	png_read_info(png, info);

	*width      = png_get_image_width(png, info);
	*height     = png_get_image_height(png, info);
	color_type = png_get_color_type(png, info);
	bit_depth  = png_get_bit_depth(png, info);

	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if (bit_depth == 16)
		png_set_strip_16(png);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if (color_type == PNG_COLOR_TYPE_RGB ||	color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);
	
	png_set_interlace_handling(png);
	
	png_read_update_info(png, info);
	
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * (*height));
	int y = 0;
	for (y = 0; y < (*height); y++)
		{
		row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png,info));
		}
	
	png_read_image(png, row_pointers);
	
	fclose(fp);

	return row_pointers;
	}

static void copyPixels(unsigned char *ar, png_bytep *row_pointers, size_t width, size_t height)
	{
	size_t ia = 0;
	int y = 0;
	int x = 0;
	for(y = 0; y < height; y++)
		{
		png_bytep row = row_pointers[y];
		for(x = 0; x < width; x++)
			{
			png_bytep px = &(row[x * 4]);

			ar[ia] = px[0];
			ia++;
			ar[ia] = px[1];
			ia++;
			ar[ia] = px[2];
			ia++;
			ar[ia] = px[3];
			ia++;
			}
		}
	}

static void copyPixelsOut(unsigned char *ar, png_bytep *row_pointers, size_t width, size_t height)
	{
	size_t ia = 0;
	int y = 0;
	int x = 0;
	for(y = 0; y < height; y++)
		{
		png_bytep row = row_pointers[y];
		for(x = 0; x < width; x++)
			{
			png_bytep px = &(row[x * 4]);

			px[0] = ar[ia];
			ia++;
			px[1] = ar[ia];
			ia++;
			px[2] = ar[ia];
			ia++;
			px[3] = ar[ia];
			ia++;
			}
		}
	}

INSTRUCTION_DEF op_load_image(VFrame *cframe)
	{
	unsigned char res = 0;
	
	char *path = getParam_char_array(cframe, 0);
	
	size_t width = 0;
	size_t height = 0;
	png_bytep *row_pointers = read_png_file(path, &width, &height);
	
	free(path);
	
	if (row_pointers == NULL)
		{
		api -> throwException(cframe, "failed to read file");
		return RETURN_OK;
		}
	
	VVarLivePTR *ct = (VVarLivePTR*) getVariableContent(cframe, 1);
	
	//there are now two pointers: one to the LiveData of the WH instance, and one to the LiveArray of the pixel map
	VVarLivePTR *ict = (VVarLivePTR*) ((LiveData*) ct -> content) -> data;
	
	size_t *num = (size_t*) ((LiveData*) ict -> content) -> data;
	
	copyHostInteger((unsigned char*) num, (unsigned char*) &width, sizeof(width));
	
	num ++;
	
	copyHostInteger((unsigned char*) num, (unsigned char*) &height, sizeof(height));
	
	//...and the pixel map
	ict ++;
	
	LiveArray *array = malloc(sizeof(LiveArray));
	memset(array, '\0', sizeof(LiveArray));
	
	size_t len = (width * 4) * height;
	array -> data = malloc(len);
	array -> length = len;
	
	array -> gtLink = byteArrayGT;
	api -> incrementGTRefCount(array -> gtLink);
	array -> owner = cframe -> blocking -> instance;
	
	array -> refCount ++;
	
	ict -> content = (unsigned char*) array;
	ict -> typeLink = array -> gtLink -> typeLink;
	
	//populate the pixel map (see above helper function)
	copyPixels(array -> data, row_pointers, width, height);
	
	int y = 0;
	for(y = 0; y < height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);
	
	res = 1;
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	return RETURN_OK;
	}

bool write_png_file(png_bytep *row_pointers, size_t width, size_t height, char *filename)
	{
	FILE *fp = fopen(filename, "wb");
	if(!fp) return false;

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) return false;

	png_infop info = png_create_info_struct(png);
	if (!info) return false;

	if (setjmp(png_jmpbuf(png))) return false;

	png_init_io(png, fp);

	// Output is 8bit depth, RGBA format.
	png_set_IHDR(
		png,
		info,
		width, height,
		8,
		PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
		);
	png_write_info(png, info);

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	//png_set_filler(png, 0, PNG_FILLER_AFTER);

	png_write_image(png, row_pointers);
	png_write_end(png, NULL);

	fclose(fp);
	
	return true;
	}

INSTRUCTION_DEF op_save_image(VFrame *cframe)
	{
	unsigned char res = 0;
	
	char *path = getParam_char_array(cframe, 0);
	
	size_t width = 0;
	size_t height = 0;
	
	VVarLivePTR *ct = (VVarLivePTR*) getVariableContent(cframe, 1);
	
	//there are now two pointers: one to the LiveData of the WH instance, and one to the LiveArray of the pixel map
	VVarLivePTR *ict = (VVarLivePTR*) ((LiveData*) ct -> content) -> data;
	
	size_t *num = (size_t*) ((LiveData*) ict -> content) -> data;
	
	copyHostInteger((unsigned char*) &width, (unsigned char*) num, sizeof(width));
	
	num ++;
	
	copyHostInteger((unsigned char*) &height, (unsigned char*) num, sizeof(height));
	
	//...and the pixel map
	ict ++;
	
	LiveArray *array = (LiveArray*) ict -> content;
	
	png_bytep *row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	int y = 0;
	for (y = 0; y < height; y++)
		{
		row_pointers[y] = (png_byte*) malloc(4 * width);
		}
	
	copyPixelsOut(array -> data, row_pointers, width, height);
	
	write_png_file(row_pointers, width, height, path);
	
	for(y = 0; y < height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);
	
	res = 1;
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	free(path);
	
	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	byteArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("byte[]"));
	
	setInterfaceFunction("loadImage", op_load_image);
	
	setInterfaceFunction("saveImage", op_save_image);
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(byteArrayGT);
	}
