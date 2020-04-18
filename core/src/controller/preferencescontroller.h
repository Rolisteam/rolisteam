/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PREFERENCESCONTROLLER_H
#define PREFERENCESCONTROLLER_H

#include "controllerinterface.h"
#include <QObject>
#include <memory>

class DiceAliasModel;
class PaletteModel;
class CharacterStateModel;
class QAbstractItemModel;
class RolisteamTheme;
class ThemeModel;
class QStyle;
class PreferencesManager;
class GameController;
class DiceParser;
class PreferencesController : public AbstractControllerInterface
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* characterStateModel READ characterStateModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* diceAliasModel READ diceAliasModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* paletteModel READ paletteModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* themeModel READ themeModel CONSTANT)
    Q_PROPERTY(
        std::size_t currentThemeIndex READ currentThemeIndex WRITE setCurrentThemeIndex NOTIFY currentThemeIndexChanged)
public:
    enum Move
    {
        UP,
        DOWN,
        BOTTOM,
        TOP
    };
    explicit PreferencesController(QObject* parent= nullptr);
    virtual ~PreferencesController() override;

    void setGameController(GameController*) override;

    QAbstractItemModel* characterStateModel() const;
    QAbstractItemModel* diceAliasModel() const;
    QAbstractItemModel* paletteModel() const;
    QAbstractItemModel* themeModel() const;
    std::size_t currentThemeIndex() const;

    QString themeName(int i) const;
    RolisteamTheme* currentTheme() const;

    RolisteamTheme* currentEditableTheme();

    void setDiceHighLightColor(const QColor& color);
signals:
    void currentThemeIndexChanged();

public slots:
    void addAlias();
    void deleteAlias(const QModelIndex& index);
    void moveAlias(const QModelIndex& index, PreferencesController::Move move);

    void addState();
    void deleteState(const QModelIndex& index);
    void moveState(const QModelIndex& index, PreferencesController::Move move);

    void shareModels();

    void savePreferences();
    void loadPreferences();

    void importData(const QString& filename);
    void exportData(const QString& filename);
    void exportTheme(const QString& filename, int pos);
    void importTheme(const QString& filename);

    void dupplicateTheme(int pos, bool selectNew);
    void removeTheme(int i);

    void setCurrentThemeIndex(std::size_t pos);

    void setCurrentThemeStyle(QStyle* style);
    void setColorCurrentTheme(const QColor& color, int palettePos);
    void setCurrentThemeBackground(const QString& path, int pos, const QColor& color);
    void setCurrentThemeCss(const QString& css);

    void setCurrentThemeTitle(const QString& title);

    QString convertAlias(const QString& str);

private:
    std::unique_ptr<CharacterStateModel> m_characterStateModel;
    std::unique_ptr<DiceAliasModel> m_diceAliasModel;
    std::unique_ptr<PaletteModel> m_paletteModel;
    std::unique_ptr<ThemeModel> m_themeModel;
    std::unique_ptr<DiceParser> m_diceParser;

    std::size_t m_currentThemeIndex;
    PreferencesManager* m_preferences;
};

#endif // PREFERENCESCONTROLLER_H
