#include "utils.h"
#include "lexer.h"
#include "symtable.h"
#include "parser.h"
#include "gencode.h"
#include "mips.h"


int main(int argc, char *argv[])
{
	if (argc >= 2)
		lexer(argv[1]);
	else
		lexer("testfile.txt");

	parser();

	gencode();

	if (argc >= 3) // if need to optimize
		mips(true);
	else
		mips(false);

	return 0;
}