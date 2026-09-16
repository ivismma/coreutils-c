#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>

// A simple reimplementation of wc (word count):
// https://man7.org/linux/man-pages/man1/wc.1.html

// Currently, only reading from standard input - stdin (data should be piped).

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


void readArgs(int argc, char* argv[]);
bool str_equals(const char* s1, const char* s2);


bool ARG_BYTES = false; // -c, --bytes
bool ARG_CHARS = false; // -m, --chars
bool ARG_LINES = false; // -l, --lines

bool ARG_MAX_LINE_LENGTH = false; // -L, --max-line-length
bool ARG_WORDS = false; // -w, --words


int main(int argc, char* argv[]){
	readArgs(argc, argv);
	
	int newLines = 0;
	int chars = 0;
	int words = 0;

	int currentLineSize = 0;
	int longestLineSize = 0;
	
	char c, previous = -1;
	while((c = fgetc(stdin)) != EOF){
		if(c == '\n'){
			++newLines;
			++words;
			if(currentLineSize > longestLineSize){
				longestLineSize = currentLineSize;
				currentLineSize = 0;    
			}
		}
		else if(c == ' ' && previous != c){
			++words;
			++chars;
		}
		else{
			++chars;
		}
		++currentLineSize;
		if(currentLineSize > longestLineSize)
			longestLineSize = currentLineSize;

		previous = c;
	}
	
	int bytes = newLines+chars;
	
	if(words > 0 && previous != ' ') words++;
	
	if(argc > 1){
		if(ARG_LINES)
			printf("\t%d", newLines);
		if(ARG_WORDS)
			printf("\t%d", words);
		if(ARG_CHARS)
			printf("\t%d", chars);
		if(ARG_BYTES)
			printf("\t%d", bytes);
		if(ARG_MAX_LINE_LENGTH)
			printf("\t%d", longestLineSize);
		
		puts("");
	}
	else
		printf("\t%d\t%d\t%d\n", newLines, words, bytes);
	
	return 0;
}

/////////////////////////////////////////////

void readArgs(int argc, char* argv[]){
	static struct option long_options[] = {
		{"bytes", no_argument, 0, 'c'},
		{"words", no_argument, 0, 'w'},
		{"chars", no_argument, 0, 'm'},
		{"lines", no_argument, 0, 'l'},
		{"max-line-length", no_argument, 0, 'L'}
	};

	int opt;
	while((opt = getopt_long(argc, argv, "cwmlL", long_options, NULL)) != -1){
		switch(opt){
			case 'c':
				ARG_BYTES = true;
				break;
			case 'w':
				ARG_WORDS= true;
				break;
			case 'm':
				ARG_CHARS = true;
				break;
			case 'l':
				ARG_LINES = true;
				break;
			case 'L':
				ARG_MAX_LINE_LENGTH = true;
				break;
			case '?':
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

