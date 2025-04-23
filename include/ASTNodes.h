#ifndef __ASTNODES_H__
#define __ASTNODES_H__

class NBlock;
class NStatement;
class NExpression;
class NVariableDeclaration;
class Node;


class Node{
    public:
        virtual ~Node() {} 
};

class NStatement : public Node {

};
class NExpression : public Node {

};
class NVariableDeclaration : public Node {

};


#endif