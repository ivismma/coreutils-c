#pragma once

typedef struct{
	unsigned int newLines;
	unsigned int chars;
	unsigned int words;
	unsigned int longestLineSize;
	unsigned int bytes;
} wc_data;

void readArgs(int argc, char* argv[]);
bool str_equals(const char* s1, const char* s2);

void writeResults(int argc, wc_data fi);
void get_total(wc_data source, wc_data* dest);
