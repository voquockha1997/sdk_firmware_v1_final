/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../3rd.party/include/jpeglib.h" //80
#include "../../../3rd.party/include/png.h"

// #include "../include/jpeglib.h" //90
// #include "../include/png.h"

#include "../include/dbgutil.h"
#include "../include/quirc.h"
// static const char *data_type_str(int dt)
// {
// 	switch (dt) {
// 	case QUIRC_DATA_TYPE_NUMERIC: return "NUMERIC";
// 	case QUIRC_DATA_TYPE_ALPHA:   return "ALPHA";
// 	case QUIRC_DATA_TYPE_BYTE:    return "BYTE";
// 	case QUIRC_DATA_TYPE_KANJI:   return "KANJI";
// 	}

// 	return "unknown";
// }

void dump_data(const struct quirc_data *data)
{
	// printf("    Version: %d\n", data->version);
	// printf("    ECC level: %c\n", "MLHQ"[data->ecc_level]);
	// printf("    Mask: %d\n", data->mask);
	// printf("    Data type: %d (%s)\n", data->data_type, data_type_str(data->data_type));
	// printf("    Length: %d\n", data->payload_len);
	printf("Payload: %s\n", data->payload);

	// if (data->eci)
	// 	printf("    ECI: %d\n", data->eci);
}

struct my_jpeg_error {
	struct jpeg_error_mgr   base;
	jmp_buf                 env;
};

static void my_output_message(struct jpeg_common_struct *com)
{
	struct my_jpeg_error *err = (struct my_jpeg_error *)com->err;
	char buf[JMSG_LENGTH_MAX];

	err->base.format_message(com, buf);
	fprintf(stderr, "JPEG error: %s\n", buf);
}

static void my_error_exit(struct jpeg_common_struct *com)
{
	struct my_jpeg_error *err = (struct my_jpeg_error *)com->err;

	my_output_message(com);
	longjmp(err->env, 0);
}

static struct jpeg_error_mgr *my_error_mgr(struct my_jpeg_error *err)
{
	jpeg_std_error(&err->base);

	err->base.error_exit = my_error_exit;
	err->base.output_message = my_output_message;

	return &err->base;
}

int load_jpeg(struct quirc *q, const char *filename)
{
	FILE *infile = fopen(filename, "rb");
	struct jpeg_decompress_struct dinfo;
	struct my_jpeg_error err;
	uint8_t *image;
	int y;

	if (!infile) {
		perror("can't open input file");
		return -1;
	}

	memset(&dinfo, 0, sizeof(dinfo));
	dinfo.err = my_error_mgr(&err);

	if (setjmp(err.env))
		goto fail;

	jpeg_create_decompress(&dinfo);
	jpeg_stdio_src(&dinfo, infile);

	jpeg_read_header(&dinfo, TRUE);
	dinfo.output_components = 1;
	dinfo.out_color_space = JCS_GRAYSCALE;
	jpeg_start_decompress(&dinfo);

	if (dinfo.output_components != 1) {
		fprintf(stderr, "Unexpected number of output components: %d",
			 dinfo.output_components);
		goto fail;
	}

	if (quirc_resize(q, dinfo.output_width, dinfo.output_height) < 0)
		goto fail;

	image = quirc_begin(q, NULL, NULL);

	for (y = 0; y < dinfo.output_height; y++) {
		JSAMPROW row_pointer = image + y * dinfo.output_width;

		jpeg_read_scanlines(&dinfo, &row_pointer, 1);
	}

	jpeg_finish_decompress(&dinfo);
	fclose(infile);
	jpeg_destroy_decompress(&dinfo);
	return 0;

fail:
	fclose(infile);
	jpeg_destroy_decompress(&dinfo);
	return -1;
}

