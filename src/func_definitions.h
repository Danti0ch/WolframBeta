// DEF_FUNC(id, name, dif_code, val_code)
#include "dsl.h"

DEF_FUNC(1, sin, {
	return MUL(NodeCtor(2, NODE_TYPE::FUNCTION, L), DIFF(L));
},
{
	return sin(get_value_from_node(node->left, arg));
})

DEF_FUNC(2, cos, {
	return MUL(MUL(NodeCtor(-1.0, NODE_TYPE::CONSTANT), NodeCtor(1, NODE_TYPE::FUNCTION, L)), DIFF(L));
},
{
	return cos(get_value_from_node(node->left, arg));
})

DEF_FUNC(3, ln, {
	return MUL(DIV(NodeCtor(1.0, NODE_TYPE::CONSTANT), L), DIFF(L));
},
{
	return log(get_value_from_node(node->left, arg));
})

DEF_FUNC(4, exp, {
	return MUL(NodeCtor(4, NODE_TYPE::FUNCTION, L), DIFF(L));
},
{
	return exp(get_value_from_node(node->left, arg));
})
