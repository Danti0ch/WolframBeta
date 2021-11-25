
#define MAKE_LEFT_MUL(parent, mul1, mul2)		\
{												\
	MakeLeftNode(parent, '*', OPERATION);		\
	DuplNodeToLeft(parent->left, mul1);			\
	DuplNodeToRight(parent->right, mul2);		\
}

#define MAKE_RIGHT_MUL(parent, mul1, mul2)		\
{												\
	MakeRightNode(parent, '*', OPERATION);		\
	DuplNodeToLeft(parent->left, mul1);			\
	DuplNodeToRight(parent->right, mul2);		\
}
