#include "../../crb_string.h"
#include <stdio.h>

int main(void) {
	struct crb_string str;
	
	if (crb_string_init(&str, 10) != 0) {
		return -1;
	}

	int i = 0;
	while (i < 26 && crb_string_append_char(&str, 'a' + (i++)) == 0) {}
	printf("%s cap %lu %lu\n", str.data, str.capacity, str.length);

	crb_string_append_chars(&str, "01234567890123456789\n");
	printf("%s cap %lu %lu\n", str.data, str.capacity, str.length);

	return 0;
}
