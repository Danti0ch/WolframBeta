#include "node.h"
#include <assert.h>
#include <stdio.h>
#include <cstdlib>
#include <cstdarg>

#include "log.h"

//------------LOCAL-FUNCTIONS-DECLARATION----------------------------------

/**
 * выделяет память под узел и присваивает ему значение и тип
 */
Node* generate_node(T_Node val, NODE_TYPE type);

/**
 * создаёт dot файл с информацией об узле node и его потомкаха
 */
void generate_dot(Node* node, char const * file_name);

/**
 * записывает в дот файл узел node со всеми потомками
 */
void write_node_to_dot(Node const * nod, int* n_node, FILE* output_file);

/**
 * создаёт картинку для дот файла
 *
 * \param input_file_name имя дот файлa
 * \param output_file_name имя картинки
 */
void generate_image(char const * input_file_name, char const * output_file_name);

/**
 * выполняет команду, указанную в строку cmd_str в консоли
 */
void execute_term_cmd(char const * cmd_str, ...);

/**
 * конвертируют код типа узла в строку
 */
const char* get_type_string(NODE_TYPE type);

//--------------PUBLIC-FUNCTIONS-DEFINITIONS-----------------------------------------

Node* NodeConstructor(Node* parent, T_Node val, NODE_TYPE type, NODE_PLACE place){

	Node* node = generate_node(val, type);

	if(node == NULL) return NULL;

	node->parent   = parent;

	if(place == NODE_PLACE::LEFT){
		node->is_left  = true;
		if(parent != NULL) parent->left  = node;
	}
	else{
		node->is_left  = false;
		if(parent != NULL) parent->right = node;
	}

	ToLog(LOG_TYPE::INFO, "created Node %p", node);
	return node;
}

//------------------------------------------------------------------

void NodeDestructor(Node* node){

	assert(node != NULL);

	if(node == NULL)  return;
	
	if(node->is_left && node->parent != NULL){
		node->parent->left = NULL;
	}

	else if(node->parent != NULL){
		node->parent->right = NULL;
	}

	free(node);

	ToLog(LOG_TYPE::INFO, "destruct Node %p", node);

	return;
}

//------------------------------------------------------------------

void NodeFullDestructor(Node* nod){

	assert(nod != NULL);

	if(nod->left != NULL) NodeFullDestructor(nod->left);

	if(nod->right != NULL) NodeFullDestructor(nod->right);

	NodeDestructor(nod);
	
	ToLog(LOG_TYPE::INFO, "destruct Node %p", *nod);
	
	return;
}

//------------------------------------------------------------------

void RemoveDescendants(Node* node){

	assert(node != NULL);

	if(node->left  != NULL) NodeFullDestructor(&(node->left));
	if(node->right != NULL) NodeFullDestructor(&(node->right));

	ToLog(LOG_TYPE::INFO, "destructed descedants from Node %p", node);
	return;
}

//------------------------------------------------------------------

Node* CopyNode(Node* parent, Node* source_node, NODE_PLACE place){

	assert(parent 		!= NULL);
	assert(source_node 	!= NULL);

	Node* new_node = generate_node(source_node->value, source_node->type);

	if(new_node == NULL){
		ToLog(LOG_TYPE::WARNING, "couldn't alloc mem");
		return NULL;
	}

	if(source_node->left != NULL){
		CopyNode(new_node, source_node->left, NODE_PLACE::LEFT);

		if(new_node->left != NULL){
			new_node->left->parent  = new_node;
			new_node->left->is_left = true;
		}
	}

	if(source_node->right != NULL){
		CopyNode(new_node, source_node->right, NODE_PLACE::RIGHT);

		if(new_node->right != NULL){
			new_node->right->parent  = new_node;
			new_node->right->is_left = true;
		}
	}
	
	if(place == NODE_PLACE::LEFT){
		parent->left  = new_node;
		new_node->is_left = true;
	}
	else{
		parent->right  = new_node;
		new_node->is_left = false;
	}

	new_node->parent  = parent;

	ToLog(LOG_TYPE::INFO, "copied Node %p to Node %p", source_node, new_node);
	return new_node;
}

//------------------------------------------------------------------

bool IsLeaf(const Node* node){

	assert(node != NULL);
	return node->left == NULL && node->right == NULL;
}

//------------------------------------------------------------------

void ShowNode(Node* node){

	assert(node != NULL);

	generate_dot(node, GVIZ_DOT_NAME);
	generate_image(GVIZ_DOT_NAME, DUMP_IMAGE_NAME);
	
	system(DUMP_IMAGE_NAME);
	ToLog(LOG_TYPE::INFO, "showed Node %p", node);	
	return;
}

//------------------------------------------------------------------

void MakeZeroNode(Node* node){

	assert(node != NULL);

	RemoveDescendants(node);
	node->type  = CONSTANT;
	node->value = 0;

	ToLog(LOG_TYPE::INFO, "descedants of Node %p removed", node);
	return;
}

//--------------LOCAL-FUNCTIONS-DEFINITIONS-----------------------------------------

