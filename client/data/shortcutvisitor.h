class ShortcutVisitor : public QObject
{
  Q_OBJECT

public:
  ShortcutVisitor(QObject* parent = nullptr);
  ~ShortcutInspector();

  QAbstractModel* getModel() const;
  bool registerWidget(QWidget* widget, const QString& categoryName, bool recursion = false);
  bool unregisterWidget(QWidget* widget);

private slots:
  void objectDeleted(QObject* obj);

private:
  void visit(QWidget* widget, int categoryIndex, bool recursion);

private:
  QAbstractModel* m_model;
  QMap<QWidget*, QString> m_categoriesNames;
};
