
///////////////////////// CPP
ShortcutVisitor::ShortcutVisitor(QObject* parent)
  : QObject(parent)
  , m_model(new ShortcutModel(this))
{}

ShortcutVisitor::~ShortcutVisitor()
{}

bool ShortcutVisitor::registerWidget(
    QWidget* widget, const QString& categoryName, bool recursion)
{
  if(a_categoriesNames.find(widget) != a_categoriesNames.end())
    return false; // widget already registered

  int categoryIndex = a_model->addCategory(categoryName);

  if(categoryIndex == -1)
    return false; // Error during creation (category name already used)

  a_categoriesNames[widget] = categoryName;

  visit(widget, categoryIndex, recursion);
  connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(onWidgetDeleted(QObject*)));
  return true;
}

bool ShortcutVisitor::unregisterWidget(QWidget* widget)
{
  if(m_categoriesNames.find(widget) == m_categoriesNames.end())
    return false;

  int categoryIndex = m_model->indexOfCategory(m_categoriesNames[widget]);

  if(categoryIndex == -1)
    return false;

  disconnect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(objectDeleted(QObject*)));
  m_model->removeCategory(categoryIndex);
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

void ShortcutVisitor::visit(QWidget* widget, int categoryIndex, bool recursion)
{
  QList<QAction*> actions = widget->actions();
  for(QAction* action : actions)
  {
    if(!action->shortcut().isEmpty())
    {
      QString actionName = action->text();
      actionName.remove('&');
      m_model->addAction(categoryIndex, actionName,
                         action->shortcut().toString(QKeySequence::NativeText));
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
        visit(wd, categoryIndex, recursion);
        }
    }
  }
}
