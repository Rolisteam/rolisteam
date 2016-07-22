#include "parenthesesfnode.h"
namespace Formula
{
ParenthesesFNode::ParenthesesFNode()
{

}

FormulaNode *ParenthesesFNode::internalNode() const
{
    return m_internalNode;
}

void ParenthesesFNode::setInternalNode(FormulaNode *internalNode)
{
    m_internalNode = internalNode;
}
bool ParenthesesFNode::run(FormulaNode* /*previous*/)
{
    bool result = false;
    if(NULL!=m_internalNode)
    {
        m_internalNode->run(this);
        FormulaNode* temp=m_internalNode;
       while(NULL!=temp->next())
       {
            temp=temp->next();
       }
       m_result = temp->getResult();
       result = true;
    }


    if(NULL!=m_next)
    {
        return m_next->run(this);
    }
    else
    {
        return result;
    }
}
int ParenthesesFNode::getPriority()
{
    return 3;
}

}
