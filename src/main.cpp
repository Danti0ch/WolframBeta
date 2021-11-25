#include "stdio.h"
#include "expression.h"

int main(){

	Expr expr = {};
	ExprInit(&expr);
	ExprRead(stdin, &expr);
	
	ExprDif(expr.root.left);
	ShowExpr(&expr);
	// SaveExpr(&expr); ???

	return 0;
}
