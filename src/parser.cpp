#include "expression.h"
#include <assert.h>
#include <stdio.h>
#include <cstdlib>
#include <ctype.h>
#include <stdlib.h>

char* cur_symb = NULL;

Node* GetG(char* input_str);
Node* GetN();
Node* GetE();
Node* GetT();
Node* GetP();
Node* GetPow();
Node* GetFunc();

void require(char c);

static void create_buf(String* buffer, FILE* fp);

static void destroy_buf(String* buffer);

//_____________________________________________________________________________

void ReadExpr(FILE* f_stream, Expr* expr){

	assert(expr     != NULL);
	assert(f_stream != NULL);

	String buffer = {};
	create_buf(&buffer, f_stream);

	Node* expr_root = GetG(buffer.data);
	MakeConnection(expr->root, expr_root, NODE_PLACE::LEFT);

	destroy_buf(&buffer);
	return;
}
//_____________________________________________________________________________

static void create_buf(String* buffer, FILE* fp){

	assert(buffer != NULL);
	assert(fp     != NULL);

	buffer->data = (char*)calloc(MAX_BUF_SIZE, sizeof(char));

	fgets(buffer->data, MAX_BUF_SIZE, fp);

	buffer->size = strlen(buffer->data);
	
	char *buf_symb = buffer->data;
	int cur_pos    = 0;
	
	while(buf_symb < buffer->data + buffer->size){

		if(!isspace(*buf_symb)){
			buffer->data[cur_pos] = *buf_symb;
			cur_pos++;
		}
		buf_symb++;
	}
	buffer->data[cur_pos] = '\0';
	buffer->size = cur_pos + 1;

	ToLog(LOG_TYPE::INFO, "buffer has been read\n"
						  "\tbuffer.data: %s\n"
						  "\tbuffer.size: %d", buffer->data, buffer->size);
	return;
}

static void destroy_buf(String* buffer){
	free(buffer->data);
	buffer->size = -1;
}
//__________________________________________________________________

Node* GetG(char* input_str){

    assert(input_str != NULL);

    cur_symb = (char*)input_str;
    
    Node* general_node = GetE();

    require('\0');
    
    cur_symb++;

    return general_node;
}
//__________________________________________________________________

Node* GetN(){

    double val = 0;

    if(*cur_symb == 'x'){
        cur_symb++;
        return NodeCtor('x', NODE_TYPE::VARIABLE);
    }

    while('0' <= *cur_symb && *cur_symb <= '9'){
        val = val * 10.0 + (*cur_symb - '0');
        cur_symb++;
    }
    if(*cur_symb == '.'){
        cur_symb++;

        double mod_value = 0.1;

        while('0' <= *cur_symb && *cur_symb <= '9'){
            val += mod_value * (*cur_symb - '0');
            mod_value /= 10.0;
            cur_symb++;
        }
    }

    return NodeCtor(val, NODE_TYPE::CONSTANT);
}
//__________________________________________________________________


Node* GetE(){
    
    Node* parent = GetT();

    while(*cur_symb == '+' || *cur_symb == '-'){

        char op_symb = *cur_symb;
        cur_symb++;

        Node* r_node = GetT();

        if(op_symb == '+'){
            parent = NodeCtor('+', NODE_TYPE::OPERATION, parent, r_node);
        }
        else{
            parent = NodeCtor('-', NODE_TYPE::OPERATION, parent, r_node);            
        }
    }
    return parent;
}
//__________________________________________________________________

Node* GetT(){

    Node* parent = GetPow();

    while(*cur_symb == '*' || *cur_symb == '/' || *cur_symb == '^'){

        char op_symb = *cur_symb;
        cur_symb++;

        Node* r_node = GetPow();

        if(op_symb == '*'){
            parent = NodeCtor('*', NODE_TYPE::OPERATION, parent, r_node);
        }
        else{
            parent = NodeCtor('/', NODE_TYPE::OPERATION, parent, r_node);            
        }
    }

    return parent;
}
//__________________________________________________________________

Node* GetPow(){

    Node* parent = GetP();

    while(*cur_symb == '^'){

    	cur_symb++;
        Node* r_node = GetP();

        parent = NodeCtor('^', NODE_TYPE::OPERATION, parent, r_node);
    }

    return parent;
}
//__________________________________________________________________

Node* GetP(){

    if(*cur_symb == '('){
        cur_symb++;
        
        Node* node = GetE();

        require(')');
        cur_symb++;

        return node;
    }
	
	if (isalpha(*cur_symb)){
	    if (isalpha(*(cur_symb + 1))){
	        
	        Node* func_node = GetFunc();
	        Node* arg_node  = GetP();
	        
	        MakeConnection(func_node, arg_node, NODE_PLACE::LEFT);
	        return func_node;
	    }
	    else return GetN();
	}

    return GetN();
}
//__________________________________________________________________

#define DEF_FUNC(id, name, dif_code, val_code)              \
{                                                           \
    if(strcmp(function_name, #name) == 0){                  \
    	func_id = id;										\
    }                                                       \
}

Node* GetFunc(){

	char function_name[MAX_BUF_SIZE] = "";
	int n_readen = 0;

	sscanf(cur_symb, "%[^(]%n", function_name, &n_readen);
	cur_symb += n_readen;

	int func_id = -1;

	// КОДОГЕНЕРАЦИЯ
	#include "func_definitions.h"
	// КОДОГЕНЕРАЦИЯ

	return NodeCtor(func_id, NODE_TYPE::FUNCTION);	
}

#undef DEF_FUNC
//__________________________________________________________________

void require(char c){
    if(*cur_symb != c){
        exit(0);
    }
    return;
}
//__________________________________________________________________
