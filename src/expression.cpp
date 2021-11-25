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

void sub_expr_destr(Expr* expr, Node* nod);

//--------------PUBLIC-FUNCTIONS-DEFINITIONS-----------------------------------------

void ExprInit(Expr* expr){

	assert(expr != NULL);

	expr->root.left   = NULL;
	expr->root.right  = NULL;
	expr->root.parent = NULL;
	expr->root.type   = INVALID;

	expr->size       = 0;
	expr->is_saved   = false;

	return;
}

//------------------------------------------------------------------

void ExprRead(FILE* f_stream, Expr* expr){

	assert(expr        != NULL);
	assert(f_stream != NULL);

	char *buffer = NULL;

	create_buf(&buffer, f_stream);

	uint32_t buf_size = strlen(buffer);

	int    n_cur_symb    = 0;
	int    n_readen      = 0;

	T_Node value = 0;
	char   symb  = '0';

	char   function_name[MAX_FUNC_NAME_LEN] = "";

	Node*  cur_nod = &(expr->root);

	while(n_cur_symb < buf_size){

		sscanf((char*)buffer + n_cur_symb, " %n", &n_readen);

		n_cur_symb += n_readen;

		if(n_cur_symb >= buf_size) break;

		if(buffer[n_cur_symb] == '('){
			if(cur_nod->left == NULL){
				cur_nod = MakeLeftNode(cur_nod, VAL_POISON, INVALID);
			}

			else{
				cur_nod = MakeRightNode(cur_nod, VAL_POISON, INVALID);
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

		if(sscanf((char*)buffer + n_cur_symb, "%[^(]%n", function_name, &n_readen) == 1){

			int func_code = get_func_code(function_name);

			if(func_code != INVALID_CODE){
				cur_nod->value = func_code;
				cur_nod->type  = FUNCTION;
				n_cur_symb     += n_readen;
				continue;
			}
		}
		
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
		}

		ExprDestr(expr);

		// err???
		return;
	}

}

//------------------------------------------------------------------

void ExprDif(Node* node){

	assert(node != NULL);

	if(node->type == INVALID) return;

	if(node->type == CONSTANT){
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
			ExprDif(node->left);
			ExprDif(node->right);
			
			return;
		}
		
		if(node->value == '*'){
			node->value = '+';
			
			Node* mul1 = node->left;
			Node* mul2 = node->right;
			
			MAKE_LEFT_MUL(node, mul1, mul2);
			MAKE_RIGHT_MUL(node, mul1, mul2);

			NodeDestructor(mul1);
			NodeDestructor(mul2);
			
			ExprDif(node->left->left);
			ExprDif(node->right->right);
			
			return;
		}

		if(node->value == '/'){

			Node* divisible = node->left;
			Node* divider = node->right;

			MakeTransNode(divisible, '-', OPERATION);

			MAKE_LEFT_MUL(node->left, divider, divisible);
			MAKE_RIGHT_MUL(node->left, divider, divisible);
			
			MAKE_RIGHT_MUL(noderight, divider, divider);

			NodeDestructor(divisible);
			NodeDestructor(divider);
			
			ExprDif(node->left->left->left);
			ExprDif(node->left->right->right);

			return;
		}

		/*
		if(node->type == '^'){

			Node* foundation_node = node->left;
			Node* degree_node     = node->right;

			if(isVarInTree(degree_node)){
				MakeTransNode(node, '*');
				MakeRightNode(node, '+', OPERATION);

				MakeLeftNode(node->right->left, '*', OPERATION);
				MakeLeftNode(node->right->left->left, 'l', FUNCTION);

				node->right->left->left->left = DuplicateNode(node->right->left->left, foundation_node);

				node->right->left->right = DuplicateNode(node->right->left, degree_node);


			}
		}
		*/
	}
}

//------------------------------------------------------------------

void ExprDestr(Expr* expr){

	assert(expr != NULL);

	sub_expr_destr(expr, &(expr->root));

	return;
}

//------------------------------------------------------------------

void sub_expr_destr(Expr* expr, Node* nod){

	assert(nod != NULL);

	if(nod->left != NULL) sub_expr_destr(expr, nod->left);

	if(nod->right != NULL) sub_expr_destr(expr, nod->right);

	NodeDestructor(&nod);

	expr->size--;

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

void destroy_buf(char* buffer){
	free(buffer);
}

//------------------------------------------------------------------

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

	return INVALID_CODE;
}
