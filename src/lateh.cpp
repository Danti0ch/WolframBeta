#include "expression.h"
#include <assert.h>
#include <cstdarg>
#include <cstdlib>

static void write_node_to_teh(Node* node, FILE* output_file);

static bool check_low_priority_operation(Node* node, char source_oper);

static void execute_term_cmd(char const * cmd_str, ...);

//__________________________________________________________________

void LatehInit(){

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
						"\\maketitle\n\n");

	fclose(teh_file);
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

//void WriteDifExpr()

//void WriteMaclorenExpr()
//------------------------------------------------------------------

#define DEF_FUNC(id, name, dif_code, val_code)		\
	case id:										\
		fprintf(output_file, "%s", #name);			\
		break;

static void write_node_to_teh(Node* node, FILE* output_file){

	assert(node        != NULL);
	assert(output_file != NULL);

	if(!IsValid(node)){
		ToLog(LOG_TYPE::ERROR, "Node %p has invalid type", node);
		return;
	}
	else if(IsConstant(node)){
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

void LatehWriteDifExpr(Expr* expr){

	assert(expr != NULL);

	LatehWrite("(");
	LatehWriteExpr(expr);
	LatehWrite(")^\\prime = ");

	Expr dif_expr = {};

	InitExpr(&dif_expr);

	DifExpr(expr, &dif_expr);

	LatehWriteExpr(&dif_expr);

	LatehWrite("\n\n");
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
	execute_term_cmd("%s", OUTPUT_PDF_NAME);

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

    int buflen = _vscprintf(cmd_str, args) + 1;
    char *buffer = (char*)calloc(buflen, sizeof(char));

    if (buffer != NULL){
        vsprintf(buffer, cmd_str, args);
    }

    va_end( args );

    system(buffer);

	free(buffer);

	return;
}
