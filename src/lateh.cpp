#include "expression.h"

void write_node_to_teh(Node* node, FILE* output_file);

bool check_presence_of_var(Node* node);

bool check_low_priority_operation(Node* node, char source_oper);

bool node_reduction(Node* node);

bool mul_reduction(Node* node);

bool sum_reduction(Node* node);

bool pow_reduction(Node* node);


void WriteExpr(Expr* expr){

	assert(expr != NULL);
	
	Expr original_expr = {};
	ExprInit(&original_expr);
	
	CopyNode(&(original_expr.root), expr->root.left, NODE_PLACE::LEFT);

	Expr* diffur = expr;
	ExprDif(diffur);

	FILE* lateh_file = fopen(LATEH_FILE_NAME, "w");

	fprintf(lateh_file, "\\documentclass{article}\n"
						"\\usepackage[utf8]{inputenc}\n"
						"\\usepackage[russian]{babel}\n"
						"\\usepackage{fullpage}\n"
						"\\title{Differentiator}\n"
						"\\begin{document}\n"
						"\\maketitle\n");

	fprintf(lateh_file, "$(");
	write_node_to_teh(original_expr.root.left, lateh_file);
	fprintf(lateh_file, ")^\\prime = \n");

	write_node_to_teh(diffur->root.left, lateh_file);

	fprintf(lateh_file, "$\n");
	fprintf(lateh_file, "\\end{document}\n");

	fclose(lateh_file);

	system("pdflatex tmp.teh");
	system("tmp.pdf");
	return;
}

//------------------------------------------------------------------
// check it
void write_node_to_teh(Node* node, FILE* output_file){

	assert(node != NULL);

	if(IsOperation(node) && node->value == '/'){
		fprintf(output_file, "\\frac{");
		if(node->left != NULL) write_node_to_teh(node->left, output_file);
		
		fprintf(output_file, "}{");
		if(node->right != NULL) write_node_to_teh(node->right, output_file);

		fprintf(output_file, "}");
		return;
	}
	
	if(IsFunction(node)){
		fprintf(output_file, "%c(", (char)node->value);

		if(node->left != NULL) write_node_to_teh(node->left, output_file);

		fprintf(output_file, ")");

		return;
	}

	if(node->left != NULL){

		bool brackets_needed = (IsOperation(node) && check_low_priority_operation(node->left, node->value));
		if(brackets_needed){
			fprintf(output_file, "(");
		}

		write_node_to_teh(node->left, output_file);

		if(brackets_needed){
			fprintf(output_file, ")");
		}
	}

	if(IsOperation(node) && node->value == '^'){
		fprintf(output_file, "^{");
		if(node->right != NULL) write_node_to_teh(node->right, output_file);
		fprintf(output_file, "}");
		return;
	}

	if(node->type == NODE_TYPE::INVALID){
		ToLog(LOG_TYPE::ERROR, "Node %p has invalid type", node);
		return;
	}

	else if(IsVariable(node)){
		fprintf(output_file, "%c", (char)node->value);
	}

	else if(IsOperation(node)){

		if(node->value == '*'){
			fprintf(output_file, " \\cdot ");
		}
		else{
			fprintf(output_file, "%c", (char)node->value);
		}
	}

	else if(IsConstant(node)){
		if(node->value < 0 && !(IsOperation(node->parent) && node->parent->value == '+')){
			fprintf(output_file, "(%.5lg)", node->value);
		}
		else{
			fprintf(output_file, "%.5lg", node->value);
		}
	}
	else{
		ToLog(LOG_TYPE::ERROR, "The type of Node %p is unreacheble", node);
		return;
	}

	if(node->right != NULL){

		bool brackets_needed = (IsOperation(node) && check_low_priority_operation(node->right, node->value));

		if(brackets_needed){
			fprintf(output_file, "(");
		}

		write_node_to_teh(node->right, output_file);

		if(brackets_needed){
			fprintf(output_file, ")");
		}
	}
	
	return;
}