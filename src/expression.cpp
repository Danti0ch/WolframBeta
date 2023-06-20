#include "expression.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <cstdlib>
#include "dsl.h"

//---------------LOCAL-FUNCTIONS-DECLARATION-----------------------------------------

/**
 * возвращает поддерево, которое является "производной" узла - константы node
 */
static Node* const_dif(Node* node);

/**
 * возвращает поддерево, которое является "производной" узла - переменной node
 */
static Node* var_dif(Node* node);

/**
 * возвращает поддерево, которое является "производной" узла - операции node
 */
static Node* oper_dif(Node* node);

/**
 * возвращает поддерево, которое является "производной" узла - функции node
 */
static Node* func_dif(Node* node);

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

/**
 * проверяет, есть ли узел типа variable в субдереве node
 * \return 1, если есть, 0, если нет
 */
static bool check_presence_of_var(Node* node);

/**
 * возвращает значение узла node в точке arg
 */
static double get_value_from_node(Node* node, double arg);

//--------------PUBLIC-FUNCTIONS-DEFINITIONS-----------------------------------------

void InitExpr(Expr* expr){

	assert(expr != NULL);

	expr->root = NodeCtor();

	expr->root->type = NODE_TYPE::CONSTANT;
	ToLog(LOG_TYPE::INFO, "expression %p was initiated", expr);
	return;
}
//__________________________________________________________________

void DifExpr(Expr* expr, Expr* dif_expr, int n){

	assert(expr     != NULL);
	assert(dif_expr != NULL);

	DestrExpr(dif_expr);
	InitExpr(dif_expr);

	ToLog(LOG_TYPE::INFO, "attempt to diff Expr %p", expr);

	while(node_reduction(expr->root->left));

	Node* new_diffed_node = NodeDif(expr->root->left);
	MakeConnection(dif_expr->root, new_diffed_node, NODE_PLACE::LEFT);
	while(node_reduction(dif_expr->root->left));

	for(int i = 1; i < n; i++){
		new_diffed_node = NodeDif(dif_expr->root->left);

		NodeFullDtor(dif_expr->root->left);

		MakeConnection(dif_expr->root, new_diffed_node, NODE_PLACE::LEFT);
		new_diffed_node      = (Node*)NODE_POISON;

		while(node_reduction(dif_expr->root->left));
	}

	ToLog(LOG_TYPE::INFO, "successful diff Expr %p", expr);
	
	return;
}
//__________________________________________________________________

Node* NodeDif(Node* node){

	assert(node != NULL);

	if(!IsValid(node)){
		ToLog(LOG_TYPE::ERROR, "attempt to diff invalid type Node %p", node);
		return (Node*)NODE_POISON;
	}

	switch((int)node->type){
		case (int)NODE_TYPE::CONSTANT:
			return const_dif(node);
			break;

		case (int)NODE_TYPE::VARIABLE:
			return var_dif(node);
			break;

		case (int)NODE_TYPE::OPERATION:
			return oper_dif(node);
			break;

		case (int)NODE_TYPE::FUNCTION:
			return func_dif(node);
			break;

		default:
			ToLog(LOG_TYPE::ERROR, "attempt to diff invalid type Node %p", node);
			return (Node*)NODE_POISON;
			break;
	}

	return (Node*)NODE_POISON;
}
//__________________________________________________________________

static Node* const_dif(Node* node){

	assert(node != NULL);
	return NodeCtor(0.0, NODE_TYPE::CONSTANT);
}
//__________________________________________________________________

static Node* var_dif(Node* node){

	assert(node != NULL);
	return NodeCtor(1.0, NODE_TYPE::CONSTANT);
}
//__________________________________________________________________

