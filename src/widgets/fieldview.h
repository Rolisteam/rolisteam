#ifndef FIELDVIEW_H
#define FIELDVIEW_H

#include <QContextMenuEvent>
#include <QList>
#include <QSignalMapper>
#include <QTreeView>

class QUndoStack;
class FieldModel;
class Canvas;
class FieldView : public QTreeView
{
    Q_OBJECT
public:
    FieldView(QWidget* parent= nullptr);

    QUndoStack* getUndoStack() const;
    void setUndoStack(QUndoStack* undoStack);

    void applyValue(QModelIndex& index, bool selection);
    void defineItemCode(QModelIndex& index);

    int getCurrentPage() const;

    QList<Canvas*>* getCanvasList() const;
    void setCanvasList(QList<Canvas*>* canvasList);

    FieldModel* getModel() const;
    void setFieldModel(FieldModel* model);

public slots:
    void editColor(QModelIndex index);
    void hideAllColumns(bool);
    void setCurrentPage(int currentPage);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    QAction* m_delItem= nullptr;
    QAction* m_applyValueOnSelection= nullptr;
    QAction* m_applyValueOnAllLines= nullptr;
    QAction* m_defineCode= nullptr;
    QAction* m_resetCode= nullptr;

    // Show Hide Columns
    QAction* m_showGeometryGroup= nullptr;
    QAction* m_showEsteticGroup= nullptr;
    QAction* m_showValueGroup= nullptr;
    QAction* m_showAllGroup= nullptr;
    QAction* m_showIdGroup= nullptr;

    QUndoStack* m_undoStack= nullptr;
    FieldModel* m_model= nullptr;
    QList<Canvas*>* m_canvasList= nullptr;
    int m_currentPage= 0;

    QSignalMapper* m_mapper= nullptr;
};

#endif // FIELDVIEW_H
