#ifndef PARENTHESESFNODE_H
#define PARENTHESESFNODE_H

#include "charactersheet_formula/formula_global.h"
#include "formulanode.h"
namespace Formula
{
/**
 * @brief The ParenthesesFNode class
 */
class CHARACTERSHEET_FORMULA_EXPORT ParenthesesFNode : public FormulaNode
{
public:
    ParenthesesFNode();

    bool run(FormulaNode* previous);

    FormulaNode* internalNode() const;

    void setInternalNode(FormulaNode* internalNode);
    virtual int getPriority();

    virtual QVariant getResult();

private:
    FormulaNode* m_internalNode;
    QVariant m_result;
};
} // namespace Formula
#endif // PARENTHESESFNODE_H
