#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <limits.h>
#include <stdbool.h>
#include <getopt.h>
#include <locale.h>

#include "wc.h"

// A simple reimplementation of wc (word count):
// https://man7.org/linux/man-pages/man1/wc.1.html

/////////////
// WIP!!! //
////////////

/* current supported OPTIONS:
	-c, --bytes
              print the byte counts
	
       -m, --chars
              print the character counts

       -l, --lines
              print the newline counts
	
	-L, --max-line-length
              print the maximum display width

       -w, --words
              print the word counts
*/ 

static wc_data sum  = {0, 0, 0, 0, 0};

static struct option long_options[] = {
		{"bytes", no_argument, 0, 'c'},
		{"chars", no_argument, 0, 'm'},
		{"lines", no_argument, 0, 'l'},
		{"max-line-length", no_argument, 0, 'L'},
		{"words", no_argument, 0, 'w'}
	};

static bool ARG_BYTES = false; // -c, --bytes
static bool ARG_CHARS = false; // -m, --chars
static bool ARG_LINES = false; // -l, --lines

static bool ARG_MAX_LINE_LENGTH = false; // -L, --max-line-length
static bool ARG_WORDS = false; // -w, --words

static int _optcount = 0;


int main(int argc, char* argv[]){
	readArgs(argc, argv);
	
	int argStart = 1; // assumindo nenhum opt inserido.
	if(_optcount > 0){
		argStart = 2; // assuma que somente um arg contém todos os opt's
		for(int i = 2; i < argc; ++i){
			if(argv[i][0] == '-')
				argStart++;
			else 
				break;
		}
	}
	
	wc_data fi;
	
	int currentLineSize;
	bool inWord;
	int c;
	
	// qtd. de arquivos recebidos via argumento(s) (caso seja zero, vai ler do stdin):
	const int FILE_COUNT = argc - argStart;
	setlocale(LC_ALL, "");
	
	if(FILE_COUNT > 0){ // ler de arquivos
		mbstate_t state = {0};
		size_t mbLen = 0;
		char mb[MB_LEN_MAX];
		
		int i = optind;
		while(i < argc){
			FILE* f = fopen(argv[i], "rb");
			if(f == NULL){
				printf("%s: %s: No such file or directory", argv[0], argv[i]);
			}
			
			fi = (wc_data) {0, 0, 0, 0, 0};
			currentLineSize = 0;
			inWord = false;
			
			while((c = fgetc(f)) != EOF){
				++fi.bytes;
				if(c == '\r'){
					
				}
				else if(c == '\n'){
					++fi.newLines;
					currentLineSize = 0;
				}
				else{
					++currentLineSize;
				}
				
				mb[mbLen++] = (char)c;

				wchar_t wc;
				size_t n = mbrtowc(&wc, mb, mbLen, &state);
				if (n == (size_t)-2) {
					continue;
				}

				if (n == (size_t)-1) {
					memset(&state, 0, sizeof(state));
					mbLen = 0;
					continue;
				}

				++fi.chars;
					
				if(iswspace(wc)){
					inWord = false;
				}
				else if(!inWord){
					++fi.words;
					inWord = true;
				}
				
				mbLen = 0;
				
				if(currentLineSize > fi.longestLineSize)
					fi.longestLineSize = currentLineSize;
			}
			fclose(f);
			
			writeResults(argc, fi);
			printf("\t%s\n", argv[i]);
			get_total(fi, &sum);
			
			++i;
		}
		if(FILE_COUNT > 1){
			writeResults(argc, sum);
			printf("\ttotal\n");
		}
	}
	else{ // leitura do stdin
		mbstate_t state = {0};
		size_t mbLen = 0;
		char mb[MB_LEN_MAX];
		
		fi = (wc_data) {0, 0, 0, 0, 0};
		currentLineSize = 0;
		inWord = false;
		
		while((c = fgetc(stdin)) != EOF){
			++fi.bytes;
			if(c == '\n'){
				++fi.newLines;
				if(currentLineSize > fi.longestLineSize){
					fi.longestLineSize = currentLineSize;
				}
				currentLineSize = 0;
			}
			else
				++currentLineSize;
			
			mb[mbLen++] = (char)c;

			wchar_t wc;
			size_t n = mbrtowc(&wc, mb, mbLen, &state);

			if (n == (size_t)-2) {
				continue;
			}

			if (n == (size_t)-1) {
				memset(&state, 0, sizeof(state));
				mbLen = 0;
				continue;
			}

			++fi.chars;
			
			if(iswspace(c)){
				inWord = false;
			}
			else if(!inWord){
				++fi.words;
				inWord = true;
			}
			
			if(currentLineSize > fi.longestLineSize)
				fi.longestLineSize = currentLineSize;
		}
	
		writeResults(argc, fi);
	}
	
	return 0;
}

/////////////////////////////////////////////

void readArgs(int argc, char* argv[]){
	int opt;
	while((opt = getopt_long(argc, argv, "cwmlL", long_options, NULL)) != -1){
		switch(opt){
			case 'c':
				ARG_BYTES = true;
				++_optcount;
				break;
			case 'w':
				ARG_WORDS= true;
				++_optcount;
				break;
			case 'm':
				ARG_CHARS = true;
				++_optcount;
				break;
			case 'l':
				ARG_LINES = true;
				++_optcount;
				break;
			case 'L':
				ARG_MAX_LINE_LENGTH = true;
				++_optcount;
				break;
			case '?':
				++_optcount;
				break;
		}
	}
}

bool str_equals(const char* s1, const char* s2){
	int i;
	for(i = 0; s1[i] != '\0' && s2[i] != '\0'; ++i){
		if(s1[i] != s2[i])
			return false;
	}
	return (s1[i] != s2[i])? false : true;
}

void writeResults(int argc, wc_data fi){
	if(_optcount == 0){
		printf("%u\t%u\t%u", fi.newLines, fi.words, fi.bytes);
		return;
	}
	
	if(ARG_LINES)
		printf("\t%u", fi.newLines);
	if(ARG_WORDS)
		printf("\t%u", fi.words);
	if(ARG_CHARS)
		printf("\t%u", fi.chars);
	if(ARG_BYTES)
		printf("\t%u", fi.bytes);
	if(ARG_MAX_LINE_LENGTH)
		printf("\t%u", fi.longestLineSize);
}

void get_total(wc_data source, wc_data* dest){
	dest->newLines += source.newLines;
	dest->chars += source.chars;
	dest->words += source.words;
	if(source.longestLineSize > dest->longestLineSize)
		dest->longestLineSize = source.longestLineSize;
	dest->bytes += source.bytes;
}

