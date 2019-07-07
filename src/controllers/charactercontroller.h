#ifndef CHARACTER_CONTROLLER_H
#define CHARACTER_CONTROLLER_H

#include <QObject>
#include <QUndoStack>
#include <memory>

#include "charactersheetmodel.h"

class Section;
class QAction;
class QTreeView;
class QAbstractItemModel;

class CharacterController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model CONSTANT)
    Q_PROPERTY(int characterCount READ characterCount NOTIFY characterCountChanged)
public:
    CharacterController(QUndoStack& undoStack, QTreeView* view, QObject* parent= nullptr);
    void setRootSection(Section* section);
    void save(QJsonObject& obj, bool);
    void load(const QJsonObject& obj, bool);
    QAbstractItemModel* model() const;
    int characterCount() const;
    void checkCharacter(Section* sec);
    CharacterSheet* characterSheetFromIndex(int index) const;

    void clear();
public slots:
    void addCharacter();
    void removeCharacter(int index);
    void insertCharacter(int pos, CharacterSheet* sheet);

    void sendAddCharacterCommand();
    void sendRemoveCharacterCommand();

signals:
    void characterCountChanged(int csCount);
    void dataChanged();

protected:
    void applyOnSelection();
    void applyOnAllCharacter();

protected slots:
    void contextMenu(const QPoint& pos);

private:
    QTreeView* m_view;
    QUndoStack& m_undoStack;
    QAction* m_addCharacter= nullptr;
    QAction* m_deleteCharacter= nullptr;
    QAction* m_copyCharacter= nullptr;
    QAction* m_defineAsTabName= nullptr;
    QAction* m_applyValueOnSelectedCharacterLines= nullptr;
    QAction* m_applyValueOnAllCharacters= nullptr;
    std::unique_ptr<CharacterSheetModel> m_characterModel;
};

#endif
