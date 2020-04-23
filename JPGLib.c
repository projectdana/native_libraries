#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>

#include <setjmp.h>

#include <jpeglib.h>

// libjpeg source at http://www.ijg.org/

static size_t DANA_PIXEL_WIDTH = 4;

static CoreAPI *api;

static GlobalTypeLink *byteArrayGT = NULL;

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  // (*cinfo->err->output_message) (cinfo); //TODO: make this into an exception...

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

/*
the below read_file function is based on code by Kenneth Finnegan (2012)
*/

static unsigned char* read_file(char *filename, size_t *out_width, size_t *out_height)
	{
	int rc, i;
	
	// Variables for the source jpg
	struct stat file_info;
	unsigned long jpg_size;
	unsigned char *jpg_buffer;

	// Variables for the decompressor itself
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	// Variables for the output buffer, and how long each row is
	unsigned long bmp_size;
	unsigned char *bmp_buffer;
	int row_stride, width, height, pixel_size;
	
	memset(&cinfo, '\0', sizeof(cinfo));
	
	rc = stat(filename, &file_info);
	if (rc) {
		return NULL;
	}
	jpg_size = file_info.st_size;
	jpg_buffer = (unsigned char*) malloc(jpg_size + 100);

	FILE *fd = fopen(filename, "rb");
	/*
	i = 0;
	while (i < jpg_size) {
		rc = fread(jpg_buffer + i, sizeof(unsigned char), jpg_size - i, fd);
		i += rc;
	}
	fclose(fd);
*/
	
	//configure custom error handling...
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	
	if (setjmp(jerr.setjmp_buffer))
		{
		jpeg_destroy_decompress(&cinfo);
		fclose(fd);
		return NULL;
		}
	
	jpeg_create_decompress(&cinfo);
	//jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);
	
	jpeg_stdio_src(&cinfo, fd);
	
	rc = jpeg_read_header(&cinfo, TRUE);
	
	if (rc != 1) {
		return NULL;
	}
	
	// By calling jpeg_start_decompress, you populate cinfo
	// and can then allocate your output bitmap buffers for
	// each scanline.
	jpeg_start_decompress(&cinfo);
	
	width = cinfo.output_width;
	height = cinfo.output_height;
	pixel_size = cinfo.output_components;

	//printf("Image is %d by %d with %d components", width, height, pixel_size);

	bmp_size = width * height * pixel_size;
	bmp_buffer = (unsigned char*) malloc(bmp_size);

	// The row_stride is the total number of bytes it takes to store an
	// entire scanline (row). 
	row_stride = width * pixel_size;
	
	//
	// Now that you have the decompressor entirely configured, it's time
	// to read out all of the scanlines of the jpeg.
	//
	// By default, scanlines will come out in RGBRGBRGB...  order, 
	// but this can be changed by setting cinfo.out_color_space
	//
	// jpeg_read_scanlines takes an array of buffers, one for each scanline.
	// Even if you give it a complete set of buffers for the whole image,
	// it will only ever decompress a few lines at a time. For best 
	// performance, you should pass it an array with cinfo.rec_outbuf_height
	// scanline buffers. rec_outbuf_height is typically 1, 2, or 4, and 
	// at the default high quality decompression setting is always 1.
	while (cinfo.output_scanline < cinfo.output_height)
		{
		unsigned char *buffer_array[1];
		buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * row_stride;

		jpeg_read_scanlines(&cinfo, buffer_array, 1);
		}


	// Once done reading *all* scanlines, release all internal buffers,
	// etc by calling jpeg_finish_decompress. This lets you go back and
	// reuse the same cinfo object with the same settings, if you
	// want to decompress several jpegs in a row.
	//
	// If you didn't read all the scanlines, but want to stop early,
	// you instead need to call jpeg_abort_decompress(&cinfo)
	jpeg_finish_decompress(&cinfo);

	// At this point, optionally go back and either load a new jpg into
	// the jpg_buffer, or define a new jpeg_mem_src, and then start 
	// another decompress operation.
	
	// Once you're really really done, destroy the object to free everything
	jpeg_destroy_decompress(&cinfo);
	// And free the input buffer
	free(jpg_buffer);
	
	*out_width = width;
	*out_height = height;
	
	fclose(fd);

	return bmp_buffer;
	}

