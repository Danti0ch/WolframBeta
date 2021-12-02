#include "stdio.h"
#include "expression.h"

int main(const int argc, const char* argv[]){

	LogInit();

	Expr Expression = {};

	InitExpr(&Expression);

	// TODO: read args
	FILE* input = NULL;
	if(argc > 1){
		input = fopen(argv[1], "r");
	}
	else{
		input = stdin;
	}

	ReadExpr(input, &Expression);
	ShowExpr(&Expression);

	/*
	ReductExpr(&Expression);
	
	LatehInit();
	LatehWriteDifExpr(&Expression);

	LatehClose();
	LatehToPdf();

	DestrExpr(&Expression);
	LogClose();*/
	return 0;
}
