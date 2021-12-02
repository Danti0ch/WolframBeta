#include "node.h"
#include <assert.h>
#include <stdio.h>
#include <cstdlib>
#include <cstdarg>

Node* NodeCtor(Node* parent, double val, NODE_TYPE type, NODE_PLACE place){

	assert(parent != NULL);

	T_Node node_val = { .const_val = val};

	Node* node = NodeCtor(parent, node_val, NODE_TYPE::CONSTANT, place);

	return node;
}
//__________________________________________________________________

Node* NodeCtor(Node* parent, char val, NODE_TYPE type, NODE_PLACE place){

	assert(parent != NULL);

	T_Node node_val = { .symb = val};
	
	return NodeCtor(parent, node_val, type, place);
}
//__________________________________________________________________

Node* NodeCtor(Node* parent, int val, NODE_TYPE type, NODE_PLACE place){

	assert(parent != NULL);
	
	Node* node = (Node*)NODE_POISON;

	if(type == NODE_TYPE::FUNCTION){
		T_Node node_val = { .func_id = val};
		
		node = NodeCtor(parent, node_val, NODE_TYPE::FUNCTION, place);
	}
	else{
		T_Node node_val = { .const_val = (double)val};
		
		node = NodeCtor(parent, node_val, NODE_TYPE::CONSTANT, place);
	}

	return node;
}
//__________________________________________________________________

Node* NodeCtor(Node* parent, T_Node val, NODE_TYPE type, NODE_PLACE place){

	assert(parent != NULL);

	Node* node = NodeCtor(val, type);

	if(node == NULL) return NULL;

	node->parent   = parent;

	node->place = place;

	if(place == NODE_PLACE::LEFT){
		if(parent != NULL) parent->left  = node;
	}
	else{
		if(parent != NULL) parent->right = node;
	}

	return node;
}
//__________________________________________________________________

Node* NodeCtor(T_Node val, NODE_TYPE type){

	Node* node  = (Node*)calloc(1, sizeof(Node));

	if(node == NULL){
		ToLog(LOG_TYPE::WARNING, "couldn't alloc mem");
		return NULL;
	}

	AssignVal(node, val, type);

	node->left   = (Node*)NODE_POISON;
	node->right  = (Node*)NODE_POISON;
	node->parent = (Node*)NODE_POISON;

	node->place  = NODE_PLACE::LEFT;

	ToLog(LOG_TYPE::INFO, "created Node %p", node);
	return node;
}
//__________________________________________________________________

Node* NodeCtor(double val, NODE_TYPE type){

	T_Node node_val = { .const_val = val};

	return NodeCtor(node_val, type);
}
//__________________________________________________________________

Node* NodeCtor(char val, NODE_TYPE type){

	T_Node node_val = { .symb = val};

	return NodeCtor(node_val, type);
}
//__________________________________________________________________

Node* NodeCtor(int val, NODE_TYPE type){

	T_Node node_val = { .func_id = val};

	return NodeCtor(node_val, type);
}
//__________________________________________________________________

Node* NodeCtor(char val, NODE_TYPE type, Node* left, Node* right){

	if(type != NODE_TYPE::OPERATION) return NULL;

	T_Node node_val = { .symb = val};

	Node* node = NodeCtor(node_val, type);

	CopyNode(node, left, NODE_PLACE::LEFT);
	CopyNode(node, right, NODE_PLACE::RIGHT);
		
	return node;
}
//__________________________________________________________________

Node* NodeCtor(){

	return NodeCtor(VAL_POISON, NODE_TYPE::INVALID);
}
//__________________________________________________________________

void NodeDtor(Node* node){

	assert(node != NULL);

	if(node == NULL)  return;
	
	if(IsLeft(node) && IsValid(node->parent)){
		node->parent->left = (Node*)NODE_POISON;
	}

	else if(IsValid(node->parent)){
		node->parent->right = (Node*)NODE_POISON;
	}

	ToLog(LOG_TYPE::INFO, "destruct Node %p", node);

	free(node);

	return;
}
//__________________________________________________________________

void NodeFullDtor(Node* node){

	assert(node != NULL);

	if(IsValid(node->left))  NodeFullDtor(node->left);

	if(IsValid(node->right)) NodeFullDtor(node->right);

	NodeDtor(node);
	
	return;
}
//__________________________________________________________________

void RemoveDescendants(Node* node){

	assert(node != NULL);

	if(node->left  != NULL) NodeFullDtor(node->left);
	if(node->right != NULL) NodeFullDtor(node->right);

	ToLog(LOG_TYPE::INFO, "destructed descedants from Node %p", node);
	return;
}
//__________________________________________________________________

