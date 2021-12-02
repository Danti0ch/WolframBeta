#include "expression.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>

//---------------LOCAL-FUNCTIONS-DECLARATION-----------------------------------------

bool is_null(Node* node);

/**
 * функция-оптимизатор
 * \return 1, если произошло изменение, 0, если изменений не было
 */
static bool node_reduction(Node* node);

/**
 * функция-оптимизатор умножения
 * \return 1, если произошло изменение, 0, если изменений не было
 */
static bool mul_reduction(Node* node);

/**
 * функция-оптимизатор деления
 * \return 1, если произошло изменение, 0, если изменений не было
 */
static bool div_reduction(Node* node);

/**
 * функция-оптимизатор сложения
 * \return 1, если произошло изменение, 0, если изменений не было
 */
static bool sum_reduction(Node* node);

/**
 * функция-оптимизатор возведения в степень
 * \return 1, если произошло изменение, 0, если изменений не было
 */
static bool pow_reduction(Node* node);

//--------------PUBLIC-FUNCTIONS-DEFINITIONS-----------------------------------------

void InitExpr(Expr* expr){

	assert(expr != NULL);

	expr->root = NodeCtor();

	expr->root->type = NODE_TYPE::CONSTANT;
	ToLog(LOG_TYPE::INFO, "expression %p was initiated", expr);
	return;
}
//__________________________________________________________________
/*
void DifExpr(Expr* expr){

	assert(expr != NULL);

	while(node_reduction(expr->root->left));

	NodeDif(expr->root->left);

	while(node_reduction(expr->root->left));

	return;
}
*/
//__________________________________________________________________

//------------------------------------------------------------------
/*
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
*/
//------------------------------------------------------------------

void DestrExpr(Expr* expr){

	assert(expr != NULL);

	NodeFullDtor(expr->root);

	return;
}
//__________________________________________________________________

void ShowExpr(Expr* expr){

	assert(expr != NULL);

	ShowNode(expr->root->left);

	while(node_reduction(expr->root->left)){
		ShowNode(expr->root->left);
	};

	return;
}
//__________________________________________________________________

static bool node_reduction(Node* node){

	assert(node != NULL);

	if(mul_reduction(node)) return true;
	if(div_reduction(node)) return true;
	if(sum_reduction(node)) return true;
	if(pow_reduction(node)) return true;

	if(IsValid(node->left)){
		if(node_reduction(node->left)) return true;
	}

	if(IsValid(node->right)){
		if(node_reduction(node->right)) return true;
	}

	return false;
}
//__________________________________________________________________

static bool mul_reduction(Node* node){

	assert(node != NULL);

	if(IsOperation(node) && node->value.symb == '*'){


		Node*      cur_parent = node->parent;
		NODE_PLACE cur_place  = node->place;
		Node*      left       = node->left;
		Node*      right      = node->right;

		//(1) const * const
		if(IsConstant(left) && IsConstant(right)){
			Node* mul_node = NodeCtor(left->value.const_val * right->value.const_val, NODE_TYPE::CONSTANT);
			NodeFullDtor(node);

			MakeConnection(cur_parent, mul_node, cur_place);
			return true;
		}

		//(2) obj * 0 || 0 * obj
		else if((IsConstant(left)  && left->value.const_val  == 0) ||
			    (IsConstant(right) && right->value.const_val == 0)){
			Node* mul_node = NodeCtor(0, NODE_TYPE::CONSTANT);

			NodeFullDtor(node);

			MakeConnection(cur_parent, mul_node, cur_place);

			return true;
		}

		//(3) obj1*obj2 = obj1^2 (if obj1= obj2)
		else if(EqualFullCheck(left, right)){
			Node* pow_node = NodeCtor('^', NODE_TYPE::OPERATION);

			CopyNode(pow_node, left, NODE_PLACE::LEFT);
			NodeCtor(pow_node, 2, NODE_TYPE::CONSTANT, NODE_PLACE::RIGHT);

			NodeFullDtor(node);
			MakeConnection(cur_parent, pow_node, cur_place);

			return true;
		}
	}

	return false;
}
//__________________________________________________________________

