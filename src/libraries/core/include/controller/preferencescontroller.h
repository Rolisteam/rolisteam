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
#include "model/thememodel.h"
#include <QObject>
#include <core_global.h>
#include <memory>

#include <QAbstractItemModel>

class PaletteModel;
class RolisteamTheme;
class LanguageModel;
class QStyle;
class PreferencesManager;
class GameController;
class CORE_EXPORT PreferencesController : public AbstractControllerInterface
{
    Q_OBJECT
    // Q_PROPERTY(QAbstractItemModel* paletteModel READ paletteModel CONSTANT)
    Q_PROPERTY(ThemeModel* themeModel READ themeModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* languageModel READ languageModel CONSTANT)
    Q_PROPERTY(
        std::size_t currentThemeIndex READ currentThemeIndex WRITE setCurrentThemeIndex NOTIFY currentThemeIndexChanged)
    Q_PROPERTY(bool systemLang READ systemLang WRITE setSystemLang NOTIFY systemLangChanged)
    Q_PROPERTY(bool hasCustomFile READ hasCustomFile WRITE setHasCustomFile NOTIFY hasCustomFileChanged)
    Q_PROPERTY(QString customFilePath READ customFilePath WRITE setCustomFile NOTIFY customFileChanged)
    Q_PROPERTY(int currentLangIndex READ currentLangIndex WRITE setCurrentLangIndex NOTIFY currentLangIndexChanged)
    Q_PROPERTY(QStringList currentLangPath READ currentLangPath NOTIFY currentLangIndexChanged)

public:
    explicit PreferencesController(QObject* parent= nullptr);
    virtual ~PreferencesController() override;

    void setGameController(GameController*) override;

    // QAbstractItemModel* paletteModel() const;
    ThemeModel* themeModel() const;
    QAbstractItemModel* languageModel() const;

    // i18n
    int currentLangIndex() const;
    bool systemLang() const;
    bool hasCustomFile() const;
    QString customFilePath() const;
    QStringList currentLangPath() const;

    // theme
    QString themeName(int i) const;
    RolisteamTheme* currentTheme() const;
    std::size_t currentThemeIndex() const;
    RolisteamTheme* currentEditableTheme();

    void setDiceHighLightColor(const QColor& color);

signals:
    void currentThemeIndexChanged();
    void currentLangIndexChanged();
    void systemLangChanged();
    void hasCustomFileChanged();
    void customFileChanged();

public slots:
    void savePreferences();
    void loadPreferences();

    void importData(const QString& filename);
    void exportData(const QString& filename);

    // i18n
    void setSystemLang(bool b);
    void setHasCustomFile(bool b);
    void setCustomFile(const QString& string);
    void setCurrentLangIndex(int index);

    // theme
    void setCurrentThemeIndex(std::size_t pos);
    void setCurrentThemeStyle(QStyle* style);
    void setColorCurrentTheme(const QColor& color, int palettePos);
    void setCurrentThemeBackground(const QString& path, int pos, const QColor& color);
    void setCurrentThemeCss(const QString& css);
    void exportTheme(const QString& filename, int pos);
    void importTheme(const QString& filename);
    void dupplicateTheme(int pos, bool selectNew);
    void removeTheme(int i);
    void setCurrentThemeTitle(const QString& title);

private:
    // std::unique_ptr<PaletteModel> m_paletteModel;
    std::unique_ptr<ThemeModel> m_themeModel;
    std::unique_ptr<LanguageModel> m_languageModel;

    std::size_t m_currentThemeIndex= 0;
    int m_currentLangIndex= -1;
    bool m_systemLang= true;
    bool m_customFile= false;
    QString m_customFilePath;

    PreferencesManager* m_preferences;
};

#endif // PREFERENCESCONTROLLER_H
