#include <stdio.h>
#include <string.h>
#include "../include/quirc_internal.h"
#include "../include/dbgutil.h"

static void dump_info(struct quirc *q)
{
	int count = quirc_count(q);
	int i;

	printf("%d QR-codes found:\n\n", count);
	for (i = 0; i < count; i++) {
		struct quirc_code code;
		struct quirc_data data;
		quirc_decode_error_t err;

		quirc_extract(q, i, &code);
		err = quirc_decode(&code, &data);

		//printf("\n");

		if (err) {
			printf("  Decoding FAILED: %s\n", quirc_strerror(err));
		} else {
			printf("Decoding successful:\n");
			dump_data(&data);
		}

		printf("\n");
	}
}

struct quirc *q;
void scan(){
	q = quirc_new();
	if (!q) {
		perror("can't create quirc object");
		return;
	}
	int status = -1;
	status = load_jpeg(q, "../../register/src/PTGP.jpeg");
	if (status < 0) {
		quirc_destroy(q);
		return;
	}
	quirc_end(q);
}
int main()
{
	
	scan();
	dump_info(q);
	quirc_destroy(q);
	return 0;
}
