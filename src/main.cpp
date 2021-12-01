#include "stdio.h"
#include "expression.h"

int main(){

	LogInit();

	Expr Expression = {};

	InitExpr(&Expression);

	ReadExpr(stdin, &Expression);

	ShowExpr(&Expression);
	DestrExpr(&Expression);

	LogClose();
	return 0;
}
