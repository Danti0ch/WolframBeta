#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstdarg>
#include <assert.h>

static int n_img_file = 0;

/**
 * создаёт dot файл с информацией об узле node и его потомкаха
 */
static void generate_dot(Node* node, char const * file_name);

/**
 * записывает в дот файл узел node со всеми потомками
 */
static void write_node_to_dot(Node const * nod, int* n_node, FILE* output_file);

/**
 * создаёт картинку для дот файла
 *
 * \param input_file_name имя дот файлa
 * \param output_file_name имя картинки
 */
static void generate_image(char const * input_file_name, char const * output_file_name);

/**
 * выполняет команду, указанную в строку cmd_str в консоли
 */
static void execute_term_cmd(char const * cmd_str, ...);

/**
 * конвертируют код типа узла в строку
 */
static const char* get_type_string(NODE_TYPE type);

///--------------LOCAL-FUNCTIONS-DEFINITIONS-----------------------------------------

void ShowNode(Node* node){

	assert(node != NULL);

	char n_img_string[MAX_IMG_N_STRING_LEN] = "";
	sprintf(n_img_string, "%d", n_img_file);

	char image_name[FILE_NAME_LEN] = "";
	strcpy(image_name, DUMP_IMAGE_NAME);
	strcat(image_name, n_img_string);
	strcat(image_name, ".png");

	generate_dot(node, GVIZ_DOT_NAME);
	generate_image(GVIZ_DOT_NAME, image_name);

	char full_path[200] =  "C:/Users/79162/Desktop/code/mipt_projects/WolframBeta/dumps/";

	system(strcat(full_path, image_name));

	n_img_file++;

	ToLog(LOG_TYPE::INFO, "showed Node %p", node);	
	return;
}
//__________________________________________________________________

static void generate_dot(Node* node, char const * file_name){

	assert(node      != NULL);
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
//__________________________________________________________________

static void write_node_to_dot(Node const * node, int* n_node, FILE* output_file){

	assert(node        != NULL);
	assert(output_file != NULL);

	int cur_n_node = *n_node;

	#if DUMP_MODE == DETAILED_DUMP
		if(IsConstant(node)){
			fprintf(output_file, "%d[style = \"filled\", fillcolor = \"#E6F099\", ", cur_n_node);
		}
		else if(IsVariable(node) || IsOperation(node)){
			fprintf(output_file, "%d[style = \"filled\", fillcolor = \"#6E90CA\", ", cur_n_node);
		}
		else if(IsOperation(node)){
			fprintf(output_file, "%d[style = \"filled\", fillcolor = \"#CA6EC2\", ", cur_n_node);			
		}
		else if(IsFunction(node)){
			fprintf(output_file, "%d[style = \"filled\", fillcolor = \"#6ECA7B\", ", cur_n_node);
		}
		else{
			ToLog(LOG_TYPE::ERROR, "try to dump invalid node(Node %p)", node);
			return;
		}
		fprintf(output_file, "shape = \"record\", label = \"{"
								  "adress:  %p |"
								  "parent:  %p |"
								  "left:    %p |"
								  "right:   %p |"
								  "place:   %s |"
								  "type:    %s |",
								  node,
								  node->parent,
								  node->left,
								  node->right,
								  IsLeft(node) ? "left" : "right",
								  get_type_string(node->type));

		if(IsConstant(node)){
			fprintf(output_file, "value:   %.6lg}\"];\n", node->value);
		}
		else if(IsVariable(node) || IsOperation(node)){
			fprintf(output_file, "value:   %c}\"];\n", node->value);
		}
		else if(IsFunction(node)){
			fprintf(output_file, "value:   %s}\"];\n", GetFuncName(node->value.func_id));
		}
		else{
			ToLog(LOG_TYPE::ERROR, "try to dump invalid node(Node %p)", node);
			return;
		}
	#else
		if(IsConstant(node)){
			fprintf(output_file, "%d[style = \"filled\", fillcolor = \"#E6F099\", shape = \"record\", label = \"%.3g\"]\n", cur_n_node, node->value.const_val);
		}

		else if(IsVariable(node)){
			fprintf(output_file, "%d[style = \"filled\", fillcolor = \"#6E90CA\", shape = \"record\", label = \"%c\"];\n", cur_n_node, node->value.symb);
		}
		else if(IsOperation(node)){
			fprintf(output_file, "%d[style = \"filled\", fillcolor = \"#CA6EC2\", shape = \"record\", label = \"%c\"];\n", cur_n_node, node->value.func_id);			
		}
		else if(IsFunction(node)){
			fprintf(output_file, "%d[style = \"filled\", fillcolor = \"#6ECA7B\", shape = \"record\", label = \"%s\"];\n", cur_n_node, GetFuncName(node->value.func_id));
		}
		else{
			ToLog(LOG_TYPE::ERROR, "try to dump invalid node(Node %p)", node);
			return;
		}
	#endif //DUMP_MODE == DETAILED_DUMP

	if(IsValid(node->left)){
		(*n_node)++;
		fprintf(output_file, "%d->%d;\n", cur_n_node, *n_node);
		write_node_to_dot(node->left, n_node, output_file);
	}

	if(IsValid(node->right)){
		(*n_node)++;
		fprintf(output_file, "%d->%d;\n", cur_n_node, *n_node);
		write_node_to_dot(node->right, n_node, output_file);
	}

	ToLog(LOG_TYPE::INFO, "Node %p was written to file", node);
	return;
}
//__________________________________________________________________

static void generate_image(char const * input_file_name, char const * output_file_name){

	assert(input_file_name   != NULL);
	assert(output_file_name  != NULL);
	assert(input_file_name   != output_file_name);

	execute_term_cmd("dot -Tpng %s -o ../dumps/%s", input_file_name, output_file_name);

	ToLog(LOG_TYPE::INFO, "Image %s has showen", output_file_name);
	return;
}
//__________________________________________________________________

void execute_term_cmd(char const * cmd_str, ...){

	assert(cmd_str != NULL);

	va_list args;
    va_start(args, cmd_str);

    int buflen   = 500; //v(cmd_str, args) + 1;
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
//__________________________________________________________________

static const char* get_type_string(NODE_TYPE type){
	
	switch(type){
		case NODE_TYPE::INVALID:
			return "invalid";
		case NODE_TYPE::VARIABLE:
			return "variable";
		case NODE_TYPE::CONSTANT:
			return "constant";
		case NODE_TYPE::OPERATION:
			return "operation";
		case NODE_TYPE::FUNCTION:
			return "function";
		default:
			return "invalid";
	}
}
//__________________________________________________________________
