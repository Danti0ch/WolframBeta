#ifndef EX_TREE
#define EX_TREE

#include <stdio.h>
#include <stdint.h>
#include "node.h"
#include "log.h"

const int MAX_BUF_SIZE = 1000;

const char LATEH_FILE_NAME[100]            = "tmp.teh";
const char OUTPUT_PDF_NAME[100]            = "output.pdf";

struct Expr{
	
	Node    root;
	// int32_t size;
	// bool    is_saved;
};

//------------PUBLIC-FUNCTIONS-DECLARATION------------------------

void ExprInit(Expr* expr);

void ExprRead(FILE* f_stream, Expr* expr);

void ExprDestr(Expr* expr);

void sub_expr_destr(Expr* expr, Node* nod);

void ExprDif(Expr* expr);

void NodeDif(Node* node);

void ShowExpr(Expr* expr);

void WriteExpr(Expr* expr);

#endif
