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

typedef struct TokenNode {
	Token *toke;
	struct TokenNode *next;
} TokenNode;

// start linked list functions

TokenNode *createNode(Token *toke) {
	TokenNode *node = (TokenNode *)malloc(sizeof(TokenNode));
	
	node->next = NULL;
	if(toke == NULL)
		node->toke = NULL;
	else
		node->toke = toke;

	return node;
}

// Adds node to tail, I dont think we need to add any to the head
TokenNode *addNode(TokenNode *head, Token *toke) {
	TokenNode *temp = head;

	if(head == NULL){
		head = createNode(toke);
		printf("\n"); return head;
	}

	while(temp->next != NULL)
		temp = temp->next;

	temp->next = createNode(toke);
	return head;
}

void printList(TokenNode *head) {
	TokenNode *temp = head;

	printf("Lexeme List:\n");
	while(temp != NULL) {
		if(temp->toke == NULL)
			printf("NullBois ");
		else if(temp->toke->type == identsym)
			printf("%d %s ", temp->toke->type, temp->toke->lexeme);
		else	
			printf("%d ", temp->toke->type);

		temp = temp->next;
	}
	printf("\n");
}

void printTable(TokenNode *head) {
	TokenNode *temp = head;
	int len;

	printf("Lexeme Table:\n");
	printf("Lexeme\t\ttoken type\n");
	while(temp != NULL) {
		len = strlen(temp->toke->lexeme);
		if(len < 8)
			printf("%s\t\t%d\n", temp->toke->lexeme, temp->toke->type);
		else
			printf("%s\t%d\n", temp->toke->lexeme, temp->toke->type);

		temp = temp->next;
	}
}

// end linked list functions

int readFile(char *filename, char *contents) {
	FILE *file = fopen(filename, "r");

	if (file == NULL) return -1;

	int i = 0;
	char c;

	while (!feof(file)) {
		c = fgetc(file);
		if (c == '\t') continue;
		contents[i++] = c;
	}
	contents[i] = '\0';
	fclose(file);
	return i;
}

