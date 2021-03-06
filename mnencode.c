#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "mnemonic.h"

int counthex(const mn_byte *s, size_t len) {
	int total = 0;
	size_t i;
	for(i = 0; i < len; i++) {
		if(isxdigit(s[i])) total++;
	}
	return total;
}

int countLeadingCrapAndZeros(const mn_byte *in, size_t inlen) {
	size_t i;
	for(i = 0; i < inlen; i++) {
		if(in[i] != '0' && isxdigit(in[i])) break; /* Find first non-zero hex digit */
	}
	return i;
}

int hex2bytes(int inlen, const mn_byte *in, mn_byte *out) {
	char t[3];
	int i, j = 0, k = 0;
	i = countLeadingCrapAndZeros(in, inlen);
	inlen = inlen - i;
	in = in + i;
	if(counthex(in, inlen) % 2 != 0) {
		t[j++] = '0';
	}
	for(i = 0; i < inlen; i++) {
		if(isxdigit(in[i])) {
			t[j++] = in[i];
			if(j == 2) {
				t[2] = '\0';
				errno = 0;
				out[k++] = strtol(t, NULL, 16);
				if(errno != 0) {
					return -1;
				}
				j = 0;
			}
		}
	}
	return k;
}

int main(int argc, char **argv) {
	mn_byte *buf;
	mn_byte cbuf[0x10000];
	mn_byte xbuf[0x8000];
	char outbuf[0x90000];
	int buflen;
	int n;
	int hex_encoded_input = 0;

	if(argc > 1 && strcmp(argv[1],"-x") == 0) {
		hex_encoded_input = 1;
	}

	fprintf (stderr, "%s\n", mn_wordlist_version);

	buflen = fread(cbuf, 1, sizeof cbuf, stdin);

	if(hex_encoded_input) {
		buflen = hex2bytes(buflen, cbuf, xbuf);
		if(buflen < 0) {
			perror("hex2bytes");
			return EXIT_FAILURE;
		}
		buf = xbuf;
	} else {
		buf = cbuf;
	}

	n = mn_encode(buf, buflen, outbuf, sizeof outbuf, "x x x. x x x. x x x\n");

	if(n != 0) {
		fprintf(stderr, "mn_encode error %d\n", n);
		return EXIT_FAILURE;
	}

	fwrite(outbuf, 1, strlen (outbuf), stdout);
	putchar('\n');

	return EXIT_SUCCESS;
}
