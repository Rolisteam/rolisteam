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
#include "controller/preferencescontroller.h"

#include <QFile>
#include <QJsonDocument>
#include <QStyleFactory>

#include "controller/gamecontroller.h"
#include "data/rolisteamtheme.h"
#include "model/characterstatemodel.h"
#include "model/dicealiasmodel.h"
#include "model/languagemodel.h"
#include "model/palettemodel.h"
#include "preferences/preferencesmanager.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"

#define GRAY_SCALE 191

void initializeThemeModel(ThemeModel* model)
{
    // normal
    model->addTheme(new RolisteamTheme(QPalette(), ThemeModel::tr("default"), "", QStyleFactory::create("fusion"),
                                       ":/resources/rolistheme/workspacebackground.jpg", 0,
                                       QColor(GRAY_SCALE, GRAY_SCALE, GRAY_SCALE), false));

    // DarkOrange
    QFile styleFile(":/stylesheet/resources/stylesheet/darkorange.qss");
    styleFile.open(QFile::ReadOnly);
    QByteArray bytes= styleFile.readAll();
    QString css(bytes);
    model->addTheme(new RolisteamTheme(QPalette(), ThemeModel::tr("darkorange"), css, QStyleFactory::create("fusion"),
                                       ":/resources/rolistheme/workspacebackground.jpg", 0,
                                       QColor(GRAY_SCALE, GRAY_SCALE, GRAY_SCALE), false));

    // DarkFusion
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15, 15, 15));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);

    palette.setColor(QPalette::Highlight, QColor(142, 45, 197).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);

    model->addTheme(new RolisteamTheme(palette, ThemeModel::tr("darkfusion"), "", QStyleFactory::create("fusion"),
                                       ":/resources/rolistheme/workspacebackground.jpg", 0,
                                       QColor(GRAY_SCALE, GRAY_SCALE, GRAY_SCALE), false));
}

/*appendAlias(new DiceAlias("l5r", "D10k"));
model->addAlias(new DiceAlias("l5R", "D10K"));
model->addAlias(new DiceAlias("DF", "D[-1-1]"));
model->addAlias(new DiceAlias("nwod", "D10e10c[>7]"));
model->addAlias(new DiceAlias("(.*)wod(.*)", "\\1d10e[=10]c[>=\\2]-@c[=1]", false));*/

// healthy
/*CharacterState* state= new CharacterState();
state->setId("0");
state->setColor(Qt::black);
state->setLabel(CharacterStateModel::tr("Healthy"));
model->addState(state);

state= new CharacterState();
state->setId("1");
state->setColor(QColor(255, 100, 100));
state->setLabel(CharacterStateModel::tr("Lightly Wounded"));
model->addState(state);

state= new CharacterState();
state->setId("2");
state->setColor(QColor(255, 0, 0));
state->setLabel(CharacterStateModel::tr("Seriously injured"));
model->addState(state);

state= new CharacterState();
state->setId("4");
state->setColor(Qt::gray);
state->setLabel(CharacterStateModel::tr("Dead"));
model->addState(state);

state= new CharacterState();
state->setId("5");
state->setColor(QColor(80, 80, 255));
state->setLabel(CharacterStateModel::tr("Sleeping"));
model->addState(state);

state= new CharacterState();
state->setId("6");
state->setColor(QColor(0, 200, 0));
state->setLabel(CharacterStateModel::tr("Bewitched"));
model->addState(state);*/

PreferencesController::PreferencesController(QObject* parent)
    : AbstractControllerInterface(parent), m_themeModel(new ThemeModel), m_languageModel(new LanguageModel)
{
}

PreferencesController::~PreferencesController()= default;

void PreferencesController::setGameController(GameController* game)
{
    m_preferences= game->preferencesManager();
    loadPreferences();

    if(m_themeModel->rowCount() == 0)
        initializeThemeModel(m_themeModel.get());

    emit preferencesChanged();
    emit externalToolChanged();
}

