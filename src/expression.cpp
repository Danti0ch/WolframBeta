#include "expression.h"
#include <assert.h>
#include <stdlib.h>
#include "dls_functions.h"

//---------------LOCAL-FUNCTIONS-DECLARATION-----------------------------------------

void create_buf(char** buffer, FILE* fp);

void destroy_buf(char* buffer);

bool is_operation(char c);

bool is_variable(char c);

int get_func_code(char* name);

void sub_expr_destr(Node* nod);

void write_node_to_teh(Node* node, FILE* output_file);

bool check_presence_of_var(Node* node);

bool check_low_priority_operation(Node* node, char source_oper);

bool is_null(Node* node);

bool node_reduction(Node* node);

bool mul_reduction(Node* node);

bool sum_reduction(Node* node);

bool pow_reduction(Node* node);

//--------------PUBLIC-FUNCTIONS-DEFINITIONS-----------------------------------------

void ExprInit(Expr* expr){

	assert(expr != NULL);

	expr->root.left   = NULL;
	expr->root.right  = NULL;
	expr->root.parent = NULL;
	expr->root.type   = INVALID;

	// expr->size       = 0;
	// expr->is_saved   = false;

	ToLog(LOG_TYPE::INFO, "expression %p was initiated", expr);
	return;
}

//------------------------------------------------------------------

void ExprRead(FILE* f_stream, Expr* expr){

	assert(expr     != NULL);
	assert(f_stream != NULL);

	char *buffer = NULL;

	create_buf(&buffer, f_stream);

	uint32_t buf_size = strlen(buffer);

	int    n_cur_symb    = 0;
	int    n_readen      = 0;

	T_Node value = 0;
	char   symb  = '0';
	char function_name[MAX_FUNC_NAME] = "";

	Node*  cur_nod = &(expr->root);

	while(n_cur_symb < buf_size){

		sscanf((char*)buffer + n_cur_symb, " %n", &n_readen);

		n_cur_symb += n_readen;

		if(n_cur_symb >= buf_size) break;

		if(buffer[n_cur_symb] == '('){
			if(cur_nod->left == NULL){
				cur_nod = NodeConstructor(cur_nod, VAL_POISON, INVALID, NODE_PLACE::LEFT);
			}

			else{
				cur_nod = NodeConstructor(cur_nod, VAL_POISON, INVALID, NODE_PLACE::RIGHT);
			}

			n_cur_symb++;
			continue;
		}

		if(buffer[n_cur_symb] == ')'){

			cur_nod = cur_nod->parent;

			n_cur_symb++;
			continue;
		}

		if(sscanf((char*)buffer + n_cur_symb, "%lg%n", &value, &n_readen) == 1){

			cur_nod->value = value;
			cur_nod->type  = CONSTANT;

			n_cur_symb += n_readen;
			continue;
		}
		/*
		if(sscanf((char*)buffer + n_cur_symb, "%[^(]%n", function_name, &n_readen) == 1){

			int func_code = get_func_code(function_name);

			if(func_code >= 0){
				cur_nod->value = func_code;
				cur_nod->type  = FUNCTION;
				n_cur_symb     += n_readen;
				continue;
			}
		}
		*/
		if(sscanf((char*)buffer + n_cur_symb, "%c%n", &symb) == 1){

			if(is_operation(symb)){
				cur_nod->value = (int)symb;
				cur_nod->type  = OPERATION;

				n_cur_symb += n_readen;
				continue;
			}

			if(is_variable(symb)){
				cur_nod->value = (int)symb;
				cur_nod->type  = VARIABLE;

				n_cur_symb += n_readen;
				continue;
			}

			if(is_function(symb)){
				cur_nod->value = (int)symb;
				cur_nod->type  = FUNCTION;

				n_cur_symb += n_readen;
				continue;
			}
		}

		ExprDestr(expr);

		// err???
		return;
	}
}

//------------------------------------------------------------------

void ExprDif(Expr* expr){

	assert(expr != NULL);

	while(node_reduction(expr->root.left));

	NodeDif(expr->root.left);

	while(node_reduction(expr->root.left));
	return;
}

//------------------------------------------------------------------