//NOTE: assumes RGB format; in future can check # of components in file for actual details (usually 1 or 3)...
static void copyPixels(unsigned char *ar, unsigned char *jpg_pixels, size_t width, size_t height)
	{
	size_t ia = 0;
	int y = 0;
	int x = 0;
	for(y = 0; y < height; y++)
		{
		size_t rindex = y * (width*3);
		unsigned char *row = &jpg_pixels[rindex];
		for(x = 0; x < width; x++)
			{
			unsigned char *px = &(row[x * 3]);

			ar[ia] = px[0];
			ia++;
			ar[ia] = px[1];
			ia++;
			ar[ia] = px[2];
			ia++;
			ar[ia] = 255;
			ia++;
			}
		}
	}

static void copyPixelsOut(unsigned char *ar, size_t rowNumber, unsigned char *jpg_row, size_t width, size_t height)
	{
	size_t ia = 0;
	int x = 0;
	
	size_t row_width = width * DANA_PIXEL_WIDTH;
	
	ia = row_width * rowNumber;
	
	for(x = 0; x < width; x++)
		{
		unsigned char *px = &(jpg_row[x * 3]);
		
		px[0] = ar[ia];
		ia++;
		px[1] = ar[ia];
		ia++;
		px[2] = ar[ia];
		ia++;
		ia++;
		}
	}

INSTRUCTION_DEF op_load_image(VFrame *cframe)
	{
	unsigned char res = 0;
	
	char *path = getParam_char_array(cframe, 0);
	
	size_t width = 0;
	size_t height = 0;
	unsigned char *bmp_data = read_file(path, &width, &height);
	
	free(path);
	
	if (bmp_data == NULL)
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
	
	size_t len = (width * DANA_PIXEL_WIDTH) * height;
	array -> data = malloc(len);
	array -> length = len;
	
	array -> gtLink = byteArrayGT;
	api -> incrementGTRefCount(array -> gtLink);
	array -> refi.ocm = cframe -> blocking -> instance;
	
	array -> refi.refCount ++;
	array -> refi.type = array -> gtLink -> typeLink;
	
	ict -> content = (unsigned char*) array;
	ict -> typeLink = array -> gtLink -> typeLink;
	
	//populate the pixel map (see above helper function)
	copyPixels(array -> data, bmp_data, width, height);
	
	free(bmp_data);
	
	res = 1;
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	return RETURN_OK;
	}


bool write_jpg_file(unsigned char *pixelMap, size_t width, size_t height, char *filename)
	{
	//int rc;
	
	// Variables for the source jpg
	//struct stat file_info;
	int pixel_size = 3;

	// Variables for the decompressor itself
	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;
	
	int row_stride = width * pixel_size;
	
	unsigned char *row_decode = malloc(row_stride);
	
	FILE * outfile;
	
	//set a custom error handler
	cinfo.err = jpeg_std_error(&jerr.pub);
	
	jerr.pub.error_exit = my_error_exit;
	
	if (setjmp(jerr.setjmp_buffer))
		{
		jpeg_destroy_compress(&cinfo);
		fclose(outfile);
		return NULL;
		}
	
	jpeg_create_compress(&cinfo);
	
	
	if ((outfile = fopen(filename, "wb")) == NULL) {
	    return false;
	}
	jpeg_stdio_dest(&cinfo, outfile);
	

	cinfo.image_width = width; 	/* image width and height, in pixels */
	cinfo.image_height = height;
	cinfo.input_components = 3;	/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; /* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	
	/* Make optional parameter settings here, if any */
	
	jpeg_start_compress(&cinfo, TRUE);
	
	while (cinfo.next_scanline < cinfo.image_height)
		{
		unsigned char *buffer_array[1];
		
		copyPixelsOut(pixelMap, cinfo.next_scanline, row_decode, width, height);
		
		buffer_array[0] = row_decode;

		jpeg_write_scanlines(&cinfo, buffer_array, 1);
		}
	
	jpeg_finish_compress(&cinfo);
	
	free(row_decode);
	
	fclose(outfile);
	
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
	
	res = write_jpg_file(array -> data, width, height, path);
	
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