QAbstractItemModel* PreferencesController::languageModel() const
{
    return m_languageModel.get();
}

PreferencesManager* PreferencesController::preferences() const
{
    return m_preferences;
}
ThemeModel* PreferencesController::themeModel() const
{
    return m_themeModel.get();
}

void PreferencesController::savePreferences()
{ // paths
    m_preferences->registerValue("ThemeNumber", m_themeModel->rowCount(QModelIndex()));

    // i18n
    m_preferences->registerValue("i18n_system", systemLang());
    m_preferences->registerValue("i18n_index", currentLangIndex());
    m_preferences->registerValue("i18n_path", currentLangPath());
    m_preferences->registerValue("i18n_hasCustomfile", hasCustomFile());
    m_preferences->registerValue("i18n_customfile", customFilePath());

    int i= 0;
    for(const auto& tmp : m_themeModel->themes())
    {
        m_preferences->registerValue(QStringLiteral("Theme_%1_name").arg(i), tmp->getName());
        QVariant var;
        var.setValue<QPalette>(tmp->getPalette());
        m_preferences->registerValue(QStringLiteral("Theme_%1_palette").arg(i), var);
        m_preferences->registerValue(QStringLiteral("Theme_%1_stylename").arg(i), tmp->getStyleName());
        m_preferences->registerValue(QStringLiteral("Theme_%1_bgColor").arg(i), tmp->getBackgroundColor());
        m_preferences->registerValue(QStringLiteral("Theme_%1_bgPath").arg(i), tmp->getBackgroundImage());
        m_preferences->registerValue(QStringLiteral("Theme_%1_bgPosition").arg(i), tmp->getBackgroundPosition());
        m_preferences->registerValue(QStringLiteral("Theme_%1_css").arg(i), tmp->getCss());
        m_preferences->registerValue(QStringLiteral("Theme_%1_removable").arg(i), tmp->isRemovable());
        m_preferences->registerValue(QStringLiteral("Theme_%1_highlightDice").arg(i), tmp->getDiceHighlightColor());
        ++i;
    }
}

void PreferencesController::loadPreferences()
{
    auto preferences= m_preferences;
    // Lang
    setCurrentLangIndex(m_preferences->value("i18n_index", -1).toInt());
    setSystemLang(m_preferences->value("i18n_system", true).toBool());
    setHasCustomFile(m_preferences->value("i18n_hasCustomfile", false).toBool());
    setCustomFile(m_preferences->value("i18n_customfile", "").toString());

    // theme
    int size= preferences->value("ThemeNumber", 0).toInt();
    m_currentThemeIndex= static_cast<std::size_t>(size);
    m_themeModel->clear();
    for(int i= 0; i < size; ++i)
    {
        QString name= preferences->value(QStringLiteral("Theme_%1_name").arg(i), QString()).toString();
        QPalette pal= preferences->value(QStringLiteral("Theme_%1_palette").arg(i), QPalette()).value<QPalette>();
        QString style= preferences->value(QStringLiteral("Theme_%1_stylename").arg(i), "fusion").toString();
        QColor color
            = preferences->value(QStringLiteral("Theme_%1_bgColor").arg(i), QColor(GRAY_SCALE, GRAY_SCALE, GRAY_SCALE))
                  .value<QColor>();
        QString path= preferences->value(QStringLiteral("Theme_%1_bgPath").arg(i), "").toString();
        QColor hlDice
            = preferences->value(QStringLiteral("Theme_%1_highlightDice").arg(i), QColor(Qt::red)).value<QColor>();
        int pos= preferences->value(QStringLiteral("Theme_%1_bgPosition").arg(i), 0).toInt();
        QString css= preferences->value(QStringLiteral("Theme_%1_css").arg(i), "").toString();
        bool isRemovable= preferences->value(QStringLiteral("Theme_%1_removable").arg(i), false).toBool();
        RolisteamTheme* tmp
            = new RolisteamTheme(pal, name, css, QStyleFactory::create(style), path, pos, color, isRemovable);
        tmp->setDiceHighlightColor(hlDice);
        m_themeModel->addTheme(tmp);
        // m_preferences->registerValue(QString("Theme_%1_css").arg(i),tmp->getName());
    }

    auto func= [this](const QVariant& value)
    {
        Q_UNUSED(value);
        emit externalToolChanged();
    };

    preferences->registerLambda("RcsePath", func);
    preferences->registerLambda("MindmapPath", func);
    preferences->registerLambda("textEditorPath", func);

    setCurrentThemeIndex(static_cast<std::size_t>(preferences->value("currentThemeIndex", 0).toInt()));
}

