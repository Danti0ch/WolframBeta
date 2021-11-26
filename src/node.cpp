#include "node.h"
#include <assert.h>
#include <stdio.h>
#include <cstdlib>
#include <cstdarg>
#include <windows.h>

//------------LOCAL-FUNCTIONS-DECLARATION----------------------------------

Node* make_node(T_Node val, NodeType type);

void generate_dot(Node* nod, char const * file_name);

void write_node_to_dot(Node const * nod, int* n_node, FILE* output_file);

void generate_image(char const * input_file_name, char const * output_file_name);

void execute_term_cmd(char const * cmd_str, ...);

Node* dupl_node(Node* source_node);

const char* get_type_string(NodeType type);

//--------------PUBLIC-FUNCTIONS-DEFINITIONS-----------------------------------------

Node* NodeConstructor(Node* parent, T_Node val, NodeType type, NODE_PLACE place){

	Node* nod = make_node(val, type);
	if(nod == NULL){
		TO_LOG(LOG_TYPE::ERROR, "node creation\n")
		return NULL;
	}

	nod->parent   = parent;

	if(place == NODE_PLACE::LEFT){
		nod->is_left  = true;
		parent->left  = nod;
	}
	else{
		nod->is_left  = false;
		parent->right = nod;
	}

	ASSERT_OK(nod)
	return nod;
}

//------------------------------------------------------------------

void NodeDestructor(Node** nod){

	assert(nod != NULL)
	if(*nod == NULL)  return;
	
	ASSERT_OK(*nod);

	if((*nod)->is_left && (*nod)->parent != NULL){
		(*nod)->parent->left = NULL;
	}

	else if((*nod)->parent != NULL){
		(*nod)->parent->right = NULL;
	}

	free(*nod);

	*nod = NULL;

	return;
}

//------------------------------------------------------------------

Node* CopyNode(Node* parent, Node* source_node, NODE_PLACE place){

	assert(parent 		!= NULL);
	assert(source_node 	!= NULL);

	ASSERT_OK(parent)
	ASSERT_OK(source_node)

	Node* new_node = make_node(source_node->value, source_node->type);

	if(new_node == NULL){
		TO_LOG(LOG_TYPE::ERROR, "node creation\n")
		return NULL;
	}

	if(source_node->left != NULL){
		new_node->left = dupl_node(source_node->left);

		new_node->left->parent  = new_node;
		new_node->left->is_left = true;
	}

	if(source_node->right != NULL){
		new_node->right = dupl_node(source_node->right);

		new_node->right->parent  = new_node;
		new_node->right->is_left = true;
	}
	
	parent->right     = copied_node;
	new_node->parent  = parent;
	new_node->is_left = false;

	ASSERT_OK(new_node)
	return new_node;
}

//------------------------------------------------------------------

Node*  DuplNodeToLeft(Node* parent, Node* source_node){

	assert(parent 		!= NULL);
	assert(source_node 	!= NULL);

	Node* copied_node    = dupl_node(source_node);
	
	parent->left         = copied_node;
	copied_node->parent  = parent;
	copied_node->is_left = true;

	return copied_node;
}

//------------------------------------------------------------------

void MakeTransNode(Node* source_node, T_Node val, NodeType type){

	assert(source_node      != NULL);

	Node* trans_node = make_node(val, type);

	if(source_node->is_left){
		source_node->parent->left = trans_node;
		source_node->is_left      = true;
	}
	else{
		source_node->parent->right = trans_node;
		source_node->is_left       = false;
	}

	trans_node->parent  = source_node->parent;
	trans_node->left    = source_node;
	source_node->parent = trans_node;

	return;
}

//------------------------------------------------------------------

