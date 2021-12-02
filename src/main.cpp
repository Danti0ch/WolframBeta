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

	ShowExpr(&Expression);
	
	Expr dif_expr = {};
	InitExpr(&dif_expr);

	DifExpr(&Expression, &dif_expr);
	
	ShowExpr(&dif_expr);
	
	DestrExpr(&Expression);

	LogClose();
	return 0;
}
