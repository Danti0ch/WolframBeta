/**
 * \file
 * 	\brief Библиотека, в который реализована структура узла и методы, для работы с ней.
 */

#ifndef NODE_H
#define NODE_H

#include <string.h>
#include "log.h"

#define SIMPLE_DUMP   0
#define DETAILED_DUMP 1

#ifndef DUMP_MODE
#define DUMP_MODE SIMPLE_DUMP
#endif

/**
 * энам, для определения положения узла относительно родителя
 */
enum class NODE_PLACE{
	LEFT,
	RIGHT
};

/**
 * энам, определяющий тип узла
 */
enum class NODE_TYPE{
	INVALID,
	VARIABLE,
	CONSTANT,
	OPERATION,
	FUNCTION
};

union T_Node{
	int    func_id;
	char   symb;
	double const_val;
};

/**
 * структура узла
 */
struct Node{

	Node*      left; 	  //!< указатель на левого потомка
	Node*      right;   //!< указатель на правого потомка
	Node*      parent;  //!< указатель на родителя

	T_Node     value;   //!< значение узла

	NODE_TYPE  type;    //!< тип узла
	NODE_PLACE place; //!< задаёт положение узла относительно родителя
};

const T_Node VAL_POISON  = { .const_val = -7 };	//!< яд для поля value
const int NODE_POISON    = 0xDEADD00D;

const int MAX_FUNC_LEN = 10;

const int  MAX_N_OPERS = 10;
const char OPERATIONS_SYMB[MAX_N_OPERS] = "+-/*^";			//!< Бинарные операции хранятся в символьном массиве
const int  N_OPERATIONS = strlen(OPERATIONS_SYMB);

const int MAX_IMG_N_STRING_LEN = 20;
const int FILE_NAME_LEN        = 40;
const char GVIZ_DOT_NAME[FILE_NAME_LEN]   = "tmp.dot";			//!< Имя dot файла, куда происходит вывод дерева
const char DUMP_IMAGE_NAME[FILE_NAME_LEN] = "dump";		//!< Имя дамп 

//------------PUBLIC-FUNCTIONS-DECLARATION------------------------

Node* NodeCtor(Node* parent, T_Node val, NODE_TYPE type, NODE_PLACE place);
Node* NodeCtor(Node* parent, double val, NODE_TYPE type, NODE_PLACE place);
Node* NodeCtor(Node* parent, char val, NODE_TYPE type, NODE_PLACE place);
Node* NodeCtor(Node* parent, int val, NODE_TYPE type, NODE_PLACE place);
Node* NodeCtor(Node* parent, int val, NODE_TYPE type, NODE_PLACE place, Node* left, Node* right);

Node* NodeCtor(T_Node val, NODE_TYPE type);
Node* NodeCtor(double val, NODE_TYPE type);
Node* NodeCtor(char val, NODE_TYPE type);
Node* NodeCtor(int val, NODE_TYPE type);

/**
 * создаёт узел с копиями узлов left, right в качестве дочерних
 */
Node* NodeCtor(char val, NODE_TYPE type, Node* left, Node* right);
Node* NodeCtor(int val, NODE_TYPE type, Node* left);

Node* NodeCtor();
/**
 * удаляет узел, без удаления потомков
 */
void NodeDtor(Node* nod);

/**
 * удаляет узел и потомков
 */
void NodeFullDtor(Node* node);

/**
 * удаляет потомков у узла
 */
void RemoveDescendants(Node* node);

/**
 * создаёт копию узла со всеми потомками
 *
 * \param parent родитель для копии
 * \param place расположение копии узла
 *
 * \return копия узла source_node
 */
Node* CopyNode(Node* parent, Node* source_node, NODE_PLACE place);

/**
 * создаёт связь между узлами parent, son
 */
void MakeConnection(Node* parent, Node* son, NODE_PLACE place);

/**
 * дезинтегрирует связь c отцовским узлом son
 */
void BreakConnWithParent(Node* son);

/**
 * дезинтегрирует связь c дочерним узлом parent
 */
void BreakConnWithSon(Node* parent, NODE_PLACE place);

/**
 * присваивает тип и значение узлу(в зависимости от типа)
 */
void AssignVal(Node* node, T_Node val, NODE_TYPE type);
void AssignVal(Node* node, double val, NODE_TYPE type);
void AssignVal(Node* node, int val, NODE_TYPE type);
void AssignVal(Node* node, char val, NODE_TYPE type);

/**
 * выводит картинку с дампом узла node со всеми потомками
 */
void ShowNode(Node* node);

/**
 * проверяет, равны ли два узла(без потомков)
 *
 * \return 1, если равны. 0, если не равны
 */
bool EqualCheck(Node* node, Node* other_node);

/**
 * проверяет, равны ли два узла(со всеми потомками)
 *
 * \return 1, если равны. 0, если не равны
 */
bool EqualFullCheck(Node* node, Node* other_node);

/**
 * возвращает имя функции по её коду
 */
char* GetFuncName(const int func_id);

//__________________________________________________________________
/**
 * проверяет, что узел был создан конструктором(то что он не пуст)
 */
inline bool IsValid(const Node* node){
	return (node != NULL) && (node != (Node*)NODE_POISON);
}
//__________________________________________________________________
/**
 * \return 1 если node не имеет потомков
 * \return 0 если node имеет потомков
 */
inline bool IsLeaf(const Node* node){
	return node->left == NULL && node->right == NULL;
}
//__________________________________________________________________

inline bool IsLeft(const Node* node){
	return node->place == NODE_PLACE::LEFT;
}

inline bool IsRight(const Node* node){
	return node->place == NODE_PLACE::RIGHT;
}
//__________________________________________________________________

inline bool IsOperation(const Node* node){
	return node->type == NODE_TYPE::OPERATION;
}

inline bool IsConstant(const Node* node){
	return node->type == NODE_TYPE::CONSTANT;
}

inline bool IsFunction(const Node* node){
	return node->type == NODE_TYPE::FUNCTION;
}

inline bool IsVariable(const Node* node){
	return node->type == NODE_TYPE::VARIABLE;
}
//__________________________________________________________________

#endif //NODE_H
