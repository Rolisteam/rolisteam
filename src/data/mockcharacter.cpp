#include "mockcharacter.h"

#include <QEvent>
#include <QVariant>

MockCharacter::MockCharacter(QObject* parent) : QObject(parent)
{
    m_properties= {{"healthPoints", "100"},
                   {"maxHP", "100"},
                   {"minHP", "0"},
                   {"avatarPath", tr("unset Path")},
                   {"isNpc", "false"},
                   {"initiative", "100"},
                   {"distancePerTurn", "25.5"},
                   {"state", "15"},
                   {"currentShape", "-1"},
                   {"lifeColor", tr("green")},
                   {"initCommand", ""},
                   {"hasInitiative", "false"},
                   {"name", "Character"},
                   {"color", tr("red")},
                   {"avatar", ""}};

    for(auto pair : m_properties)
    {
        setProperty(pair.first.toStdString().c_str(), pair.second);
    }
}

bool MockCharacter::event(QEvent* event)
{
    if(event->type() == QEvent::DynamicPropertyChange)
    {
        auto changeEv= dynamic_cast<QDynamicPropertyChangeEvent*>(event);
        emit dataChanged(QString::fromUtf8(changeEv->propertyName()));
    }
    return QObject::event(event);
}

void MockCharacter::setCurrentShape(int index)
{
    Q_UNUSED(index)
    setProperty("currentShape", index);
    // emit dataChanged(QStringLiteral("currentShape").arg(index), LogController::Features);
}
void MockCharacter::addShape(const QString& name, const QString& path)
{
    Q_UNUSED(name)
    Q_UNUSED(path)
    emit log(tr("Shape has been added [name: %1, path: %2]").arg(name, path));
}
