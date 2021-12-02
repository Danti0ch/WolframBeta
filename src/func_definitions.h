// DEF_FUNC(id, name, code)
#include "dsl.h"

DEF_FUNC(1, sin, {
	return MUL(NodeCtor(2, NODE_TYPE::FUNCTION, L), DIFF(L));
})

DEF_FUNC(2, cos, {
	return MUL(MUL(NodeCtor(-1.0, NODE_TYPE::CONSTANT), NodeCtor(1, NODE_TYPE::FUNCTION, L)), DIFF(L));
})

DEF_FUNC(3, ln, {
	return MUL(DIV(NodeCtor(1.0, NODE_TYPE::CONSTANT), L), DIFF(L));
})

DEF_FUNC(4, exp, {
	return MUL(NodeCtor(4, NODE_TYPE::FUNCTION, L), DIFF(L));
})