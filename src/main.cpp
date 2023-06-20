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
	ReductExpr(&Expression);
	LatehInit(&Expression);

	LatehWriteNDiffs(&Expression);
	//LatehWriteMaclorenExpr(&Expression);
	LatehClose();
	LatehToPdf();

	DestrExpr(&Expression);
	LogClose();
	return 0;
}
