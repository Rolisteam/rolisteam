#ifndef CHARACTER_CONTROLLER_H
#define CHARACTER_CONTROLLER_H

#include <QObject>
#include <QUndoStack>
#include <memory>

#include "charactersheet/charactersheetmodel.h"
#include "data/characterlist.h"

class Section;
class QAction;
class QTreeView;

class CharacterController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CharacterSheetModel* model READ model CONSTANT)
    Q_PROPERTY(CharacterList* characters READ characters CONSTANT)
    Q_PROPERTY(int characterCount READ characterCount NOTIFY characterCountChanged)
public:
    CharacterController(QObject* parent= nullptr);
    virtual ~CharacterController();
    void setRootSection(Section* section);
    void save(QJsonObject& obj, bool);
    void load(const QJsonObject& obj, bool);
    CharacterSheetModel* model() const;
    CharacterList* characters() const;
    int characterCount() const;
    void checkCharacter(Section* sec);
    CharacterSheet* characterSheetFromIndex(int index) const;
    CharacterSheet* characterSheetFromUuid(const QString& uuid) const;

    void clear();

public slots:
    void addCharacter();
    void removeCharacter(int index);
    void insertCharacter(int pos, CharacterSheet* sheet);

    void sendAddCharacterCommand();
    void sendRemoveCharacterCommand(const QModelIndex& index);
    void applyOnSelection(const QModelIndex& index, const QModelIndexList& list);
    void applyOnAllCharacter(const QModelIndex& index);

signals:
    void characterCountChanged(int csCount);
    void dataChanged();
    void performCommand(QUndoCommand* cmd);

private:
    std::unique_ptr<CharacterSheetModel> m_characterModel;
    std::unique_ptr<CharacterList> m_characters;
};

#endif