Node* dupl_node(Node* source_node){

	assert(source_node != NULL);

	Node* node = make_node(source_node->value, source_node->type);

	if(source_node->left != NULL){
		node->left = dupl_node(source_node->left);

		node->left->parent  = node;
		node->left->is_left = true;
	}

	if(source_node->right != NULL){
		node->right = dupl_node(source_node->right);

		node->right->parent  = node;
		node->right->is_left = true;
	}
	
	return node;
}

//------------------------------------------------------------------

bool IsLeaf(const Node* nod){

	return nod->left == NULL && nod->right == NULL;
}

//------------------------------------------------------------------

void ShowNode(Node* nod){

	assert(nod != NULL);

	generate_dot(nod, GVIZ_DOT_NAME);
	generate_image(GVIZ_DOT_NAME, DUMP_IMAGE_NAME);
	
	return;
}

//--------------LOCAL-FUNCTIONS-DEFINITIONS-----------------------------------------

Node* make_node(T_Node val, NodeType type){

	Node* nod = (Node*)calloc(1, sizeof(Node));

	if(nod == NULL){
		TO_LOG(LOG_TYPE::ERROR, "mem alloc\n");
		return NULL;
	}

	nod->value = val;
	nod->type  = type;

	return nod;
}

//------------------------------------------------------------------

void generate_dot(Node* nod, char const * file_name){

	assert(nod       != NULL);
	assert(file_name != NULL);

	FILE* tmp_file = fopen(file_name, "w");

	//TODO: log
	assert(tmp_file != NULL);

	fprintf(tmp_file, "digraph Tree{\n");

	int n_node = 0;
	write_node_to_dot(nod, &n_node, tmp_file);

	fprintf(tmp_file, "}\n");

	fclose(tmp_file);

	return;
}

//------------------------------------------------------------------

void write_node_to_dot(Node const * nod, int* n_node, FILE* output_file){

	assert(nod != NULL);
	assert(output_file != NULL);

	int cur_n_node = *n_node;

	char nod_color[20] = "";

	if(IsLeaf(nod))   strcpy(nod_color, "#7FB3D5");
	else	          strcpy(nod_color, "#76D7C4");

	// color fix

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

		else if(nod->type == VARIABLE || nod->type == FUNCTION || nod->type == OPERATION){
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
	#else
		if(nod->type == CONSTANT){
			printf("%d[shape = \"record\", label = \"%.6lg\"]\n", cur_n_node, nod->value);
			fprintf(output_file, "%d[shape = \"record\", label = \"%.6lg\"]\n", cur_n_node, nod->value);
		}

		else if(nod->type == VARIABLE || nod->type == FUNCTION || nod->type == OPERATION){
			printf("%d[shape = \"record\", label = \"%c\"]\n", cur_n_node, (char)nod->value);
			fprintf(output_file, "%d[shape = \"record\", label = \"%c\"];\n", cur_n_node, (char)nod->value);
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

	return;
}

//------------------------------------------------------------------

void generate_image(char const * input_file_name, char const * output_file_name){

	assert(input_file_name != NULL);
	assert(output_file_name     != NULL);
	assert(input_file_name != output_file_name);

	execute_term_cmd("dot -Tpng %s -o %s", input_file_name, output_file_name);

	return;
}

//------------------------------------------------------------------

void execute_term_cmd(char const * cmd_str, ...){

	assert(cmd_str != NULL);

	va_list args;
    va_start(args, cmd_str);

    int buflen = _vscprintf(cmd_str, args) + 1;
    char *buffer = (char*)calloc(buflen, sizeof(char));

    if (buffer != NULL){
        vsprintf(buffer, cmd_str, args);
    }

    va_end( args );

    WinExec(buffer, 0);

	free(buffer);

	return;
}

const char* get_type_string(NodeType type){
	
	switch(type){
		case INVALID:
			return "wtf";
		case VARIABLE:
			return "variable";
		case CONSTANT:
			return "constant";
		case OPERATION:
			return "operation";
		case FUNCTION:
			return "function";
		default:
			printf("some error msg log\n");
			return "wtf";
	}
}