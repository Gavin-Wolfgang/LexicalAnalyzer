#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 5000
#define MAX_IDENT 11

typedef enum {
	nulsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym, 
	oddsym, eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, 
	rparentsym, commasym, semicolonsym,periodsym, becomessym, beginsym, endsym,
	ifsym, thensym, whilesym, dosym, callsym, constsym, varsym, procsym, 
	writesym, readsym, elsesym
} token_type;

typedef struct Token {
	token_type type;
	char *lexeme;
} Token;

int readFile(char *filename, char *contents) {
	FILE *file = fopen(filename, "r");

	if (file == NULL) return -1;

	int i = 0;
	char c;

	while (!feof(file)) {
		c = fgetc(file);
		if (c == '\t' || c == ' ') continue;
		contents[i++] = c;
	}

	close(file);
	return i;
}

bool isLetter(char c) {
	return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

bool isDigit(char c) {
	return ((c >= '0') && (c <= '9'));
}

void error(int e) {
	printf("Error %d", e);
}

Token *nextToken(int *counter, char *contents) {
	Token *toke = (Token *)malloc(sizeof(Token));
	token_type t;
	char word[MAX_IDENT+1];
	bool comment = false;
	int i;

	if (isLetter(contents[*counter])) {
		toke->type = identsym;
	} else if (isDigit(contents[*counter])) {
		toke->type = numbersym;
	} else {
		switch (contents[*counter]) {
			toke->lexeme = (char *)malloc(2*sizeof(char));
			case '+':
				toke->type = plussym;
				strcpy(toke->lexeme, "+\0");
				return toke;
			case '-':
				toke->type = minussym;
				strcpy(toke->lexeme, "-\0");
				return toke;
			case '*':
				toke->type = multsym;
				strcpy(toke->lexeme, "*\0");
				return toke;
			case '/':
				toke->type = slashsym;
				free(toke->lexeme);
				return toke;
			case '=':
				toke->type = eqlsym;
				strcpy(toke->lexeme, "=\0");
				return toke;
			case '<':
				toke->type = lessym;
				free(toke->lexeme);
				return toke;
			case '>':
				toke->type = gtrsym;
				free(toke->lexeme);
				return toke;
			case '(':
				toke->type = lparentsym;
				strcpy(toke->lexeme, "(\0");
				return toke;
			case ')':
				toke->type = rparentsym;
				strcpy(toke->lexeme, ")\0");
				return toke;
			case ',':
				toke->type = commasym;
				strcpy(toke->lexeme, ",\0");
				return toke;
			case ';':
				toke->type = semicolonsym;
				strcpy(toke->lexeme, ";\0");
				return toke;
			case '.':
				toke->type = periodsym;
				strcpy(toke->lexeme, ".\0");
				return toke;
			case ':':
				toke->type = becomessym;
				strcpy(toke->lexeme, ".\0");
				return toke;
			default:
				error(4);
	}

	switch (toke->type) {
		case numbersym:
			for(i=1; i<5; i++) {
				if (!isDigit(contents[*counter+i])) break;
			}
			if (isDigit(contents[*counter+i])) error(2);

			toke->lexeme = (char *)malloc((i+1)*sizeof(char));
			strncpy(toke->lexeme, contents+*counter, i);
			toke->lexeme[i] = '\0';
			*counter += i-1;

		case identsym:
			word[0] = contents[*counter];
			for(i=1; i<11; i++) {
				word[i] = contents[*counter+i];
				word[i+1] = '\0';
				if (!isLetter(word[i]) && !isDigit(word[i])) { 
					break;
				} else if (i == 1 && strcmp(word, "if") == 0) {
					toke->lexeme = (char *)malloc(3*sizeof(char));
					strcpy(toke->lexeme, "if");
					toke->type = ifsym; 
					return toke;
				} else if (i == 1 && strcmp(word, "do") == 0) {
					toke->lexeme = (char *)malloc(3*sizeof(char));
					strcpy(toke->lexeme, "do");
					toke->type = dosym;
					return toke;
				} else if (i == 2 && strcmp(word, "var") == 0) {
					toke->type = varsym;
					break;
				} else if (i == 2 && strcmp(word, "end") == 0) {
					toke->type = endsym;
					break;
				} else if (i == 3 && strcmp(word, "call") == 0) {
					toke->type = callsym;
					break;
				} else if (i == 3 && strcmp(word, "then") == 0) {
					toke->type = thensym;
					break;
				} else if (i == 3 && strcmp(word, "else") == 0) {
					toke->type = elsesym;
					break;
				} else if (i == 3 && strcmp(word, "read") == 0) {
					toke->type = readsym;
					break;
				} else if (i == 4 && strcmp(word, "const") == 0) {
					toke->type = dosym;
					break;
				} else if (i == 4 && strcmp(word, "begin") == 0) {
					toke->type = beginsym;
					break;
				} else if (i == 4 && strcmp(word, "while") == 0) {
					toke->type = whilesym;
					break;
				} else if (i == 4 && strcmp(word, "write") == 0) {
					toke->type = writesym;
					break;
				} else if (i == 8 && strcmp(word, "procedure") == 0) {
					toke->type = procsym;
					break;
				}
			}



		case slashsym:
			if (contents[*counter+1] == '/') {
				*counter++;
				comment = true;
			} else {
				toke->lexeme = (char *)malloc(2*sizeof(char));
				strcpy(toke->lexeme, "/\0");
			}

		case lessym:
			if (contents[++*counter] == '>') {
				toke->type = neqsym;
				toke->lexeme = (char *)malloc(3*sizeof(char));
				strcpy(toke->lexeme, "<>\0");
			} else if (contents[*counter] == '=') {
				toke->type = leqsym;
				toke->lexeme = (char *)malloc(3*sizeof(char));
				strcpy(toke->lexeme, "<=\0");
			} else {
				*counter--;
				toke->lexeme = (char *)malloc(2*sizeof(char));
				strcpy(toke->lexeme, "<\0");
			}

		case gtrsym:
			if (contents[++*counter] == '=') {
				toke->type = geqsym;
				toke->lexeme = (char *)malloc(3*sizeof(char));
				strcpy(toke->lexeme, ">=\0");
			} else {
				*counter--;
				toke->lexeme = (char *)malloc(2*sizeof(char));
				strcpy(toke->lexeme, ">\0");
			}
	}
}

void analyze(int fileSize, char *contents) {
	int i, j = 0;
	token_type *tokenTypes = (token_type *)malloc(fileSize*sizeof(token_type));
	char **tokens = (char **)malloc(fileSize*sizeof(char*));

	for(i=0; i<fileSize; i++) {
		if (isLetter(contents[i])) {
			//stuff
		} else if (isDigit(contents[i])) {
			//stuff
		} else {

		}
	}
}

int main(int argc, char** argsv) {
	char contents[MAX_SIZE] = {};
	int fileSize = readFile(argsv[1], contents);
}