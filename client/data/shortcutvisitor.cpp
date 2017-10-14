#include "shortcutvisitor.h"
#include "shortcutmodel.h"

#include <QWidget>
#include <QList>
#include <QAction>

///////////////////////// CPP
ShortcutVisitor::ShortcutVisitor(QObject* parent)
    : QObject(parent)
    , m_model(new ShortCutModel())
{}

ShortcutVisitor::~ShortcutVisitor()
{}

QAbstractItemModel *ShortcutVisitor::getModel() const
{
    return m_model;
}

bool ShortcutVisitor::registerWidget(QWidget* widget, const QString& categoryName, bool recursion)
{
    if(m_categoriesNames.find(widget) != m_categoriesNames.end())
        return false; // widget already registered

    const QString& categoryIndex = m_categoriesNames[widget];

    if(categoryIndex == -1)
        return false; // Error during creation (category name already used)

    m_categoriesNames[widget] = categoryName;

    visit(widget, categoryIndex, recursion);
    //connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(onWidgetDeleted(QObject*)));
    return true;
}

bool ShortcutVisitor::unregisterWidget(QWidget* widget)
{
    if(m_categoriesNames.find(widget) == m_categoriesNames.end())
        return false;

    const QString& categoryIndex = m_categoriesNames[widget];

    if(categoryIndex == -1)
        return false;

   // disconnect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(objectDeleted(QObject*)));
  //  m_model->removeCategory(categoryIndex);
    return true;
}

void ShortcutVisitor::objectDeleted(QObject* obj)
{
    QWidget* widget = qobject_cast<QWidget*>(obj);
    if(nullptr != widget)
    {
        unregisterWidget(widget);
    }
}
#include <QDebug>
void ShortcutVisitor::visit(QWidget* widget,const QString& category, bool recursion)
{
    QList<QAction*> actions = widget->actions();
    if(!actions.isEmpty())
    {
        m_model->addCategory(category);
    }
    for(QAction* action : actions)
    {
        //if(!action->shortcut().isEmpty())
        {
            QString actionName = action->text();
            qDebug() << actionName << action->shortcut().toString(QKeySequence::NativeText);
            actionName.remove('&');
            m_model->insertShortCut(category, actionName,action->shortcut().toString(QKeySequence::NativeText));
        }
    }

    if(recursion)
    {
        QObjectList children = widget->children();
        for(QObject* obj : children)
        {
            QWidget* wd = qobject_cast<QWidget*>(obj);
            if(nullptr != wd)
            {
                visit(wd, wd->objectName(), recursion);
            }
        }
    }
}
