#ifndef CHARACTER_CONTROLLER_H
#define CHARACTER_CONTROLLER_H

#include <QObject>
#include <memory>

class CharacterController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractModel* model READ model CONTANST)
public:
    CharacterController();

    void setRootSection(Section* section);

    void save(QJsonObject obj, bool)
    void load(QJsonObject obj, bool);

    CharacterSheetModel* model() const;

protected slots:
    void contextMenu(const QPoint& pos);
    



private:
    QAction* m_addCharacter=nullptr;
    QAction* m_deleteCharacter=nullptr;
    QAction* m_copyCharacter=nullptr;
    QAction* m_defineAsTabName=nullptr;
    QAction* m_applyValueOnAllCharacterLines=nullptr;
    QAction* m_applyValueOnSelectedCharacterLines=nullptr;
    QAction* m_applyValueOnAllCharacters=nullptr;
    std::unique_ptr<CharacterSheetModel> m_characterModel;
};


#endif
