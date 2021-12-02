#include "stdio.h"
#include "expression.h"

int main(const int argc, const char* argv[]){

	LogInit();

	Expr Expression = {};

	InitExpr(&Expression);

	FILE* input = NULL;
	if(argc > 1){
		input = fopen(argv[1], "r");
	}
	else{
		input = stdin;
	}
	
	ReadExpr(input, &Expression);

	ShowExpr(&Expression);
	DestrExpr(&Expression);

	LogClose();
	return 0;
}
