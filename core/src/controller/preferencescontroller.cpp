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
#include "preferencescontroller.h"

#include <QFile>
#include <QJsonDocument>
#include <QStyleFactory>

#include "controller/gamecontroller.h"
#include "diceparser.h"
#include "model/thememodel.h"
#include "preferences/characterstatemodel.h"
#include "preferences/dicealiasmodel.h"
#include "preferences/palettemodel.h"
#include "preferences/preferencesmanager.h"
#include "preferences/rolisteamtheme.h"
#include "worker/messagehelper.h"

#include "network/networkmessage.h"
#include "network/receiveevent.h"

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

void initializeDiceAliasModel(DiceAliasModel* model)
{
    model->addAlias(new DiceAlias("l5r", "D10k"));
    model->addAlias(new DiceAlias("l5R", "D10K"));
    model->addAlias(new DiceAlias("DF", "D[-1-1]"));
    model->addAlias(new DiceAlias("nwod", "D10e10c[>7]"));
    model->addAlias(new DiceAlias("(.*)wod(.*)", "\\1d10e[=10]c[>=\\2]-@c[=1]", false));
}

void initializeState(CharacterStateModel* model)
{
    // healthy
    CharacterState* state= new CharacterState();
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
    model->addState(state);
}

PreferencesController::PreferencesController(QObject* parent)
    : AbstractControllerInterface(parent)
    , m_characterStateModel(new CharacterStateModel)
    , m_diceAliasModel(new DiceAliasModel)
    , m_paletteModel(new PaletteModel)
    , m_themeModel(new ThemeModel)
    , m_diceParser(new DiceParser)
{

    // to remove
    ReceiveEvent::registerNetworkReceiver(NetMsg::SharePreferencesCategory, m_characterStateModel.get());
}

PreferencesController::~PreferencesController()= default;

void PreferencesController::setGameController(GameController* game)
{
    m_preferences= game->preferencesManager();
    loadPreferences();
    if(m_diceAliasModel->rowCount() == 0)
        initializeDiceAliasModel(m_diceAliasModel.get());

    if(m_themeModel->rowCount() == 0)
        initializeThemeModel(m_themeModel.get());

    if(m_characterStateModel->rowCount() == 0)
        initializeState(m_characterStateModel.get());
}

QAbstractItemModel* PreferencesController::characterStateModel() const
{
    return m_characterStateModel.get();
}
QAbstractItemModel* PreferencesController::diceAliasModel() const
{
    return m_diceAliasModel.get();
}
QAbstractItemModel* PreferencesController::paletteModel() const
{
    return m_paletteModel.get();
}
QAbstractItemModel* PreferencesController::themeModel() const
{
    return m_themeModel.get();
}

void PreferencesController::addAlias()
{
    m_diceAliasModel->appendAlias();
}

void PreferencesController::deleteAlias(const QModelIndex& index)
{
    m_diceAliasModel->deleteAlias(index);
}

QString PreferencesController::convertAlias(const QString& str)
{
    m_diceParser->constAliases();
    return m_diceParser->convertAlias(str);
}

void PreferencesController::moveAlias(const QModelIndex& index, PreferencesController::Move move)
{
    switch(move)
    {
    case UP:
        m_diceAliasModel->upAlias(index);
        break;
    case DOWN:
        m_diceAliasModel->downAlias(index);
        break;
    case TOP:
        m_diceAliasModel->topAlias(index);
        break;
    case BOTTOM:
        m_diceAliasModel->bottomAlias(index);
        break;
    }
}

void PreferencesController::addState()
{
    m_characterStateModel->appendState();
}

void PreferencesController::deleteState(const QModelIndex& index)
{
    m_characterStateModel->deleteState(index);
}

void PreferencesController::moveState(const QModelIndex& index, PreferencesController::Move move)
{
    switch(move)
    {
    case UP:
        m_characterStateModel->upState(index);
        break;
    case DOWN:
        m_characterStateModel->downState(index);
        break;
    case TOP:
        m_characterStateModel->topState(index);
        break;
    case BOTTOM:
        m_characterStateModel->bottomState(index);
        break;
    }
}
void PreferencesController::shareModels()
{
    MessageHelper::sendOffAllDiceAlias(m_diceAliasModel.get());
    MessageHelper::sendOffAllCharacterState(m_characterStateModel.get());
}

