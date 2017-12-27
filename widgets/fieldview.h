#ifndef FIELDVIEW_H
#define FIELDVIEW_H

#include <QTreeView>
#include <QContextMenuEvent>
#include <QList>

class QUndoStack;
class FieldModel;
class Canvas;
class FieldView : public QTreeView
{
    Q_OBJECT
public:
    FieldView(QWidget* parent = nullptr);

    QUndoStack* getUndoStack() const;
    void setUndoStack(QUndoStack *undoStack);

    void applyValue(QModelIndex &index, bool selection);
    void defineItemCode(QModelIndex &index);

    int *getCurrentPage() const;
    void setCurrentPage(int *currentPage);

    QList<Canvas *> *getCanvasList() const;
    void setCanvasList(QList<Canvas *> *canvasList);

    FieldModel *getModel() const;
    void setFieldModel(FieldModel *model);

public slots:
    void editColor(QModelIndex index);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
private:
    QAction* m_delItem;
    QAction* m_applyValueOnSelection;
    QAction* m_applyValueOnAllLines;
    QAction* m_defineCode;
    QAction* m_resetCode;

    QUndoStack* m_undoStack;
    FieldModel* m_model;
    QList<Canvas*>* m_canvasList;
    int* m_currentPage;
};

#endif // FIELDVIEW_H
