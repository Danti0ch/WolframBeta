#include "stdio.h"
#include "expression.h"

int main(){

	LogInit();

	Expr expr = {};
	ExprInit(&expr);

	ExprRead(stdin, &expr);
	ShowExpr(&expr);
	
	WriteExpr(&expr);

	// SaveExpr(&expr); ???
	LogClose();
	return 0;
}
