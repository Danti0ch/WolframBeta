#include "expression.h"
#include <assert.h>
#include <stdio.h>
#include <cstdlib>

static void create_buf(String* buffer, FILE* fp);

static void destroy_buf(String* buffer);

static bool is_operation(char c);

static bool is_variable(char c);

//_____________________________________________________________________________

#define DEF_FUNC(id, name, code)							\
{															\
	if(strcmp(function_name, #name) == 0){					\
		AssignVal(cur_node, (id), NODE_TYPE::FUNCTION);		\
		n_cur_symb += n_readen;								\
		continue;											\
	}														\
}

void ReadExpr(FILE* f_stream, Expr* expr){

	assert(expr     != NULL);
	assert(f_stream != NULL);

	String buffer = {};
	create_buf(&buffer, f_stream);

	int    n_cur_symb  = 0;
	int    n_readen    = 0;

	T_Node value       = {};
	Node*  cur_node    = expr->root;

	char function_name[MAX_FUNC_LEN] = "";

	while(n_cur_symb < buffer.size){

		// пропускаем пробельные символы
		sscanf((char*)buffer.data + n_cur_symb, " %n", &n_readen);

		n_cur_symb += n_readen;

		if(n_cur_symb >= buffer.size) break;

		if(buffer.data[n_cur_symb] == '('){
			if(!IsValid(cur_node->left)){
				cur_node = NodeCtor(cur_node, VAL_POISON, NODE_TYPE::INVALID, NODE_PLACE::LEFT);
			}

			else{
				cur_node = NodeCtor(cur_node, VAL_POISON, NODE_TYPE::INVALID, NODE_PLACE::RIGHT);
			}

			n_cur_symb++;
			continue;
		}

		else if(buffer.data[n_cur_symb] == ')'){
			cur_node = cur_node->parent;

			n_cur_symb++;
			continue;
		}

		if(sscanf((char*)buffer.data + n_cur_symb, "%lg%n", &(value.const_val), &n_readen) == 1){
			AssignVal(cur_node, value, NODE_TYPE::CONSTANT);
			n_cur_symb += n_readen;
			continue;
		}

		if(sscanf((char*)buffer.data + n_cur_symb, "%[^(]%n", function_name) == 1){

			// КОДОГЕНЕРАЦИЯ
			#include "func_definitions.h"
			// КОДОГЕНЕРАЦИЯ
		}

		if(sscanf((char*)buffer.data + n_cur_symb, "%c%n", &(value.symb)) == 1){
			if(is_operation(value.symb)){
				AssignVal(cur_node, value, NODE_TYPE::OPERATION);

				n_cur_symb += n_readen;
				continue;
			}

			if(is_variable(value.symb)){
				AssignVal(cur_node, value, NODE_TYPE::VARIABLE);

				n_cur_symb += n_readen;
				continue;
			}
		}

		// если ничего не распознано
		DestrExpr(expr);

		ToLog(LOG_TYPE::ERROR, "Expression can't be readen, check the %d symbol", n_cur_symb);
		return;
	}
}

#undef DEF_FUNC
//_____________________________________________________________________________

static void create_buf(String* buffer, FILE* fp){

	assert(buffer != NULL);
	assert(fp     != NULL);

	buffer->data = (char*)calloc(MAX_BUF_SIZE, sizeof(char));

	fgets(buffer->data, MAX_BUF_SIZE, fp);
	buffer->size = strlen(buffer->data);

	ToLog(LOG_TYPE::INFO, "buffer has been read\n"
						  "\tbuffer.data: %s\n"
						  "\tbuffer.size: %d", buffer->data, buffer->size);
	return;
}

static void destroy_buf(String* buffer){
	free(buffer->data);
	buffer->size = -1;
}
//_____________________________________________________________________________

static bool is_operation(char c){

	for(int i = 0; i < N_OPERATIONS; i++){
		if(OPERATIONS_SYMB[i] == c){
			return true;
		}
	}

	return false;
}
//_____________________________________________________________________________

static bool is_variable(char c){

	return (c >= 'a' &&  c <= 'z') || (c >= 'A' && c <= 'Z'); 
}
//_____________________________________________________________________________
