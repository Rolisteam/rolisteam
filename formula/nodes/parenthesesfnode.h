#ifndef PARENTHESESFNODE_H
#define PARENTHESESFNODE_H

#include "formulanode.h"
namespace Formula
{
class ParenthesesFNode : public FormulaNode
{
public:
    ParenthesesFNode();

    bool run(FormulaNode *previous);

    FormulaNode *internalNode() const;

    void setInternalNode(FormulaNode *internalNode);
    virtual int getPriority();
private:
    FormulaNode* m_internalNode;
    QVariant m_result;
};
}
#endif // PARENTHESESFNODE_H
