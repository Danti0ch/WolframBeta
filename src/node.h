#ifndef NODE_H
#define NODE_H

#include <string.h>
#define meow printf("meeeeow\n");

const char GVIZ_DOT_NAME[40]   = "tmp.dot";
const char DUMP_IMAGE_NAME[40] = "img.png";

const int MAX_FUNC_NAME_LEN = 50;
const int N_FUNCS           = 4;

enum FUNC_CODES{

	INVALID_CODE = -1,
	SIN_CODE,
	COS_CODE,
	EXP_CODE,
	LOG_CODE
};

// upgrade mb
const char func_names[][MAX_FUNC_NAME_LEN] = {
	"sin", "cos", "exp", "log"
};

enum NodeType{

	INVALID,
	VARIABLE,
	CONSTANT,
	OPERATION,
	FUNCTION
};

typedef double T_Node;

struct Node{

	Node*    left;
	Node*    right;
	Node*    parent;

	T_Node   value;

	NodeType type;
	bool     is_left;
};

// const Node*  NODE_POISON = (Node*)7;
const T_Node VAL_POISON  = 0;

const char   OPERATIONS_SYMB[10] = "+-/*^";
const int    N_OPERATIONS = strlen(OPERATIONS_SYMB);

//------------PUBLIC-FUNCTIONS-DECLARATION------------------------

Node* MakeLeftNode(Node* parent, T_Node val, NodeType type);

Node* MakeRightNode(Node* parent, T_Node val, NodeType type);

void NodeDestructor(Node** nod);

void MakeTransNode(Node* source_node, T_Node val, NodeType type);

Node* DuplNodeToLeft(Node* parent, Node* source_node);

Node* DuplNodeToRight(Node* parent, Node* source_node);

bool IsLeaf(const Node* nod);

void ShowNode(Node* nod);

#endif
