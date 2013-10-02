/*
 * hexDump 0.2 by MetalHead
 * DEC 11 2007
 */
#include <stdio.h>
#include <string.h>

#define SEGMENT 16

void dump(FILE *in, FILE *out)
{
	char buf[SEGMENT];
	int c,i,m = 0;

	while((c = fgetc(in)) != EOF) {
		if(!(m % SEGMENT)) {
			memset(&buf,0,SEGMENT+1);
			fprintf(out," [0x%04X] | ",m);
		}

		fprintf(out,"%02X ",c);

		if(isspace© || !isprint©)
			buf[m % SEGMENT] = '.';
		else
			buf[m % SEGMENT] = c;
		m++;
		if(!(m % SEGMENT))
			fprintf(out,"| %s\n",buf);
	}

	if(m % SEGMENT != 0) {
		for(i = 0; i < (SEGMENT - m % SEGMENT); i++)
			fprintf(out,"   ");
		fprintf(out,"| %s\n",buf);
	}
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		fprintf(stderr,
			"ERROR: Not enough parameters provided.\n"
			"Usage: %s INPUT_FILE [OUTPUT_FILE]\n",
			argv[0]);
		return 1;
	}

	FILE *in, *out;

	in = fopen(argv[1],"rb");
	out = (argc > 2) ? fopen(argv[2],"w") : stdout;

	if(in == NULL || out == NULL) {
		fprintf(stderr,"ERROR: Error opening file.");
		return 1;
	}

	fprintf(out,"\t\t\t- Hex Dump of %s -\n", argv[1]);
	dump(in,out);
	fprintf(out,"\t\t\t- End of file. -\n");

	fclose(in);
	fclose(out);

	return 0;
}
