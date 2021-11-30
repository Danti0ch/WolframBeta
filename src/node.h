/**
 * \file
 * 	\brief Библиотека, в который реализована структура узла и методы, для работы с ней.
 */

#ifndef NODE_H
#define NODE_H

#include <string.h>

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

typedef double T_Node;

/**
 * структура узла
 */
struct Node{

	Node*    left; 	 //!< указатель на левого потомка
	Node*    right;  //!< указатель на правого потомка
	Node*    parent; //!< указатель на родителя

	T_Node   value;  //!< значение узла

	NODE_TYPE type;   //!< тип узла
	bool     is_left;//!< задаёт положение узла относительно родителя
};

const T_Node VAL_POISON  = 0;	//!< яд для поля value

const int MAX_N_FUNCS = 20;

char func_names[MAX_N_FUNCS] = "scl";				//!< Имена фукнций однстрочные и хранятся в символьном массиве
const int N_FUNCS = strlen(func_names);

const int MAX_N_OPERS = 10;
const char OPERATIONS_SYMB[MAX_N_OPERS] = "+-/*^";			//!< Бинарные операции хранятся в символьном массиве
const int  N_OPERATIONS = strlen(OPERATIONS_SYMB);

const char GVIZ_DOT_NAME[40]   = "tmp.dot";			//!< Имя dot файла, куда происходит вывод дерева
const char DUMP_IMAGE_NAME[40] = "dump.png";		//!< Имя дамп 

//------------PUBLIC-FUNCTIONS-DECLARATION------------------------

/**
 * создаёт узел
 */
Node* NodeConstructor(Node* parent, T_Node val, NODE_TYPE type, NODE_PLACE place);

/**
 * удаляет узел, без удаления потомков
 */
void NodeDestructor(Node* nod);

/**
 * удаляет узел и потомков
 */
void NodeFullDestructor(Node** node);

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
 * убирает всех потомков у узла
 */
void MakeZeroNode(Node* node);

/**
 * \return 1 если node не имеет потомков
 * \return 0 если node имеет потомков
 */
bool IsLeaf(const Node* node);

/**
 * выводит картинку с дампом узла node со всеми потомками
 */
void ShowNode(Node* node);

#endif
