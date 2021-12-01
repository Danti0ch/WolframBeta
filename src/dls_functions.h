#define MAKE_MUL(parent, mul1, mul2, place)					\
{															\
	NodeConstructor(parent, '*', NODE_TYPE::OPERATION, place);			\
	if(place == NODE_PLACE::LEFT){							\
		CopyNode(parent->left, mul1, NODE_PLACE::LEFT);		\
		CopyNode(parent->left, mul2, NODE_PLACE::RIGHT);	\
	}														\
	else{													\
		CopyNode(parent->right, mul1, NODE_PLACE::LEFT);	\
		CopyNode(parent->right, mul2, NODE_PLACE::RIGHT);	\
	}														\
}