static Node* oper_dif(Node* node){

	assert(node != NULL);

	switch(node->value.symb){
		case '+':
			return ADD(DIFF(L), DIFF(R));
			break;

		case '-':
			return SUB(DIFF(L), DIFF(R));
			break;
		
		case '*':
			return ADD(MUL(DIFF(L), R), MUL(L, DIFF(R)));
			break;

		case '/':
			return DIV(SUB(MUL(DIFF(L), R), 
				           MUL(DIFF(R), L)), MUL(R, R));
			break;

		case '^':
			if(!check_presence_of_var(R)){
				Node* pow_node = POW(L, SUB(R, NodeCtor(1.0, NODE_TYPE::CONSTANT)));
				return MUL(MUL(R, pow_node), DIFF(L));
			}
			else{

				Node* dif_left  = MUL(DIFF(R), NodeCtor(3, NODE_TYPE::FUNCTION, CopyNode(L)));
				Node* dif_right = MUL(MUL(R, DIV(NodeCtor(1.0, NODE_TYPE::CONSTANT), L)), DIFF(L));

				return MUL(CopyNode(node), ADD(dif_left, dif_right));
			}
			break;

		default:
			ToLog(LOG_TYPE::ERROR, "Node %p has invalid oper value", node);
			return (Node*)NODE_POISON;
			break;
	}

	return (Node*)NODE_POISON;
}
//__________________________________________________________________

#define DEF_FUNC(id, name, dif_code, val_code)		\
	case id:										\
		{											\
			dif_code 									\
		}											\
		break;

static Node* func_dif(Node* node){

	assert(node != NULL);

	switch(node->value.func_id){

		// КОДОГЕНЕРАЦИЯ
		#include "func_definitions.h"
		// КОДОГЕНЕРАЦИЯ
		
		default:
			ToLog(LOG_TYPE::ERROR, "Node %p has invalid func id", node);
			return (Node*)NODE_POISON;
			break;
	}

	return (Node*)NODE_POISON;
}

#undef DEF_FUNC
//__________________________________________________________________

double GetValueExpr(Expr* expr, double arg){

	assert(expr != NULL);

	double val = get_value_from_node(expr->root->left, arg);

	if(isnan(val) != 0){
		printf("calculated NAN value, exit");
		exit(0);
	}

	return val;
}
//__________________________________________________________________

#define DEF_FUNC(id, name, dif_code, val_code)	\
	case id:									\
		val_code

static double get_value_from_node(Node* node, double arg){

	assert(node != NULL);

	switch((int)node->type){
		case (int)NODE_TYPE::INVALID:
			ToLog(LOG_TYPE::ERROR, "Node %p has invalid type", node);
			exit(0);
			return -1;

		case (int)NODE_TYPE::CONSTANT:
			return node->value.const_val;

		case (int)NODE_TYPE::VARIABLE:
			return arg;

		case (int)NODE_TYPE::OPERATION:
			switch(node->value.symb){
				case '+':
					return get_value_from_node(node->left, arg) + get_value_from_node(node->right, arg);
				case '-':
					return get_value_from_node(node->left, arg) - get_value_from_node(node->right, arg);
				case '*':
					return get_value_from_node(node->left, arg) * get_value_from_node(node->right, arg);
				case '/':
					{
						double divisible = get_value_from_node(node->left, arg);
						double divider   = get_value_from_node(node->right, arg);
	
						return divisible / divider;
					}
					break;
				case '^':
					return pow(get_value_from_node(node->left, arg), get_value_from_node(node->right, arg));
				default:
					ToLog(LOG_TYPE::ERROR, "Node %p has invalid value", node);
					return -1.0;
			}

		case (int)NODE_TYPE::FUNCTION:

			switch(node->value.func_id){

				// КОДОГЕНЕРАЦИЯ
				#include "func_definitions.h"
				// КОДОГЕНЕРАЦИЯ

				default:
					ToLog(LOG_TYPE::ERROR, "Node %p has invalid value", node);
					return -1.0;
			}

		default:
			ToLog(LOG_TYPE::ERROR, "type of Node %p is not recognized");
			exit(0);
			return -1;
	}
}
#undef DEF_FUNC

//__________________________________________________________________

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