Node* generate_node(T_Node val, NODE_TYPE type){

	Node* node  = (Node*)calloc(1, sizeof(Node));

	if(node == NULL){
		ToLog(LOG_TYPE::WARNING, "couldn't alloc mem");
		return NULL;
	}

	node->value = val;
	node->type  = type;

	return node;
}

//------------------------------------------------------------------

void generate_dot(Node* node, char const * file_name){

	assert(node       != NULL);
	assert(file_name != NULL);

	FILE* tmp_file = fopen(file_name, "w");

	if(tmp_file == NULL){
		ToLog(LOG_TYPE::ERROR, "can't create/read file %s", file_name);
		return;
	}

	fprintf(tmp_file, "digraph Tree{\n");

	int n_node = 0;	//< индекс узла
	write_node_to_dot(node, &n_node, tmp_file);

	fprintf(tmp_file, "}\n");

	fclose(tmp_file);

	ToLog(LOG_TYPE::INFO, "file %s was generated for Node %p", file_name, node);
	return;
}

//------------------------------------------------------------------

void write_node_to_dot(Node const * nod, int* n_node, FILE* output_file){

	assert(nod         != NULL);
	assert(output_file != NULL);

	int cur_n_node = *n_node;

	#if DEBUG == 1
		if(nod->type == CONSTANT){
			fprintf(output_file, "%d[shape = \"record\", label = \"{"
											  "adress:  %p |"
											  "parent:  %p |"
											  "left:    %p |"
											  "right:   %p |"
											  "is_left: %s |"
											  "type:    %s |"
											  "value:   %.6lg}\"];\n", 
											  cur_n_node, nod,
											  nod->parent,
											  nod->left,
											  nod->right,
											  nod->is_left ? "true" : "false",
											  get_type_string(nod->type),
											  nod->value);
		}

		else if(nod->type == NODE_TYPE::VARIABLE || nod->type == NODE_TYPE::OPERATION){
			fprintf(output_file, "%d[shape = \"record\", label = \"{"
											  "adress:  %p |"
											  "parent:  %p |"
											  "left:    %p |"
											  "right:   %p |"
											  "is_left: %s |"
											  "type:    %s |"
											  "value:   %c}\"];\n", 
											  cur_n_node, nod,
											  nod->parent,
											  nod->left,
											  nod->right,
											  nod->is_left ? "true" : "false",
											  get_type_string(nod->type),
											  (char)nod->value);
		}
		else if(nod->type == FUNCTION){
			fprintf(output_file, "%d[shape = \"record\", label = \"{"
											  "adress:  %p |"
											  "parent:  %p |"
											  "left:    %p |"
											  "right:   %p |"
											  "is_left: %s |"
											  "type:    %s |"
											  "value:   %s}\"];\n", 
											  cur_n_node, nod,
											  nod->parent,
											  nod->left,
											  nod->right,
											  nod->is_left ? "true" : "false",
											  get_type_string(nod->type),
											  func_names[(int)nod->value]);
		}

	#else
		if(nod->type == CONSTANT){
			fprintf(output_file, "%d[shape = \"record\", label = \"%.6lg\"]\n", cur_n_node, nod->value);
		}

		else if(nod->type == VARIABLE || nod->type == OPERATION){
			fprintf(output_file, "%d[shape = \"record\", label = \"%c\"];\n", cur_n_node, (char)nod->value);
		}

		else if(nod->type == FUNCTION){
			fprintf(output_file, "%d[shape = \"record\", label = \"%s\"];\n", cur_n_node, func_names[(int)nod->value]);
		}
	#endif

	if(nod->left != NULL){
		(*n_node)++;
		fprintf(output_file, "%d->%d;\n", cur_n_node, *n_node);
		write_node_to_dot(nod->left, n_node, output_file);
	}

	if(nod->right != NULL){
		(*n_node)++;
		fprintf(output_file, "%d->%d;\n", cur_n_node, *n_node);
		write_node_to_dot(nod->right, n_node, output_file);
	}

	ToLog(LOG_TYPE::INFO, "Node %p was written to file", nod);
	return;
}

//------------------------------------------------------------------

void generate_image(char const * input_file_name, char const * output_file_name){

	assert(input_file_name   != NULL);
	assert(output_file_name  != NULL);
	assert(input_file_name   != output_file_name);

	execute_term_cmd("dot -Tpng %s -o %s", input_file_name, output_file_name);

	ToLog(LOG_TYPE::INFO, "Image %s has showen", output_file_name);
	return;
}

//------------------------------------------------------------------

void execute_term_cmd(char const * cmd_str, ...){

	assert(cmd_str != NULL);

	va_list args;
    va_start(args, cmd_str);

    int buflen   = _vscprintf(cmd_str, args) + 1;
    char *buffer = (char*)calloc(buflen, sizeof(char));

    if (buffer != NULL){
        vsprintf(buffer, cmd_str, args);
    }

    va_end( args );

    // WinExec(buffer, 0);
    system(buffer);

	free(buffer);

	return;
}

//------------------------------------------------------------------

const char* get_type_string(NODE_TYPE type){
	
	switch(type){
		case INVALID:
			return "invalid";
		case VARIABLE:
			return "variable";
		case CONSTANT:
			return "constant";
		case OPERATION:
			return "operation";
		case FUNCTION:
			return "function";
		default:
			return "invalid";
	}
}
