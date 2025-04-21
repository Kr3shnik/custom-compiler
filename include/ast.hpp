class ASTNode {
    public:
        virtual void dump(int indent = 0) = 0;
        virtual ~ASTNode() = default;
    };
    
    class IntLiteralNode : public ASTNode {
    public:
        int value;
        IntLiteralNode(int v) : value(v) {}
        void dump(int indent) override {
            std::cout << std::string(indent, ' ') << "Int(" << value << ")\n";
        }
    };
    
// similar for VariableNode, BinaryOpNode, AssignmentNode...
    