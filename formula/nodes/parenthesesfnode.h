#ifndef PARENTHESESFNODE_H
#define PARENTHESESFNODE_H

#include "formulanode.h"
namespace Formula
{
/**
 * @brief The ParenthesesFNode class
 */
class ParenthesesFNode : public FormulaNode
{
public:
    ParenthesesFNode();

    bool run(FormulaNode *previous);

    FormulaNode *internalNode() const;

    void setInternalNode(FormulaNode *internalNode);
    virtual int getPriority();

    virtual QVariant getResult();
private:
    FormulaNode* m_internalNode;
    QVariant m_result;
};
}
#endif // PARENTHESESFNODE_H
