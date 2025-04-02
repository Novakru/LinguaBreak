#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include "front_end/sysy_parser.tab.hh"
#include "include/symbol.h"

extern FILE *yyin;
extern char *yytext;
extern YYSTYPE yylval;
extern int yylex();
extern void dumpTokens(FILE* output, int token, int line_number, char *yytext, YYSTYPE yylval);

int line = 0;

// option table 
#define nr_options 5 
const char *valid_options[] = {"-lexer", "-parser", "-semant", "-llvm", "-target"};

int main(int argc, char** argv) {
	/* argc = 4 : ./bin/SysYc input_file -lexer output_file */ 
    if (argc != 4) return 1;

    FILE* input = fopen(argv[1], "r");
    FILE* output = stdout;
    
	/* option check */
	bool is_valid_option = false;
	for (int i = 0; i < nr_options; i++) {
		if (strcmp(argv[2], valid_options[i]) == 0) {
			is_valid_option = true;
			break;
		}
	}

	if (!output || !input || !is_valid_option) {
		perror("can't open input/output file or input an invalid option.");
		return 1;
	} 
	
	output = fopen(argv[3], "w");
	
	/* lexer */
	int token;
	yyin = input;
	if (strcmp(argv[2], "-lexer") == 0) {
		fprintf(output, "%-10s %-20s %-15s %-10s\n", "line", "token", "type", "value");
		fprintf(output, "------------------------------------------------------\n");
		while ((token = yylex()) != 0) {
        	dumpTokens(output, token, line, yytext, yylval);
	    }
		fclose(input), fclose(output);
		return 0;
	} else {
		while ((token = yylex()) != 0) {}
	}

	/* parser and so on */
	// if (strcmp(argv[2], "-parser") == 0) {
	// 	TODO("-parser compile option is not supported now.");
	// } else if (strcmp(argv[2], "-semant") == 0) {
	// 	TODO("-semant compile option is not supported now.");
	// } else if (strcmp(argv[2], "-llvm") == 0) {
	// 	TODO("-llvm compile option is not supported now.");
	// } else if (strcmp(argv[2], "-target") == 0) {
	// 	TODO("-target compile option is not supported now.");
	// } else {
	// 	perror("invalid compile option.");
	// }

    fclose(input);
    if (output != stdout) fclose(output);
    return 0;
}