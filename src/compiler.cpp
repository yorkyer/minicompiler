#include "utils.h"
#include "lexer.h"
#include "symtable.h"
#include "parser.h"
#include "gencode.h"
#include "mips.h"


int main(int argc, char *argv[])
{
	lexer(argc, argv);

	parser();

	gencode();

	mips();

	return 0;
}