static bool div_reduction(Node* node){
	
	assert(node != NULL);

	if(IsOperation(node) && node->value.symb == '/'){


		Node*      cur_parent = node->parent;
		NODE_PLACE cur_place  = node->place;
		Node*      left       = node->left;
		Node*      right      = node->right;

		//(1) 0/obj
		if(IsConstant(left) && left->value.const_val == 0){
			Node* new_node = NodeCtor(0, NODE_TYPE::CONSTANT);

			NodeFullDtor(node);
			MakeConnection(cur_parent, new_node, cur_place);

			return true;
		}

		//(2) obj/obj TODO: obj^const1 / obj^const2 = obj^(const1 - const2)
		else if(EqualFullCheck(left, right)){
			Node* new_node = NodeCtor(1.0, NODE_TYPE::CONSTANT);

			NodeFullDtor(node);
			MakeConnection(cur_parent, new_node, cur_place);

			return true;
		}
	}

	return false;
}
//__________________________________________________________________

static bool sum_reduction(Node* node){

	assert(node != NULL);

	if(IsOperation(node) && (node->value.symb == '+' || node->value.symb == '-')){

		Node*      cur_parent = node->parent;
		NODE_PLACE cur_place  = node->place;
		Node*      left       = node->left;
		Node*      right      = node->right;

		BreakConnWithParent(node);
		BreakConnWithSon(node, NODE_PLACE::LEFT);
		BreakConnWithSon(node, NODE_PLACE::RIGHT);

		//(1) const1 +- const2 
		if(IsConstant(left) && IsConstant(right)){
			if(node->value.symb == '+'){
				NodeCtor(cur_parent, left->value.const_val + right->value.const_val, NODE_TYPE::CONSTANT, cur_place);
			}
			else{
				NodeCtor(cur_parent, left->value.const_val - right->value.const_val, NODE_TYPE::CONSTANT, cur_place);				
			}

			NodeFullDtor(node);

			return true;
		}

		//(2.1) 0 + obj
		else if(IsConstant(left) && left->value.const_val == 0 && node->value.symb == '+'){
			MakeConnection(cur_parent, right, cur_place);

			NodeFullDtor(node);
			NodeFullDtor(left);
			return true;
		}

		//(2.2) obj +- 0
		else if(IsConstant(right) && right->value.const_val == 0){
			MakeConnection(cur_parent, left, cur_place);

			NodeFullDtor(node);
			NodeFullDtor(right);
			return true;
		}
		
		//(3) obj1 + obj1 = 2obj1
		else if(EqualFullCheck(left, right) && node->value.symb == '+'){

			Node* mul = NodeCtor(cur_parent, '*', NODE_TYPE::OPERATION, cur_place);

			NodeCtor(mul, 2.0, NODE_TYPE::CONSTANT, NODE_PLACE::LEFT);
			MakeConnection(mul, left, NODE_PLACE::RIGHT);

			NodeFullDtor(node);
			NodeFullDtor(right);
			return true;
		}
		else if(EqualFullCheck(left, right)){
			NodeCtor(cur_parent, 0.0, NODE_TYPE::CONSTANT, cur_place);

			NodeFullDtor(node);
			NodeFullDtor(left);
			NodeFullDtor(right);
			return true;
		}

		MakeConnection(cur_parent, node, cur_place);
		MakeConnection(node, left, NODE_PLACE::LEFT);
		MakeConnection(node, right, NODE_PLACE::LEFT);
	}

	return false;
}
//__________________________________________________________________

static bool pow_reduction(Node* node){
	
	assert(node != NULL);

	if(IsOperation(node) && node->value.symb == '^'){

		Node*      cur_parent = node->parent;
		NODE_PLACE cur_place  = node->place;
		Node*      left       = node->left;
		Node*      right      = node->right;

		BreakConnWithParent(node);
		BreakConnWithSon(node, NODE_PLACE::LEFT);
		BreakConnWithSon(node, NODE_PLACE::RIGHT);

		//(1) const^const
		if(IsConstant(left) && IsConstant(right)){
			NodeCtor(cur_parent, pow(left->value.const_val, right->value.const_val), NODE_TYPE::CONSTANT, cur_place);
			
			NodeFullDtor(node);
			NodeFullDtor(left);
			NodeFullDtor(right);

			return true;
		}
		else if(IsConstant(right) && right->value.const_val == 0){
			NodeCtor(cur_parent, 1.0, NODE_TYPE::CONSTANT, cur_place);
			
			NodeFullDtor(node);
			NodeFullDtor(left);
			NodeFullDtor(right);

			return true;
		}
		
		MakeConnection(cur_parent, node, cur_place);
		MakeConnection(node, left, NODE_PLACE::LEFT);
		MakeConnection(node, right, NODE_PLACE::LEFT);
	}

	return false;
}

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