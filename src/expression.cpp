#include "expression.h"
#include <assert.h>
#include <stdlib.h>
#include "dls_functions.h"

//---------------LOCAL-FUNCTIONS-DECLARATION-----------------------------------------

bool is_null(Node* node);

//--------------PUBLIC-FUNCTIONS-DEFINITIONS-----------------------------------------

void InitExpr(Expr* expr){

	assert(expr != NULL);

	expr->root = NodeCtor();

	ToLog(LOG_TYPE::INFO, "expression %p was initiated", expr);
	return;
}
//__________________________________________________________________
/*
void ExprDif(Expr* expr){

	assert(expr != NULL);

	while(node_reduction(expr->root.left));

	NodeDif(expr->root.left);

	while(node_reduction(expr->root.left));

	ShowExpr(expr);
	return;
}

//------------------------------------------------------------------

void NodeDif(Node* node){

	assert(node != NULL);
	if(node->type == NODE_TYPE::INVALID){
		ToLog(LOG_TYPE::WARNING, "Node %p is invalid", node);
		return;
	}

	if(check_presence_of_var(node) == false){

		RemoveDescendants(node);
		node->type  = NODE_TYPE::CONSTANT;
		node->value = 0;
		return;
	}

	if(IsVariable(node)){
		node->type  = NODE_TYPE::CONSTANT;
		node->value = 1;
		return;
	}

	if(IsOperation(node)){
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

			NodeConstructor(node, '-', NODE_TYPE::OPERATION, NODE_PLACE::LEFT);

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

				node->type = NODE_TYPE::CONSTANT;
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

					NodeConstructor(node, '*', NODE_TYPE::OPERATION, NODE_PLACE::LEFT);

					CopyNode(node->left, degree_node, NODE_PLACE::LEFT);
					CopyNode(node->left, foundation_node, NODE_PLACE::RIGHT);
					NodeDif(node->left->right);

					NodeConstructor(node, '^', NODE_TYPE::OPERATION, NODE_PLACE::RIGHT);

					CopyNode(node->right, foundation_node, NODE_PLACE::LEFT);

					NodeConstructor(node->right, '-', NODE_TYPE::OPERATION, NODE_PLACE::RIGHT);
					CopyNode(node->right->right, degree_node, NODE_PLACE::LEFT);

					NodeConstructor(node->right->right, 1, NODE_TYPE::CONSTANT, NODE_PLACE::RIGHT);

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
	if(IsFunction(node)){
		Node* arg = node->left;

		if(node->value == 'L'){

			node->value = '*';
			node->type  = NODE_TYPE::OPERATION;

			NodeConstructor(node, '/', NODE_TYPE::OPERATION, NODE_PLACE::LEFT);
			NodeConstructor(node->left, 1, NODE_TYPE::CONSTANT, NODE_PLACE::LEFT);
			CopyNode(node->left, arg, NODE_PLACE::RIGHT);

			CopyNode(node, arg, NODE_PLACE::RIGHT);
			NodeDif(node->right);
		}
		else if(node->value == 'S'){

			node->value = '*';
			node->type  = NODE_TYPE::OPERATION;

			NodeConstructor(node, 'C', NODE_TYPE::FUNCTION, NODE_PLACE::LEFT);
			CopyNode(node->left, arg, NODE_PLACE::LEFT);

			CopyNode(node, arg, NODE_PLACE::RIGHT);
			NodeDif(node->right);
		}
		else if(node->value == 'C'){

			node->value = '*';
			node->type  = NODE_TYPE::OPERATION;

			NodeConstructor(node, '-', NODE_TYPE::OPERATION, NODE_PLACE::LEFT);
			NodeConstructor(node->left, 0, NODE_TYPE::CONSTANT, NODE_PLACE::LEFT);

			NodeConstructor(node->left, 'S', NODE_TYPE::FUNCTION, NODE_PLACE::RIGHT);
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
*/
void DestrExpr(Expr* expr){

	assert(expr != NULL);

	NodeFullDtor(expr->root);

	return;
}
//__________________________________________________________________

void ShowExpr(Expr* expr){

	assert(expr != NULL);

	ShowNode(expr->root->left);

	return;
}
//__________________________________________________________________
/*
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

	if(IsOperation(node) && node->value == '*'){

		if(IsConstant(node->left) && IsConstant(node->right)){
			node->value = node->left->value * node->right->value;
			node->type  = NODE_TYPE::CONSTANT;

			RemoveDescendants(node);
			return true;
		}
		else if(IsConstant(node->left) && node->left->value == 0){
			
			node->value = 0;
			node->type  = NODE_TYPE::CONSTANT;

			RemoveDescendants(node);
			return true;
		}
		else if(IsConstant(node->right) && node->right->value == 0){
			
			node->value = 0;
			node->type  = NODE_TYPE::CONSTANT;

			RemoveDescendants(node);
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------

bool pow_reduction(Node* node){

	assert(node != NULL);

	if(IsOperation(node) && node->value == '^'){
		if(IsConstant(node->right) && node->right->value == 1){

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

	if(IsOperation(node)){
		if(IsConstant(node->left) && IsConstant(node->right)){

			if(node->value == '+'){
				node->value = node->left->value + node->right->value;
				node->type  = NODE_TYPE::CONSTANT;

				RemoveDescendants(node);
				return true;
			}
			else if(node->value == '-'){
				node->value = node->left->value - node->right->value;
				node->type  = NODE_TYPE::CONSTANT;

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
		///
	}

	return false;
}
*/
//------------------------------------------------------------------


//------------------------------------------------------------------
/*
bool check_presence_of_var(Node* node){

	assert(node != NULL);

	if(IsVariable(node)){
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

//------------------------------------------------------------------

bool check_low_priority_operation(Node* node, char source_oper){

	assert(node != NULL);

	// fix
	if(IsOperation(node)){

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

//------------------------------------------------------------------


//------------------------------------------------------------------

bool is_null(Node* node){

	assert(node != NULL);

	return IsConstant(node) && node->value == 0;
}
*/