Node* CopyNode(Node* parent, Node* source_node, NODE_PLACE place){

	assert(parent 		!= NULL);
	assert(source_node 	!= NULL);

	Node* new_node = NodeCtor(parent, source_node->value, source_node->type, place);

	if(new_node == NULL) return NULL;

	if(IsValid(source_node->left)){
		CopyNode(new_node, source_node->left, NODE_PLACE::LEFT);
	}

	if(IsValid(source_node->right)){
		CopyNode(new_node, source_node->right, NODE_PLACE::RIGHT);
	}

	ToLog(LOG_TYPE::INFO, "copied Node %p to Node %p", source_node, new_node);
	return new_node;
}
//__________________________________________________________________

void MakeConnection(Node* parent, Node* son, NODE_PLACE place){

	assert(parent != NULL);
	assert(son    != NULL);

	son->place = place;

	if(place == NODE_PLACE::LEFT){
		parent->left = son;
	}
	else{
		parent->right = son;
	}

	son->parent = parent;

	return;
}
//__________________________________________________________________

void BreakConnWithParent(Node* son){

	assert(son    != NULL);

	if(!IsValid(son->parent)) return;

	if(IsLeft(son)){
		son->parent->left = (Node*)NODE_POISON;
	}
	else{
		son->parent->right = (Node*)NODE_POISON;		
	}

	son->parent = (Node*)NODE_POISON;

	return;
}
//__________________________________________________________________

void BreakConnWithSon(Node* parent, NODE_PLACE place){

	assert(parent    != NULL);

	if(place == NODE_PLACE::LEFT){
		if(!IsValid(parent->left)) return;

		parent->left->parent = (Node*)NODE_POISON;
		parent->left         = (Node*)NODE_POISON;
	}

	else{
		if(!IsValid(parent->right)) return;

		parent->right->parent = (Node*)NODE_POISON;
		parent->right         = (Node*)NODE_POISON;
	}
	
	return;
}
//__________________________________________________________________

void AssignVal(Node* node, double val, NODE_TYPE type){

	assert(node != NULL);

	T_Node tmp = {.const_val = val};

	AssignVal(node, tmp, NODE_TYPE::CONSTANT);

	return;
}

void AssignVal(Node* node, int val, NODE_TYPE type){

	assert(node != NULL);

	T_Node tmp = {.func_id = val};

	AssignVal(node, tmp, NODE_TYPE::FUNCTION);

	return;
}

void AssignVal(Node* node, char val, NODE_TYPE type){

	assert(node != NULL);

	T_Node tmp = {.symb = val};
	
	if(type == NODE_TYPE::OPERATION){
		AssignVal(node, tmp, NODE_TYPE::OPERATION);
	}
	else if(type == NODE_TYPE::VARIABLE){
		AssignVal(node, tmp, NODE_TYPE::VARIABLE);
	}

	return;
}

void AssignVal(Node* node, T_Node val, NODE_TYPE type){

	assert(node != NULL);

	node->type = type;
	node->value = val;

	return;
}

bool EqualCheck(Node* node, Node* other_node){

	assert(node       != NULL);
	assert(other_node != NULL);

	if(node->type != other_node->type) return false;

	if(node->type == NODE_TYPE::CONSTANT){
		if(node->value.const_val != other_node->value.const_val) return false;
	}
	else if(node->type == NODE_TYPE::VARIABLE || node->type == NODE_TYPE::OPERATION){
		if(node->value.symb != other_node->value.symb) return false;
	}
	else if(node->type == NODE_TYPE::FUNCTION){
		if(node->value.func_id != other_node->value.func_id) return false;
	}

	return true;
}	

bool EqualFullCheck(Node* node, Node* other_node){

	assert(node 	  != NULL);
	assert(other_node != NULL);

	if(!EqualCheck(node, other_node)) return false;

	bool are_equal = true;

	if(IsValid(node->left) && IsValid(other_node->left)){
		are_equal &= EqualFullCheck(node->left, other_node->left);
	}
	else if(IsValid(node->left) || IsValid(other_node->left)){
		return false;
	}

	if(IsValid(node->right) && IsValid(other_node->right)){
		are_equal &= EqualFullCheck(node->right, other_node->right);
	}
	else if(IsValid(node->right) && IsValid(other_node->right)){
		return false;
	}

	return are_equal;
}

//__________________________________________________________________

#define DEF_FUNC(id, name, code)					\
{													\
	if(func_id == id){								\
		return (char*)(#name);						\
	}												\
}

char* GetFuncName(const int func_id){

	#include "func_definitions.h"

	// если ничего не найдено
	return NULL;
}

#undef DEF_FUNC
//__________________________________________________________________
