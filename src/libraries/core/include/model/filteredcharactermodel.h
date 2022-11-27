#ifndef FILTEREDCHARACTERMODEL_H
#define FILTEREDCHARACTERMODEL_H

#include <QSortFilterProxyModel>
#include "core_global.h"
#include "model/nonplayablecharactermodel.h"

namespace campaign
{
class CORE_EXPORT FilteredCharacterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    // clang-format off
    Q_PROPERTY(QString search READ search WRITE setSearch NOTIFY searchChanged)
    Q_PROPERTY(bool advanced READ advanced WRITE setAdvanced NOTIFY advancedChanged)
    Q_PROPERTY(QString exclude READ exclude WRITE setExclude NOTIFY excludeChanged)
    Q_PROPERTY(QString tags READ tags WRITE setTags NOTIFY tagsChanged)
    Q_PROPERTY(QString excludeTags READ excludeTags WRITE setExcludeTags NOTIFY excludeTagsChanged)
    Q_PROPERTY(Definition avatarDefinition READ avatarDefinition WRITE setAvatarDefinition NOTIFY avatarDefinitionChanged)
    Q_PROPERTY(Definition initiativeScoreDef READ initiativeScoreDef WRITE setInitiativeScoreDef NOTIFY initiativeScoreDefChanged)
    Q_PROPERTY(Definition actionDef READ actionDef WRITE setActionDef NOTIFY actionDefChanged)
    Q_PROPERTY(Definition shapeDef READ shapeDef WRITE setShapeDef NOTIFY shapeDefChanged)
    Q_PROPERTY(Definition propertiesDef READ propertiesDef WRITE setPropertiesDef NOTIFY propertiesDefChanged)
    Q_PROPERTY(Definition initiativeCmdDef READ initiativeCmdDef WRITE setInitiativeCmdDef NOTIFY initiativeCmdDefChanged)
    Q_PROPERTY(Definition gmdetailsDef READ gmdetailsDef WRITE setGmdetailsDef NOTIFY gmdetailsDefChanged)
    Q_PROPERTY(QString gmdetails READ gmdetails WRITE setGmdetails NOTIFY gmdetailsChanged)
    Q_PROPERTY(FilteredCharacterModel::HealthState hlState READ hlState WRITE setHlState NOTIFY hlStateChanged)
    // clang-format on
public:
    enum Definition
    {
        All,
        With,
        Without,
    };
    Q_ENUM(Definition)
    enum HealthState
    {
        HS_All,
        HS_Full,
        HS_Injured,
        HS_Dead,
    };
    Q_ENUM(HealthState)

    FilteredCharacterModel();
    QString search() const;
    void setSearch(const QString& search);

    FilteredCharacterModel::Definition initiativeCmdDef() const;
    void setInitiativeCmdDef(FilteredCharacterModel::Definition newInitiativeCmdDef);

    FilteredCharacterModel::Definition propertiesDef() const;
    void setPropertiesDef(FilteredCharacterModel::Definition newPropertiesDef);

    FilteredCharacterModel::Definition shapeDef() const;
    void setShapeDef(FilteredCharacterModel::Definition newShapeDef);

    FilteredCharacterModel::Definition actionDef() const;
    void setActionDef(FilteredCharacterModel::Definition newActionDef);

    FilteredCharacterModel::Definition initiativeScoreDef() const;
    void setInitiativeScoreDef(FilteredCharacterModel::Definition newInitiativeScoreDef);

    FilteredCharacterModel::Definition avatarDefinition() const;
    void setAvatarDefinition(campaign::FilteredCharacterModel::Definition newAvatarDefinition);

    bool advanced() const;
    void setAdvanced(bool newAdvanced);

    const QString& exclude() const;
    void setExclude(const QString& newExclude);

    const QString& tags() const;
    void setTags(const QString& newTags);

    const QString& excludeTags() const;
    void setExcludeTags(const QString& newExcludeTags);

    FilteredCharacterModel::Definition gmdetailsDef() const;
    void setGmdetailsDef(FilteredCharacterModel::Definition newGmdetailsDef);

    const QString& gmdetails() const;
    void setGmdetails(const QString& newGmdetails);

    FilteredCharacterModel::HealthState hlState() const;
    void setHlState(FilteredCharacterModel::HealthState newHlState);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

signals:
    void searchChanged();
    void initiativeCmdDefChanged();
    void propertiesDefChanged();
    void shapeDefChanged();
    void actionDefChanged();
    void initiativeScoreDefChanged();
    void avatarDefinitionChanged();
    void advancedChanged();
    void excludeChanged();
    void tagsChanged();
    void excludeTagsChanged();
    void gmdetailsDefChanged();
    void gmdetailsChanged();
    void hlStateChanged();

private:
    QString m_search;
    int m_role= -1;
    Definition m_initiativeCmdDef{All};
    Definition m_propertiesDef{All};
    Definition m_shapeDef{All};
    Definition m_actionDef{All};
    Definition m_initiativeScoreDef{All};
    Definition m_avatarDefinition{All};
    Definition m_gmdetailsDef{All};
    bool m_advanced{};
    QString m_exclude;
    QString m_tags;
    QString m_excludeTags;
    QString m_gmdetails;
    FilteredCharacterModel::HealthState m_hlState{HS_All};
};
}
#endif // FILTEREDCHARACTERMODEL_H