void PreferencesController::importData(const QString& filename)
{
    if(filename.isEmpty())
        return;

    QFile file(filename);
    if(file.exists() && file.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc= QJsonDocument::fromJson(file.readAll());
        auto obj= doc.object();
    }
}

void PreferencesController::exportData(const QString& filename)
{
    if(filename.isEmpty())
        return;

    QJsonObject obj;
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc;
        doc.setObject(obj);
        file.write(doc.toJson());
    }
}

void PreferencesController::setSystemLang(bool b)
{
    if(m_systemLang == b)
        return;
    m_systemLang= b;
    emit systemLangChanged();
}

void PreferencesController::setHasCustomFile(bool b)
{
    if(m_customFile == b)
        return;
    m_customFile= b;
    emit hasCustomFileChanged();
}

void PreferencesController::setCustomFile(const QString& string)
{
    if(m_customFilePath == string)
        return;
    m_customFilePath= string;
    emit customFileChanged();
}

void PreferencesController::setCurrentLangIndex(int index)
{
    if(m_currentLangIndex == index)
        return;
    m_currentLangIndex= index;
    emit currentLangIndexChanged();
}

QString PreferencesController::themeName(int i) const
{
    return m_themeModel->name(i);
}

RolisteamTheme* PreferencesController::currentTheme() const
{
    return m_themeModel->theme(static_cast<int>(m_currentThemeIndex));
}

void PreferencesController::exportTheme(const QString& filename, int pos)
{
    if(filename.isEmpty())
        return;

    auto theme= m_themeModel->theme(pos);

    if(nullptr == theme)
        return;

    QFile exportFile(filename);
    if(!exportFile.open(QIODevice::WriteOnly))
    {
        return;
    }
    // QJsonObject skinObject;
    // theme->writeTo(skinObject);
    auto skinObject= IOHelper::themeToObject(theme);
    // m_paletteModel->writeTo(skinObject);
    QJsonDocument saveDoc(skinObject);
    exportFile.write(saveDoc.toJson());
}

void PreferencesController::importTheme(const QString& filename)
{
    if(filename.isEmpty())
        return;

    QFile importFile(filename);
    if(!importFile.open(QIODevice::ReadOnly))
    {
        return;
    }
    QByteArray saveData= importFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    auto theme= IOHelper::jsonToTheme(loadDoc.object());

    m_themeModel->addTheme(theme);
}
void PreferencesController::removeTheme(int i)
{
    m_themeModel->removeTheme(i);
}
void PreferencesController::dupplicateTheme(int pos, bool selectNew)
{
    auto theme= m_themeModel->theme(pos);
    if(theme == nullptr)
        return;

    QString str= theme->getName();
    str.append(tr(" (copy)"));
    RolisteamTheme* newTheme
        = new RolisteamTheme(theme->getPalette(), str, theme->getCss(), theme->getStyle(), theme->getBackgroundImage(),
                             theme->getBackgroundPosition(), theme->getBackgroundColor(), true);

    m_themeModel->addTheme(newTheme);
    if(selectNew)
        setCurrentThemeIndex(static_cast<std::size_t>(m_themeModel->rowCount() - 1));
}

