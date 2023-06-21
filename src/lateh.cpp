#include "expression.h"
#include <assert.h>
#include <cstdarg>
#include <cstdlib>
#include <math.h>

static void write_node_to_teh(Node* node, FILE* output_file);

static bool check_low_priority_operation(Node* node, char source_oper);

static void execute_term_cmd(char const * cmd_str, ...);

static int factorial(int a);
//__________________________________________________________________

void LatehInit(Expr* expr){

	FILE* teh_file = fopen(LATEH_FILE_NAME, "w");
	
	if(teh_file == NULL){
		ToLog(LOG_TYPE::WARNING, "couldn't open %s", LATEH_FILE_NAME);
		return;
	}

	fprintf(teh_file, "\\documentclass{article}\n"
						"\\usepackage[utf8]{inputenc}\n"
						"\\usepackage[russian]{babel}\n"
						"\\usepackage{fullpage}\n"
						"\\title{WolframBeta}\n"
						"\\begin{document}\n"
						"\\maketitle\n"
						"\\section{Simplified expression}\n\n"
						"f(x) = ");
	fclose(teh_file);

	LatehWriteExpr(expr);
	
	return;
}
//__________________________________________________________________

void LatehWriteExpr(Expr* expr){

	assert(expr != NULL);
	
	FILE* teh_file = fopen(LATEH_FILE_NAME, "a");

	fprintf(teh_file, "$");
	write_node_to_teh(expr->root->left, teh_file);
	fprintf(teh_file, "$");

	fclose(teh_file);
	return;
}
//__________________________________________________________________


void LatehWriteNDiffs(Expr* expr){

	assert(expr != NULL);

	LatehWrite("\\section{Multiple derivatives}\n\n");

	for(int n_diff = 1; n_diff < MAX_N_DIFFS; n_diff++){
		LatehWriteDifExpr(expr, n_diff);
	}

	return;
}
//__________________________________________________________________

void LatehWriteMaclorenExpr(Expr* expr){

	LatehWrite("\\section{Разложим в ряд Маклорена}\\\n");

	LatehWriteExpr(expr);

	LatehWrite(" = ");

	Expr dif_expr = {};

	double koef = GetValueExpr(expr, 0);

	if(!IsEqualConst(koef, 0.0))
		LatehWrite("%.3g ", koef);
	else
		LatehWrite("0 ");

	for(int n_diff = 1; n_diff <= N_MACLORENA; n_diff++){

		InitExpr(&dif_expr);
		DifExpr(expr, &dif_expr, n_diff);

		koef = GetValueExpr(&dif_expr, 0) / factorial(n_diff);

		if(!IsEqualConst(koef, 0.0)){
			if(IsEqualConst(koef, 1.0))
				LatehWrite(" + x^{%d}", n_diff);
			else{
				if(fabs(koef) < 1000)
					LatehWrite(" + %.3g \\cdot x^{%d}", koef, n_diff);
				else
					LatehWrite(" + (%.3g) \\cdot x^{%d}", koef, n_diff);				
			}
		}

		DestrExpr(&dif_expr);
	}

	LatehWrite(" + o(x^{%d})\n\n\n", N_MACLORENA);
	return;
}
//__________________________________________________________________