void NodeDif(Node* node){

	assert(node != NULL);
	if(node->type == INVALID) return;

	if(check_presence_of_var(node) == false){

		RemoveDescendants(node);
		node->type  = CONSTANT;
		node->value = 0;
		return;
	}

	if(node->type == VARIABLE){
		node->type  = CONSTANT;
		node->value = 1;
		return;
	}

	if(node->type == OPERATION){
		if(node->value == '+' || node->value == '-'){
			NodeDif(node->left);
			NodeDif(node->right);
			return;
		}
		
		if(node->value == '*'){
			if(is_null(node->left) || is_null(node->right)){
				MakeZeroNode(node);
				return;
			}
			node->value = '+';
			
			Node* mul1 = node->left;
			Node* mul2 = node->right;
			
			MAKE_MUL(node, mul1, mul2, NODE_PLACE::LEFT);
			MAKE_MUL(node, mul1, mul2, NODE_PLACE::RIGHT);

			mul1->parent = NULL;
			mul2->parent = NULL;

			NodeDestructor(mul1);
			NodeDestructor(mul2);

			NodeDif(node->left->left);
			NodeDif(node->right->right);
			
			return;
		}

		if(node->value == '/'){

			if(is_null(node->left)){
				MakeZeroNode(node);
				return;
			}

			Node* divisible = node->left;
			Node* divider = node->right;

			NodeConstructor(node, '-', OPERATION, NODE_PLACE::LEFT);

			MAKE_MUL(node->left, divisible, divider, NODE_PLACE::LEFT);

			MAKE_MUL(node->left, divisible, divider, NODE_PLACE::RIGHT);

			MAKE_MUL(node, divider, divider, NODE_PLACE::RIGHT);

			divisible->parent = NULL;
			divider->parent   = NULL;

			NodeDestructor(divisible);
			NodeDestructor(divider);
			
			NodeDif(node->left->left->left);
			NodeDif(node->left->right->right);

			return;
		}

		if(node->value == '^'){

			Node* foundation_node = node->left;
			Node* degree_node     = node->right;

			if(degree_node->value == 0 || foundation_node->value == 0){
				RemoveDescendants(node);

				node->type = CONSTANT;
				node->value = 0;
				return;
			}

			if(check_presence_of_var(degree_node) == false){

				if(check_presence_of_var(foundation_node) == false){
					MakeZeroNode(node);
					return;
				}
				else{

					node->value = '*';

					NodeConstructor(node, '*', OPERATION, NODE_PLACE::LEFT);

					CopyNode(node->left, degree_node, NODE_PLACE::LEFT);
					CopyNode(node->left, foundation_node, NODE_PLACE::RIGHT);
					NodeDif(node->left->right);

					NodeConstructor(node, '^', OPERATION, NODE_PLACE::RIGHT);

					CopyNode(node->right, foundation_node, NODE_PLACE::LEFT);

					NodeConstructor(node->right, '-', OPERATION, NODE_PLACE::RIGHT);
					CopyNode(node->right->right, degree_node, NODE_PLACE::LEFT);

					NodeConstructor(node->right->right, 1, CONSTANT, NODE_PLACE::RIGHT);

					foundation_node->parent = NULL;
					degree_node->parent     = NULL;

					NodeDestructor(foundation_node);
					NodeDestructor(degree_node);

					return;
				}
			}
			else{
				// make (a^b)' = a^b * (b' * ln(a) + b * a' / a)
			}
		}

	}
	if(node->type == FUNCTION){
		Node* arg = node->left;

		if(node->value == (int)FUNC_CODES::LN){

			node->value = '*';
			node->type  = OPERATION;

			NodeConstructor(node, '/', OPERATION, NODE_PLACE::LEFT);
			NodeConstructor(node->left, 1, CONSTANT, NODE_PLACE::LEFT);
			CopyNode(node->left, arg, NODE_PLACE::RIGHT);

			CopyNode(node, arg, NODE_PLACE::RIGHT);
			NodeDif(node->right);
		}
		else if(node->value == (int)FUNC_CODES::SIN){

			node->value = '*';
			node->type  = OPERATION;

			NodeConstructor(node, (int)FUNC_CODES::COS, FUNCTION, NODE_PLACE::LEFT);
			CopyNode(node->left, arg, NODE_PLACE::LEFT);

			CopyNode(node, arg, NODE_PLACE::RIGHT);
			NodeDif(node->right);
		}
		else if(node->value == (int)FUNC_CODES::COS){

			node->value = '*';
			node->type  = OPERATION;

			NodeConstructor(node, '-', OPERATION, NODE_PLACE::LEFT);
			NodeConstructor(node->left, 0, CONSTANT, NODE_PLACE::LEFT);

			NodeConstructor(node->left, (int)FUNC_CODES::SIN, FUNCTION, NODE_PLACE::RIGHT);
			CopyNode(node->left->right, arg, NODE_PLACE::LEFT);

			CopyNode(node, arg, NODE_PLACE::RIGHT);
			NodeDif(node->right);
		}
		arg->parent = NULL;
		NodeDestructor(arg);
		return;
	}
}

