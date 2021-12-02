#define DIFF(node)								\
NodeDif((node))									

#define ADD(L, R)								\
NodeCtor('+', NODE_TYPE::OPERATOR, (L), (R));	

#define MUL(L, R)								\
NodeCtor('*', NODE_TYPE::OPERATOR, (L), (R)); 

#define DIV(L, R)								\
NodeCtor('/', NODE_TYPE::OPERATOR, (L), (R));

#define MUL