void PreferencesController::setCurrentThemeIndex(std::size_t pos)
{
    if(pos == m_currentThemeIndex)
        return;
    m_currentThemeIndex= pos;
    emit currentThemeIndexChanged();

    auto theme= currentTheme();
    if(nullptr == theme)
        return;
    // m_paletteModel->setPalette(theme->getPalette());

    m_preferences->registerValue("PathOfBackgroundImage", theme->getBackgroundImage());
    m_preferences->registerValue("BackGroundColor", theme->getBackgroundColor());
    m_preferences->registerValue("BackGroundPositioning", theme->getBackgroundPosition());
}

std::size_t PreferencesController::currentThemeIndex() const
{
    return m_currentThemeIndex;
}

int PreferencesController::currentLangIndex() const
{
    return m_currentLangIndex;
}

bool PreferencesController::systemLang() const
{
    return m_systemLang;
}

bool PreferencesController::hasCustomFile() const
{
    return m_customFile;
}

QString PreferencesController::customFilePath() const
{
    return m_customFilePath;
}

QStringList PreferencesController::currentLangPath() const
{
    QStringList list;
    list= m_languageModel->pathFromIndex(m_languageModel->index(m_currentLangIndex, 0));
    return list;
}

RolisteamTheme* PreferencesController::currentEditableTheme()
{
    auto theme= currentTheme();

    if(!theme->isRemovable())
    {
        dupplicateTheme(static_cast<int>(m_currentThemeIndex), true);
        theme= currentTheme();
    }

    return theme;
}

QString PreferencesController::externalEditorFor(Core::MediaType type)
{
    QString path;
    switch(type)
    {
    case Core::MediaType::CharacterSheetFile:
        path= m_preferences->value("RcsePath", path).toString();
        break;
    case Core::MediaType::MindmapFile:
        path= m_preferences->value("MindmapPath", path).toString();
        break;
    case Core::MediaType::TextFile:
    case Core::MediaType::MarkdownFile:
        path= m_preferences->value("textEditorPath", path).toString();
        break;
    default:
        break;
    }

    return path;
}

QString PreferencesController::paramsFor(Core::MediaType type)
{
    QString path;
    switch(type)
    {
    case Core::MediaType::CharacterSheetFile:
        path= m_preferences->value("RcseParam", path).toString();
        break;
    case Core::MediaType::MindmapFile:
        path= m_preferences->value("MindmapParam", path).toString();
        break;
    case Core::MediaType::TextFile:
    case Core::MediaType::MarkdownFile:
        path= m_preferences->value("textEditorParam", path).toString();
        break;
    default:
        break;
    }

    return path;
}

void PreferencesController::setCurrentThemeStyle(QStyle* style)
{
    auto theme= currentEditableTheme();
    if(nullptr == theme)
        return;

    theme->setStyle(style);
}

void PreferencesController::setCurrentThemeBackground(const QString& path, int pos, const QColor& color)
{
    auto theme= currentEditableTheme();
    if(nullptr == theme)
        return;
    auto preferences= m_preferences;

    theme->setBackgroundImage(path);
    theme->setBackgroundColor(color);
    theme->setBackgroundPosition(pos);

    preferences->registerValue("PathOfBackgroundImage", path);
    preferences->registerValue("BackGroundColor", color);
    preferences->registerValue("BackGroundPositioning", pos);
}

void PreferencesController::setColorCurrentTheme(const QColor& color, int palettePos)
{
    auto theme= currentEditableTheme();
    if(nullptr == theme)
        return;
    auto model= theme->paletteModel();

    model->setColor(palettePos, color);
    theme->setPalette(model->getPalette());
}

void PreferencesController::setDiceHighLightColor(const QColor& color)
{
    auto theme= currentEditableTheme();
    theme->setDiceHighlightColor(color);
    m_preferences->registerValue("DiceHighlightColor", color);
}

void PreferencesController::setCurrentThemeCss(const QString& css)
{
    auto theme= currentEditableTheme();
    if(nullptr == theme)
        return;

    theme->setCss(css);
}

void PreferencesController::setCurrentThemeTitle(const QString& title)
{
    auto theme= currentEditableTheme();
    if(nullptr == theme)
        return;

    theme->setName(title);
}