int isLetter(char c) {
	return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

int isDigit(char c) {
	return ((c >= '0') && (c <= '9'));
}

void error(int e) {
	printf("Error %d", e);
	exit(e);
}

Token *analyzeOther(int *counter, char *contents, Token *toke) {
	toke->lexeme = (char *)malloc(2*sizeof(char));
	switch (contents[*counter]) {
		case '+':
			toke->type = plussym;
			strcpy(toke->lexeme, "+");
			return toke;
		case '-':
			toke->type = minussym;
			strcpy(toke->lexeme, "-");
			return toke;
		case '*':
			toke->type = multsym;
			strcpy(toke->lexeme, "*");
			return toke;
		case '/':
			toke->type = slashsym;
			strcpy(toke->lexeme, "/");
			return toke;
		case '=':
			toke->type = eqsym;
			strcpy(toke->lexeme, "=");
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
			strcpy(toke->lexeme, "(");
			return toke;
		case ')':
			toke->type = rparentsym;
			strcpy(toke->lexeme, ")");
			return toke;
		case ',':
			toke->type = commasym;
			strcpy(toke->lexeme, ",");
			return toke;
		case ';':
			toke->type = semicolonsym;
			strcpy(toke->lexeme, ";");
			return toke;
		case '.':
			toke->type = periodsym;
			strcpy(toke->lexeme, ".");
			return toke;
		case ':':
			toke->type = becomessym;
			free(toke->lexeme);
			toke->lexeme = malloc(3*sizeof(char));
			strcpy(toke->lexeme, ":=");
			*counter += 1;
			return toke;
		case '#':
			return NULL;	// dont know what to do here but passes test
		default:
			printf("ASCII: %i\n", contents[*counter]);
			error(4);
	}

	return toke;
}

Token *analyzeNumber(int *counter, char *contents, Token *toke) {
	int i;
	
	for(i=1; i<5; i++) {
		if (!isDigit(contents[*counter+i])) {
			// if its a letter it should throw an error, most likely error(1)
			if(isLetter(contents[*counter+i]))
				error(1);
			break;
		}
	}
	if (isDigit(contents[*counter+i])) error(2);

	toke->lexeme = (char *)malloc((i+1)*sizeof(char));
	strncpy(toke->lexeme, contents+*counter, i);
	toke->lexeme[i] = '\0';
	*counter += i-1;
	return toke;
}

Token *analyzeIdentifier(int *counter, char *contents, Token *toke) {
	char word[MAX_IDENT+1];
	int i;

	word[0] = contents[*counter];
	for(i=1; i<11; i++) {
		word[i] = contents[*counter+i];
		word[i+1] = '\0';
		if(!isDigit(word[i]) && !isLetter(word[i])){
			word[i] = '\0';
			toke->lexeme = (char *)malloc(strlen(word)*sizeof(char));
			strcpy(toke->lexeme, word);
			*counter = *counter+i-1;
			return toke;
		} else if (i == 1 && strcmp(word, "if") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(3*sizeof(char));
			strcpy(toke->lexeme, "if");
			toke->type = ifsym; 
			*counter = *counter+i;
			return toke;
		} else if (i == 1 && strcmp(word, "do") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(3*sizeof(char));
			strcpy(toke->lexeme, "do");
			toke->type = dosym;
			*counter = *counter+i;
			return toke;
		} else if (i == 2 && strcmp(word, "var") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(4*sizeof(char));
			strcpy(toke->lexeme, "var");
			toke->type = varsym;
			*counter = *counter+i;
			return toke;
		} else if (i == 2 && strcmp(word, "end") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(4*sizeof(char));
			strcpy(toke->lexeme, "end");
			*counter = *counter+i;
			toke->type = endsym;
			return toke;
		} else if (i == 3 && strcmp(word, "call") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(5*sizeof(char));
			strcpy(toke->lexeme, "call");
			toke->type = callsym;
			*counter = *counter+i;
			return toke;
		} else if (i == 3 && strcmp(word, "then") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(5*sizeof(char));
			strcpy(toke->lexeme, "then");
			toke->type = thensym;
			*counter = *counter+i;
			return toke;
		} else if (i == 3 && strcmp(word, "else") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(5*sizeof(char));
			strcpy(toke->lexeme, "else");
			toke->type = elsesym;
			*counter = *counter+i;
			return toke;
		} else if (i == 3 && strcmp(word, "read") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(5*sizeof(char));
			strcpy(toke->lexeme, "read");
			toke->type = readsym;
			*counter = *counter+i;
			return toke;
		} else if (i == 4 && strcmp(word, "const") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(6*sizeof(char));
			strcpy(toke->lexeme, "const");
			toke->type = constsym;
			*counter = *counter+i;
			return toke;
		} else if (i == 4 && strcmp(word, "begin") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(6*sizeof(char));
			strcpy(toke->lexeme, "begin");
			toke->type = beginsym;
			*counter = *counter+i;
			return toke;
		} else if (i == 4 && strcmp(word, "while") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(6*sizeof(char));
			strcpy(toke->lexeme, "while");
			toke->type = whilesym;
			*counter = *counter+i;
			return toke;
		} else if (i == 4 && strcmp(word, "write") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(6*sizeof(char));
			strcpy(toke->lexeme, "write");
			toke->type = writesym;
			*counter = *counter+i;
			return toke;
		} else if (i == 8 && strcmp(word, "procedure") == 0 && contents[*counter+1] == ' ') {
			toke->lexeme = (char *)malloc(10*sizeof(char));
			strcpy(toke->lexeme, "procedure");
			toke->type = procsym;
			*counter = *counter+i;
			return toke;
		}
	}
	if (isLetter(contents[*counter+i]) || isDigit(contents[*counter+i])) error(3);

	if(!isDigit(word[i]) && !isLetter(word[i])){
		word[i] = '\0';
		toke->lexeme = (char *)malloc(strlen(word)*sizeof(char));
		strcpy(toke->lexeme, word);
		*counter = *counter+i;
		return toke;
	}
}

int checkOther(char c) {
	return (c == '\n') || (c == ' ') || (c == 13) || (c == -1);
}


Token *nextToken(int *counter, char *contents) {
	Token *toke = (Token *)malloc(sizeof(Token));
	token_type t;
	char word[MAX_IDENT+1];
	bool comment = false;
	int i;

	if(checkOther(contents[*counter]))
		return NULL;
	if (isLetter(contents[*counter])) {
		toke->type = identsym;
	} else if (isDigit(contents[*counter])) {
		toke->type = numbersym;
	} else {
		toke = analyzeOther(counter, contents, toke);
		if(toke == NULL)
			return toke;
	}

	switch (toke->type) {
		case numbersym:
			toke = analyzeNumber(counter, contents, toke);
			break;

		case identsym:
			toke = analyzeIdentifier(counter, contents, toke);
			break;

		/*
		case slashsym:
			if (contents[*counter+1] == '/') {
				*counter++;
				comment = true;
			} else {
				toke->lexeme = (char *)malloc(2*sizeof(char));
				strcpy(toke->lexeme, "/\0");
			}
			break;
		*/

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
			break;

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
			break;
	}

	return toke;
}

TokenNode *analyze(int fileSize, char *contents) {
	int i, j = 0;
	token_type *tokenTypes = (token_type *)malloc(fileSize*sizeof(token_type));
	char **tokens = (char **)malloc(fileSize*sizeof(char*));
	Token *toke = NULL;
	TokenNode *head = NULL;
	
	for(i=0; i<fileSize; i++) {
		toke = nextToken(&i, contents);
		if(toke != NULL)
			head = addNode(head, toke);

		/*
		if (isLetter(contents[i])) {
			//stuff
		} else if (isDigit(contents[i])) {
			//stuff
		} else {

		}
		*/
	}
	printList(head);
	printTable(head);
}

int main(int argc, char** argsv) {
	char contents[MAX_SIZE] = {};
	int fileSize = readFile(argsv[1], contents);
	analyze(fileSize, contents);
}