#include "stdio.h"

#define LOG_SOURCE
#if LOG_SOURCE == 1

#elif LOG_SOURCE == 2

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
