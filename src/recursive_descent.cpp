#include "expression.h"
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

char* cur_symb = NULL;

Node* GetN();
Node* GetE();
Node* GetT();
Node* GetP();

void require(char c);

//__________________________________________________________________

Node* GetG(const char* input_str){

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

Node* GetT(){

    Node* parent = GetP();

    while(*cur_symb == '*' || *cur_symb == '/'){

        char op_symb = *cur_symb;
        cur_symb++;

        Node* r_node = GetP();

        if(op_symb == '*'){
            parent = NodeCtor('*', NODE_TYPE::OPERATION, parent, r_node);
        }
        else{
            parent = NodeCtor('/', NODE_TYPE::OPERATION, parent, r_node);            
        }
    }

    return parent;
}

Node* GetP(){

    if(*cur_symb == '('){
        cur_symb++;
        
        Node* node = GetE();

        require(')');
        cur_symb++;

        return node;
    }
    else{
        return GetN();
    }
}

void require(char c){
    if(*cur_symb != c){
        exit(0);
    }
    return;
}