void ReductExpr(Expr* expr){

	assert(expr != NULL);

	while(node_reduction(expr->root->left));

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

		BreakConnWithParent(node);
		
		//(1) const * const
		if(IsConstant(left) && IsConstant(right)){
			NodeCtor(cur_parent, left->value.const_val * right->value.const_val, NODE_TYPE::CONSTANT, cur_place);

			NodeFullDtor(node);

			return true;
		}

		//(2) obj * 0 || 0 * obj
		else if((IsConstant(left)  && IsEqualConst(left->value.const_val, 0.0)) ||
			    (IsConstant(right) && IsEqualConst(right->value.const_val, 0))){
			NodeCtor(cur_parent, 0, NODE_TYPE::CONSTANT, cur_place);

			NodeFullDtor(node);
			return true;
		}

		//(3) obj1*obj2 = obj1^2 (if obj1= obj2)
		else if(EqualFullCheck(left, right)){
			Node* pow_node = NodeCtor(cur_parent, '^', NODE_TYPE::OPERATION, cur_place);

			CopyNode(pow_node, left, NODE_PLACE::LEFT);
			NodeCtor(pow_node, 2, NODE_TYPE::CONSTANT, NODE_PLACE::RIGHT);

			NodeFullDtor(node);
			return true;
		}

		//(4) 1*obj = obj*1 = obj
		else if((IsConstant(left)  && IsEqualConst(left->value.const_val, 1.0)) ||
				 (IsConstant(right) && IsEqualConst(right->value.const_val, 1.0))){
			
			if((IsConstant(left) && IsEqualConst(left->value.const_val, 1.0))){
				 CopyNode(cur_parent, right, cur_place);
			}
			else CopyNode(cur_parent, left, cur_place);

			NodeFullDtor(node);
			return true;
		}
		MakeConnection(cur_parent, node, cur_place);
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
		if(IsConstant(left) && IsEqualConst(left->value.const_val, 0)){
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
			NodeFullDtor(left);
			NodeFullDtor(right);

			return true;
		}

		//(2.1) 0 + obj
		else if(IsConstant(left) && IsEqualConst(left->value.const_val, 0.0) && node->value.symb == '+'){
			MakeConnection(cur_parent, right, cur_place);

			NodeFullDtor(node);
			NodeFullDtor(left);
			return true;
		}

		//(2.2) obj +- 0
		else if(IsConstant(right) && IsEqualConst(right->value.const_val, 0.0)){
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
		MakeConnection(node, right, NODE_PLACE::RIGHT);
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

		bool was_reducted = false;
		//(1) const^const
		if(IsConstant(left) && IsConstant(right)){
			NodeCtor(cur_parent, pow(left->value.const_val, right->value.const_val), NODE_TYPE::CONSTANT, cur_place);
			
			was_reducted = true;
		}
		else if(IsConstant(right) && IsEqualConst(right->value.const_val, 0.0)){
			NodeCtor(cur_parent, 1.0, NODE_TYPE::CONSTANT, cur_place);
			
			was_reducted = true;
		}
		else if(IsConstant(left) && IsEqualConst(left->value.const_val, 0.0)){
			NodeCtor(cur_parent, 0.0, NODE_TYPE::CONSTANT, cur_place);
			
			was_reducted = true;
		}
		else if(IsConstant(right) && IsEqualConst(right->value.const_val, 1.0)){
			CopyNode(cur_parent, left, cur_place);

			was_reducted = true;
		}
		else if(IsConstant(left) && (IsEqualConst(left->value.const_val, 1.0))){
			NodeCtor(cur_parent, 1.0, NODE_TYPE::CONSTANT, cur_place);
			
			was_reducted = true;
		}

		if(was_reducted){
			NodeFullDtor(node);
			NodeFullDtor(left);
			NodeFullDtor(right);

			return true;
		}

		MakeConnection(cur_parent, node, cur_place);
		MakeConnection(node, left, NODE_PLACE::LEFT);
		MakeConnection(node, right, NODE_PLACE::RIGHT);
	}

	return false;
}
//__________________________________________________________________

static bool check_presence_of_var(Node* node){

	assert(node != NULL);

	if(IsVariable(node)){
		return true;
	}

	bool has_var = false;

	if(IsValid(node->left)){
		has_var |= check_presence_of_var(node->left);
	}

	if(IsValid(node->right)){
		has_var |= check_presence_of_var(node->right);
	}

	return has_var;
}
//__________________________________________________________________
