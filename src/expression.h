#ifndef EX_TREE
#define EX_TREE

#include <stdio.h>
#include <stdint.h>
#include "node.h"
#include "log.h"

const int MAX_BUF_SIZE = 1000;

const char LATEH_FILE_NAME[100] = "tmp.teh";
const char OUTPUT_PDF_NAME[100] = "tmp.pdf";

/**
 * структура для хранения буфера
 */
struct String{
	char* data;
	unsigned int size;
};

struct Expr{
	Node*  root;
};

const int N_MACLORENA = 6;
const int MAX_N_DIFFS = 3;

//------------PUBLIC-FUNCTIONS-DECLARATION------------------------

void InitExpr(Expr* expr);

void ReadExpr(FILE* f_stream, Expr* expr);

void DestrExpr(Expr* expr);

void DifExpr(Expr* expr, Expr* dif_expr, int n);

Node* NodeDif(Node* node);

void ReductExpr(Expr* expr);

void ShowExpr(Expr* expr);

/**
 * возвращает значение выражения expr в точке arg
 */
double GetValueExpr(Expr* expr, double arg);

/**
 * создаёт макет латех документа и записывает туда выражение expr
 */
void LatehInit(Expr* expr);

/**
 * записывает выражение expr в латех документ
 */
void LatehWriteExpr(Expr* expr);

/**
 * записывает диференциал выражения expr в латех документ
 */
void LatehWriteDifExpr(Expr* expr, int n);

void LatehWriteNDiffs(Expr* expr);
/**
 * записывает строку str латех документ
 */
void LatehWrite(const char* str, ...);

/**
 * записывает разложение выражения expr в ряд маклорена латех документ
 */
void LatehWriteMaclorenExpr(Expr* expr);

void LatehClose();
/**
 * закрывает макет латех документа
 */
void LatehToPdf();

#endif // EX_TREE