//------------------------------------------------------------------

void ExprDestr(Expr* expr){

	assert(expr != NULL);

	sub_expr_destr(&(expr->root));

	return;
}
//------------------------------------------------------------------

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
						"\\title{Diffirintial}\n"
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

void write_node_to_teh(Node* node, FILE* output_file){

	assert(node != NULL);

	if(node->type == OPERATION && node->value == '/'){
		fprintf(output_file, "\\frac{");
		if(node->left != NULL) write_node_to_teh(node->left, output_file);
		
		fprintf(output_file, "}{");
		if(node->right != NULL) write_node_to_teh(node->right, output_file);

		fprintf(output_file, "}");
		return;
	}
	
	if(node->type == FUNCTION){
		fprintf(output_file, "%s(", func_names[(int)node->value]);

		if(node->left != NULL) write_node_to_teh(node->left, output_file);

		fprintf(output_file, ")");

		return;
	}

	if(node->left != NULL){

		bool brackets_needed = (node->type == OPERATION && check_low_priority_operation(node->left, node->value));
		if(brackets_needed){
			fprintf(output_file, "(");
		}

		write_node_to_teh(node->left, output_file);

		if(brackets_needed){
			fprintf(output_file, ")");
		}
	}
	
	if(node->type == OPERATION && node->value == '^'){
		fprintf(output_file, "^{");
		if(node->right != NULL) write_node_to_teh(node->right, output_file);
		fprintf(output_file, "}");
		return;
	}

	if(node->type == INVALID) return;

	else if(node->type == VARIABLE){
		fprintf(output_file, "%c", (char)node->value);
	}

	else if(node->type == OPERATION){

		if(node->value == '*'){
			fprintf(output_file, " \\cdot ");
		}
		else{
			fprintf(output_file, "%c", (char)node->value);
		}
	}
	else if(node->type == CONSTANT){
		if(node->value < 0 && !(node->parent->type == OPERATION && node->parent->value == '+')){
			fprintf(output_file, "(%.5lg)", node->value);
		}
		else{
			fprintf(output_file, "%.5lg", node->value);
		}
	}
	else{
		//logs
	}

	if(node->right != NULL){

		bool brackets_needed = (node->type == OPERATION && check_low_priority_operation(node->right, node->value));
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
//------------------------------------------------------------------

void sub_expr_destr(Node* nod){

	assert(nod != NULL);

	if(nod->left != NULL) sub_expr_destr(nod->left);

	if(nod->right != NULL) sub_expr_destr(nod->right);

	NodeDestructor(nod);

	return;
}

//------------------------------------------------------------------

void ShowExpr(Expr* expr){

	assert(expr != NULL);

	ShowNode(expr->root.left);

	return;
}

//--------------LOCAL-FUNCTIONS-DEFINITIONS-----------------------------------------

void create_buf(char** buffer, FILE* fp){

	assert(buffer != NULL);
	assert(fp     != NULL);

	*buffer = (char*)calloc(MAX_BUF_SIZE, sizeof(char));

	size_t n_readen = 0;

	getline(buffer, &n_readen, fp);
	// обработка ошибок обязательно

	if(ferror(fp)){
		perror("ERROR: ");
	}

	return;
}

//------------------------------------------------------------------

bool node_reduction(Node* node){

	assert(node != NULL);

	bool is_changed = false;
	
	is_changed |= mul_reduction(node);
	is_changed |= sum_reduction(node);
	is_changed |= pow_reduction(node);
	
	if(node->left  != NULL){
		is_changed |= node_reduction(node->left);
	}

	if(node->right != NULL){
		is_changed |= node_reduction(node->right);
	}

	return is_changed;
}

//------------------------------------------------------------------

bool mul_reduction(Node* node){

	assert(node != NULL);

	if(node->type == OPERATION && node->value == '*'){

		if(node->left->type == CONSTANT && node->right->type == CONSTANT){
			node->value = node->left->value * node->right->value;
			node->type  = CONSTANT;

			RemoveDescendants(node);
			return true;
		}
		else if(node->left->type == CONSTANT && node->left->value == 0){
			
			node->value = 0;
			node->type  = CONSTANT;

			RemoveDescendants(node);
			return true;
		}
		else if(node->right->type == CONSTANT && node->right->value == 0){
			
			node->value = 0;
			node->type  = CONSTANT;

			RemoveDescendants(node);
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------

bool pow_reduction(Node* node){

	assert(node != NULL);

	if(node->type == OPERATION && node->value == '^'){
		if(node->right->type == CONSTANT && node->right->value == 1){

			node->left->parent = node->parent;
			
			if(node->is_left){
				node->parent->left = node->left;
			}
			else{
				node->parent->right = node->left;
			}

			node->left   = NULL;
			node->parent = NULL;

			NodeDestructor(node);
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------

bool sum_reduction(Node* node){

	assert(node != NULL);

	if(node->type == OPERATION){
		if(node->left->type == CONSTANT && node->right->type == CONSTANT){

			if(node->value == '+'){
				node->value = node->left->value + node->right->value;
				node->type  = CONSTANT;

				RemoveDescendants(node);
				return true;
			}
			else if(node->value == '-'){
				node->value = node->left->value - node->right->value;
				node->type  = CONSTANT;

				RemoveDescendants(node);
				return true;
			}
		}
		/*
		else if(node->left->type == CONSTANT && node->left->value == 0){

			if(node->value == '+'){

				if(node->is_left){
					CopyNode(node->parent, node->right, NODE_PLACE::LEFT);
				}
				else{
					CopyNode(node->parent, node->right, NODE_PLACE::RIGHT);
				}

				node->parent = NULL;
				NodeDestructor(&node);
				return true;
			}
		}
		else if(node->right->type == CONSTANT && node->right->value == 0){
			if(node->value == '+' || node->value == '-'){

				if(node->is_left){
					CopyNode(node->parent, node->left, NODE_PLACE::LEFT);
				}
				else{
					CopyNode(node->parent, node->left, NODE_PLACE::RIGHT);
				}

				node->parent = NULL;
				NodeDestructor(&node);
				return true;
			}
		}
		*/
	}

	return false;
}

//------------------------------------------------------------------

void destroy_buf(char* buffer){
	free(buffer);
}

//------------------------------------------------------------------

bool check_presence_of_var(Node* node){

	assert(node != NULL);

	if(node->type == VARIABLE){
		return true;
	}

	bool has_var = false;

	if(node->left != NULL){
		has_var |= check_presence_of_var(node->left);
	}

	if(node->right != NULL){
		has_var |= check_presence_of_var(node->right);
	}

	return has_var;
}

bool check_low_priority_operation(Node* node, char source_oper){

	assert(node != NULL);

	// fix
	if(node->type == OPERATION){

		if(source_oper == '+' || source_oper == '-'){
			return false;
		}
		else if(source_oper == '*' || source_oper == '/'){
			if(node->value == '+' || node->value == '-') return true;
			else return false;
		}
		else if(source_oper == '^'){
			if(node->value == '^') return false;
			else return true;
		}
		return true;
	}

	bool has_var = false;

	if(node->left != NULL){
		has_var |= check_low_priority_operation(node->left, source_oper);
	}

	if(node->right != NULL){
		has_var |= check_low_priority_operation(node->right, source_oper);
	}

	return has_var;
}

bool is_operation(char c){

	for(int i = 0; i < N_OPERATIONS; i++){
		if(OPERATIONS_SYMB[i] == c){
			return true;
		}
	}

	return false;
}

bool is_variable(char c){

	return (c >= 'a' &&  c <= 'z') || (c >= 'A' && c <= 'Z');
}

/*
#define DEF_FUNC(name, num, code)		      \
if(strcmp(name, input_name) == 0) return i;
*/

int get_func_code(char* input_name){

	assert(input_name != NULL);

	for(int i = 0; i < N_FUNCS; i++){
		if(strcmp(func_names[i], input_name) == 0) return i;
	}

	return -1;
}

bool is_null(Node* node){

	assert(node != NULL);

	return node->type == CONSTANT && node->value == 0;
}
