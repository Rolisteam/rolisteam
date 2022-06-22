#include "parenthesesfnode.h"
namespace Formula
{
    ParenthesesFNode::ParenthesesFNode() {}

    FormulaNode* ParenthesesFNode::internalNode() const { return m_internalNode; }

    void ParenthesesFNode::setInternalNode(FormulaNode* internalNode) { m_internalNode= internalNode; }
    QVariant ParenthesesFNode::getResult() { return m_result; }

    bool ParenthesesFNode::run(FormulaNode* /*previous*/)
    {
        bool result= false;
        if(nullptr != m_internalNode)
        {
            m_internalNode->run(this);
            FormulaNode* temp= m_internalNode;
            while(nullptr != temp->next())
            {
                temp= temp->next();
            }
            m_result= temp->getResult();
            result= true;
        }

        if(nullptr != m_next)
        {
            return m_next->run(this);
        }
        else
        {
            return result;
        }
    }
    int ParenthesesFNode::getPriority() { return 3; }

} // namespace Formula
