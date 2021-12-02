#define L node->left
#define R node->right

#define DIFF(node)								\
NodeDif((node))									

#define ADD(L, R)								\
NodeCtor('+', NODE_TYPE::OPERATION, (L), (R))	

#define SUB(L, R)								\
NodeCtor('-', NODE_TYPE::OPERATION, (L), (R))	

#define MUL(L, R)								\
NodeCtor('*', NODE_TYPE::OPERATION, (L), (R)) 

#define DIV(L, R)								\
NodeCtor('/', NODE_TYPE::OPERATION, (L), (R))

#define POW(L, R)								\
NodeCtor('^', NODE_TYPE::OPERATION, (L), (R))