#define DEF_FUNC(id, name, dif_code, val_code)		\
	case id:										\
		fprintf(output_file, " \\%s", #name);			\
		break;

static void write_node_to_teh(Node* node, FILE* output_file){

	assert(node        != NULL);
	assert(output_file != NULL);

	if(!IsValid(node)){
		ToLog(LOG_TYPE::ERROR, "Node %p has invalid type", node);
		return;
	}
	else if(IsConstant(node)){
		if(node->value.const_val <= -EPS || node->value.const_val >= 10000)
			fprintf(output_file, "(%.4g)", node->value.const_val);
		else
			fprintf(output_file, "%.4g", node->value.const_val);

	}
	else if(IsVariable(node)){
		fprintf(output_file, "%c", node->value.symb);
	}
	else if(IsOperation(node)){

		if(node->value.symb == '/') fprintf(output_file, "\\frac");

		if(IsValid(node->left)){

			bool brackets_needed = false;
			if(check_low_priority_operation(node->left, node->value.symb)){
				brackets_needed = true;
			}

			if(brackets_needed) fprintf(output_file, "{(");
			else 				fprintf(output_file, "{");

			write_node_to_teh(node->left, output_file);

			if(brackets_needed) fprintf(output_file, ")}");
			else 				fprintf(output_file, "}");
		}

		switch(node->value.symb){
			case '+':
				fprintf(output_file, " + ");
				break;
			case '-':
				fprintf(output_file, " - ");
				break;
			case '*':	
				fprintf(output_file, " \\cdot ");
				break;
			case '^':
				fprintf(output_file, "^");
				break;
			default:
				ToLog(LOG_TYPE::ERROR, "Node %p has invalid value", node->value.symb);
				break;
		}

		if(IsValid(node->right)){

			bool brackets_needed = false;
			if(check_low_priority_operation(node->right, node->value.symb)){
				brackets_needed = true;
			}

			if(brackets_needed) fprintf(output_file, "{(");
			else 				fprintf(output_file, "{");
			
			write_node_to_teh(node->right, output_file);

			if(brackets_needed) fprintf(output_file, ")}");
			else 				fprintf(output_file, "}");
		}
	}
	else if(IsFunction(node)){

		switch(node->value.func_id){

			// КОДОГЕНЕРАЦИЯ
			#include "func_definitions.h"
			// КОДОГЕНЕРАЦИЯ

			default:
				ToLog(LOG_TYPE::ERROR, "Wrong func name Node %p", node);
				break;
		}

		fprintf(output_file, "{(");
		if(IsValid(node->left)) write_node_to_teh(node->left, output_file);
		fprintf(output_file, ")}");
	}
	return;
}

#undef DEF_FUNC
//__________________________________________________________________

void LatehWriteDifExpr(Expr* expr, int n){

	assert(expr != NULL);

	LatehWrite("(");
	LatehWriteExpr(expr);
	LatehWrite("$)^{");

	for(int i = 0; i < n; i++){
		LatehWrite("\\prime");
	}
	LatehWrite("}$ = ");

	Expr dif_expr = {};
	InitExpr(&dif_expr);

	DifExpr(expr, &dif_expr, n);

	LatehWriteExpr(&dif_expr);

	DestrExpr(&dif_expr);
	LatehWrite("\\\\");

	return;
}
//__________________________________________________________________

void LatehWrite(const char* str, ...){

	assert(str      != NULL);

	FILE* teh_file = fopen(LATEH_FILE_NAME, "a");

	va_list args;
	va_start(args, str);
	
	vfprintf(teh_file, str, args);

	va_end(args);

	fclose(teh_file);

	return;
}
//__________________________________________________________________

void LatehClose(){

	FILE* teh_file = fopen(LATEH_FILE_NAME, "a");

	fprintf(teh_file, "\n\n\\end{document}\n");

	fclose(teh_file);
	return;
}
//__________________________________________________________________

void LatehToPdf(){

	execute_term_cmd("pdflatex -interaction=batchmode %s", LATEH_FILE_NAME);
	// execute_term_cmd("%s", OUTPUT_PDF_NAME);

	return;
}

//__________________________________________________________________

static bool check_low_priority_operation(Node* node, char source_oper){

	assert(node != NULL);

	if(IsOperation(node)){

		if(source_oper == '+' || source_oper == '-'){
			return false;
		}
		else if(source_oper == '*' || source_oper == '/'){
			if(node->value.symb == '+' || node->value.symb == '-') return true;
			else return false;
		}
		else if(source_oper == '^'){
			if(node->value.symb == '^') return false;
			else return true;
		}
		return true;
	}

	return false;
}


static void execute_term_cmd(char const * cmd_str, ...){

	assert(cmd_str != NULL);

	va_list args;
    va_start(args, cmd_str);

    int buflen = 500; // _vscprintf(cmd_str, args) + 1;
    char *buffer = (char*)calloc(buflen, sizeof(char));

    if (buffer != NULL){
        vsprintf(buffer, cmd_str, args);
    }

    va_end( args );

    system(buffer);

	free(buffer);

	return;
}

static int factorial(int a){

	int res = 1;

	while(a > 0){
		res *= a;
		a--;
	}
	return res;
}
