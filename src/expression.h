#ifndef EX_TREE
#define EX_TREE

#include <stdio.h>
#include <stdint.h>
#include "node.h"
#include "log.h"

const int MAX_BUF_SIZE = 1000;

const char LATEH_FILE_NAME[100]            = "tmp.teh";
const char OUTPUT_PDF_NAME[100]            = "output.pdf";

/**
 * структура для хранения буфера
 */
struct String{
	char* data;
	unsigned int size;
};

// или стоит сделать одно поле?
struct Expr{
	Node*  root;
};

//------------PUBLIC-FUNCTIONS-DECLARATION------------------------

void InitExpr(Expr* expr);

void ReadExpr(FILE* f_stream, Expr* expr);

void DestrExpr(Expr* expr);

void DifExpr(Expr* expr);

void NodeDif(Node* node);

void ShowExpr(Expr* expr);

void WriteExpr(Expr* expr);

#endif // EX_TREE