void PreferencesController::savePreferences()
{ // paths
    m_preferences->registerValue("ThemeNumber", m_themeModel->rowCount(QModelIndex()));
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

    // DiceSystem
    auto aliasList= m_diceAliasModel->getAliases();
    m_preferences->registerValue("DiceAliasNumber", aliasList->size());
    i= 0;
    for(auto tmpAlias : *aliasList)
    {
        m_preferences->registerValue(QStringLiteral("DiceAlias_%1_command").arg(i), tmpAlias->getCommand());
        m_preferences->registerValue(QStringLiteral("DiceAlias_%1_value").arg(i), tmpAlias->getValue());
        m_preferences->registerValue(QStringLiteral("DiceAlias_%1_type").arg(i), tmpAlias->isReplace());
        m_preferences->registerValue(QStringLiteral("DiceAlias_%1_enable").arg(i), tmpAlias->isEnable());
        ++i;
    }

    // State
    auto stateList= m_characterStateModel->getCharacterStates();
    m_preferences->registerValue("CharacterStateNumber", stateList.size());
    i= 0;
    for(auto tmpState : stateList)
    {
        m_preferences->registerValue(QStringLiteral("CharacterState_%1_id").arg(i), tmpState->id());
        m_preferences->registerValue(QStringLiteral("CharacterState_%1_label").arg(i), tmpState->getLabel());
        m_preferences->registerValue(QStringLiteral("CharacterState_%1_color").arg(i), tmpState->getColor());
        m_preferences->registerValue(QStringLiteral("CharacterState_%1_pixmap").arg(i), tmpState->getImage());
        ++i;
    }
}

void PreferencesController::loadPreferences()
{
    auto preferences= m_preferences;
    // theme
    int size= preferences->value("ThemeNumber", 0).toInt();
    m_currentThemeIndex= static_cast<std::size_t>(size);
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

    /*   ui->m_styleCombo->clear();
       ui->m_styleCombo->addItems(QStyleFactory::keys());*/

    setCurrentThemeIndex(static_cast<std::size_t>(preferences->value("currentThemeIndex", 0).toInt()));

    // connect(ui->m_themeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTheme()));
    // connect(ui->m_styleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setStyle()));

    // DiceSystem
    size= preferences->value("DiceAliasNumber", 0).toInt();
    for(int i= 0; i < size; ++i)
    {
        QString cmd= preferences->value(QStringLiteral("DiceAlias_%1_command").arg(i), "").toString();
        QString value= preferences->value(QStringLiteral("DiceAlias_%1_value").arg(i), "").toString();
        bool replace= preferences->value(QStringLiteral("DiceAlias_%1_type").arg(i), true).toBool();
        bool enable= preferences->value(QStringLiteral("DiceAlias_%1_enable").arg(i), true).toBool();

        DiceAlias* tmpAlias= new DiceAlias(cmd, value, replace, enable);
        m_diceAliasModel->addAlias(tmpAlias);
    }

    // DiceSystem
    size= preferences->value("CharacterStateNumber", 0).toInt();
    for(int i= 0; i < size; ++i)
    {
        auto id= preferences->value(QStringLiteral("CharacterState_%1_id").arg(i), QString::number(i)).toString();
        QString label= preferences->value(QStringLiteral("CharacterState_%1_label").arg(i), "").toString();
        QColor color= preferences->value(QStringLiteral("CharacterState_%1_color").arg(i), "").value<QColor>();
        QPixmap pixmap= preferences->value(QStringLiteral("CharacterState_%1_pixmap").arg(i), true).value<QPixmap>();

        CharacterState* tmpState= new CharacterState();
        tmpState->setId(id);
        tmpState->setLabel(label);
        tmpState->setColor(color);
        tmpState->setImage(pixmap);
        m_characterStateModel->addState(tmpState);
    }
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
        m_characterStateModel->load(obj);
        m_diceAliasModel->load(obj);
    }
}

void PreferencesController::exportData(const QString& filename)
{
    if(filename.isEmpty())
        return;

    QJsonObject obj;
    m_characterStateModel->save(obj);
    m_diceAliasModel->save(obj);
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc;
        doc.setObject(obj);
        file.write(doc.toJson());
    }
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
    auto idx= static_cast<std::size_t>(pos);
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
    QJsonObject skinObject;
    theme->writeTo(skinObject);
    m_paletteModel->writeTo(skinObject);
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
    RolisteamTheme* theme
        = new RolisteamTheme(QPalette(), "", "", QStyleFactory::create("fusion"), "", 0, QColor(), false);
    theme->readFrom(loadDoc.object());
    m_paletteModel->readFrom(loadDoc.object());
    theme->setPalette(m_paletteModel->getPalette());

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
    m_paletteModel->setPalette(theme->getPalette());

    m_preferences->registerValue("PathOfBackgroundImage", theme->getBackgroundImage());
    m_preferences->registerValue("BackGroundColor", theme->getBackgroundColor());
    m_preferences->registerValue("BackGroundPositioning", theme->getBackgroundPosition());
}

std::size_t PreferencesController::currentThemeIndex() const
{
    return m_currentThemeIndex;
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

    m_paletteModel->setColor(palettePos, color);
    theme->setPalette(m_paletteModel->getPalette());
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
