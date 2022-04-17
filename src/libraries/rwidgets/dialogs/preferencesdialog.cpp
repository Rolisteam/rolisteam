/*************************************************************************
 *     Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     https://rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#include "preferencesdialog.h"

#include "controller/preferencescontroller.h"
#include "model/palettemodel.h"

#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QImageReader>
#include <QInputDialog>
#include <QItemEditorCreatorBase>
#include <QJsonDocument>
#include <QPainter>
#include <QPushButton>
#include <QStringListModel>
#include <QStyleFactory>
#include <QVBoxLayout>

#ifdef HAVE_SOUND
#include <QMediaPlayer>
#endif

#include "diceparser/diceparser.h"
#include "media/mediatype.h"
#include "model/thememodel.h"

#include "ui_preferencesdialogbox.h"

inline uint qHash(PreferencesDialog::PreferenceTab type, uint seed)
{
    return ::qHash(static_cast<uint>(type), seed);
}

/*********************
 * PreferencesDialog *
 *********************/

PreferencesDialog::PreferencesDialog(PreferencesController* controller, QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f), ui(new Ui::PreferencesDialogBox()), m_ctrl(controller)
{
    ui->setupUi(this);
    m_currentThemeIsEditable= false;

    // create empty directories list
    m_musicDirectories.append(new QStringListModel(QStringList()));
    m_musicDirectories.append(new QStringListModel(QStringList()));
    m_musicDirectories.append(new QStringListModel(QStringList()));

    ui->m_directoriesList1->setModel(m_musicDirectories[0]);
    ui->m_directoriesList2->setModel(m_musicDirectories[1]);
    ui->m_directoriesList3->setModel(m_musicDirectories[2]);

    // TODO Fix this line
    // ui->m_defaultMapModeCombo->addItems();
    m_preferences= PreferencesManager::getInstance();

    ui->m_themeComboBox->setModel(m_ctrl->themeModel());
    connect(ui->m_themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int pos) { m_ctrl->setCurrentThemeIndex(static_cast<std::size_t>(pos)); });
    // m_paletteModel->setPalette(palette());
    connect(m_ctrl, &PreferencesController::currentThemeIndexChanged, this,
            [this]() { ui->m_paletteTableView->setModel(m_ctrl->currentTheme()->paletteModel()); });
    ui->m_paletteTableView->setModel(m_ctrl->currentTheme()->paletteModel());
    auto horizontalHeader= ui->m_paletteTableView->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(this, &PreferencesDialog::accepted, m_ctrl, &PreferencesController::savePreferences);

    connect(ui->m_startDiag, SIGNAL(clicked()), this, SLOT(performDiag()));
    // i18n
    connect(ui->m_systemTranslation, &QCheckBox::clicked, this, &PreferencesDialog::updateTranslationPref);
    connect(ui->m_customTranslation, &QCheckBox::clicked, this, &PreferencesDialog::updateTranslationPref);

    connect(ui->m_systemTranslation, &QCheckBox::toggled, m_ctrl, &PreferencesController::setSystemLang);
    connect(ui->m_customTranslation, &QCheckBox::toggled, m_ctrl, &PreferencesController::setHasCustomFile);
    connect(ui->m_translationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), m_ctrl,
            &PreferencesController::setCurrentLangIndex);
    connect(ui->m_translationFileEdit, &FileDirChooser::pathChanged, m_ctrl, &PreferencesController::setCustomFile);

    ui->m_translationSelector->setModel(m_ctrl->languageModel());
    // set general panel as default.
    ui->tabWidget->setCurrentIndex(0);
    connect(ui->m_highLightPenWidth, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [=]() { m_preferences->registerValue("VMAP::highlightPenWidth", ui->m_highLightPenWidth->value(), true); });
    connect(ui->m_mapItemHighlightColor, &ColorButton::colorChanged, this, [=]() {
        m_preferences->registerValue("VMAP::highlightColor", ui->m_mapItemHighlightColor->color(), true);
    });

    connect(ui->m_hideTipsOfTheDay, &QCheckBox::clicked, this, [this]() {
        m_preferences->registerValue("MainWindow::neverDisplayTips", ui->m_hideTipsOfTheDay->isChecked(), false);
    });
    // Misc
    setSizeGripEnabled(true);
    setWindowTitle(
        QString("%1 - %2").arg(m_preferences->value("Application_Name", "rolisteam").toString(), tr("Preferences")));
    setWindowModality(Qt::ApplicationModal);

    connect(ui->m_heartBeat, SIGNAL(clicked(bool)), this, SLOT(manageHeartBeat()));
    connect(ui->m_hbFrequency, SIGNAL(valueChanged(int)), this, SLOT(manageHeartBeat()));
    connect(ui->m_displayTimePage, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
            [=](int val) { m_preferences->registerValue("waitingTimeBetweenPage", val, true); });
    // Messaging
    connect(ui->m_showTimeCheckBox, SIGNAL(clicked(bool)), this, SLOT(manageMessagingPref()));
    connect(ui->m_timeColorBtn, SIGNAL(clicked(bool)), this, SLOT(manageMessagingPref()));
    connect(ui->m_maxLenghtCommand, SIGNAL(valueChanged(int)), this, SLOT(manageMessagingPref()));
    connect(ui->m_hideLongCommand, SIGNAL(toggled(bool)), this, SLOT(manageMessagingPref()));

    // background
    connect(ui->m_positioningComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(backgroundChanged()));
    connect(ui->m_bgColorPush, SIGNAL(colorChanged(QColor)), this, SLOT(backgroundChanged()));
    connect(ui->m_backgroundImage, &FileDirChooser::pathChanged, this, &PreferencesDialog::backgroundChanged);
    connect(ui->m_diceHighlightColorBtn, &ColorButton::colorChanged, m_ctrl,
            &PreferencesController::setDiceHighLightColor);
    // connect(ui->m_backgroundImage,ç)

    // themes
    connect(ui->m_copyThemeButton, SIGNAL(clicked()), this, SLOT(dupplicateTheme()));
    connect(ui->m_themeNameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(setTitleAtCurrentTheme()));

    connect(ui->m_paletteTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editColor(QModelIndex)));

    connect(ui->m_cssEdit, SIGNAL(clicked()), this, SLOT(editCss()));
    connect(ui->m_exportBtn, SIGNAL(clicked()), this, SLOT(exportTheme()));
    connect(ui->m_importBtn, SIGNAL(clicked()), this, SLOT(importTheme()));
    connect(ui->m_deleteTheme, SIGNAL(clicked()), this, SLOT(deleteTheme()));

    connect(m_ctrl, &PreferencesController::currentLangIndexChanged, this,
            [this]() { ui->m_translationSelector->setCurrentIndex(m_ctrl->currentLangIndex()); });

    ui->m_ap1Add->setData(PreferencesDialog::First);
    ui->m_ap2Add->setData(PreferencesDialog::Second);
    ui->m_ap3Add->setData(PreferencesDialog::Third);

    ui->m_ap1Delete->setData(PreferencesDialog::First);
    ui->m_ap2Delete->setData(PreferencesDialog::Second);
    ui->m_ap3Delete->setData(PreferencesDialog::Third);

    ui->m_ap1upAct->setData(PreferencesDialog::First);
    ui->m_ap2upAct->setData(PreferencesDialog::Second);
    ui->m_ap3upAct->setData(PreferencesDialog::Third);

    ui->m_ap1downAct->setData(PreferencesDialog::First);
    ui->m_ap2downAct->setData(PreferencesDialog::Second);
    ui->m_ap3downAct->setData(PreferencesDialog::Third);

    ui->m_add1Btn->setDefaultAction(ui->m_ap1Add);
    ui->m_add2Btn->setDefaultAction(ui->m_ap2Add);
    ui->m_add3Btn->setDefaultAction(ui->m_ap3Add);

    ui->m_delete1Btn->setDefaultAction(ui->m_ap1Delete);
    ui->m_delete2Btn->setDefaultAction(ui->m_ap2Delete);
    ui->m_delete3Btn->setDefaultAction(ui->m_ap3Delete);

    ui->m_up1Btn->setDefaultAction(ui->m_ap1upAct);
    ui->m_up2Btn->setDefaultAction(ui->m_ap2upAct);
    ui->m_up3Btn->setDefaultAction(ui->m_ap3upAct);

    ui->m_down1Btn->setDefaultAction(ui->m_ap1downAct);
    ui->m_down2Btn->setDefaultAction(ui->m_ap2downAct);
    ui->m_down3Btn->setDefaultAction(ui->m_ap3downAct);

    connect(ui->m_ap1Add, &QAction::triggered, this, &PreferencesDialog::addDirectory);
    connect(ui->m_ap2Add, &QAction::triggered, this, &PreferencesDialog::addDirectory);
    connect(ui->m_ap3Add, &QAction::triggered, this, &PreferencesDialog::addDirectory);

    connect(ui->m_ap1Delete, &QAction::triggered, this, &PreferencesDialog::removeDirectory);
    connect(ui->m_ap2Delete, &QAction::triggered, this, &PreferencesDialog::removeDirectory);
    connect(ui->m_ap3Delete, &QAction::triggered, this, &PreferencesDialog::removeDirectory);

    connect(ui->m_ap1upAct, &QAction::triggered, this, &PreferencesDialog::upDirectory);
    connect(ui->m_ap2upAct, &QAction::triggered, this, &PreferencesDialog::upDirectory);
    connect(ui->m_ap3upAct, &QAction::triggered, this, &PreferencesDialog::upDirectory);

    connect(ui->m_ap1downAct, &QAction::triggered, this, &PreferencesDialog::downDirectory);
    connect(ui->m_ap2downAct, &QAction::triggered, this, &PreferencesDialog::downDirectory);
    connect(ui->m_ap3downAct, &QAction::triggered, this, &PreferencesDialog::downDirectory);

    connect(ui->m_directoriesList1, &QListView::clicked, this,
            [this]()
            {
                auto current= ui->m_directoriesList1->currentIndex();

                auto isValid= current.isValid();

                ui->m_ap1Delete->setEnabled(isValid);
                ui->m_ap1upAct->setEnabled(isValid);
                ui->m_ap1downAct->setEnabled(isValid);
            });

    connect(ui->m_directoriesList2, &QListView::clicked, this,
            [this]()
            {
                auto current= ui->m_directoriesList2->currentIndex();

                auto isValid= current.isValid();

                ui->m_ap2Delete->setEnabled(isValid);
                ui->m_ap2upAct->setEnabled(isValid);
                ui->m_ap2downAct->setEnabled(isValid);
            });

    connect(ui->m_directoriesList3, &QListView::clicked, this,
            [this]()
            {
                auto current= ui->m_directoriesList3->currentIndex();

                auto isValid= current.isValid();

                ui->m_ap3Delete->setEnabled(isValid);
                ui->m_ap3upAct->setEnabled(isValid);
                ui->m_ap3downAct->setEnabled(isValid);
            });

    updateTranslationPref();
}

PreferencesDialog::~PreferencesDialog() {}

void PreferencesDialog::manageHeartBeat()
{
    m_preferences->registerValue("HeartBeatStatus", ui->m_heartBeat->isChecked());
    m_preferences->registerValue("HbFrequency", ui->m_hbFrequency->value());
}
void PreferencesDialog::manageMessagingPref()
{
    m_preferences->registerValue("MessagingShowTime", ui->m_showTimeCheckBox->isChecked());
    m_preferences->registerValue("MessagingColorTime", ui->m_timeColorBtn->color());
    m_preferences->registerValue("maxSizeForCuttingDiceCmd", ui->m_maxLenghtCommand->value());
    m_preferences->registerValue("hideLongCommand", ui->m_hideLongCommand->isChecked());
}

void PreferencesDialog::updateTranslationPref()
{
    auto systemTrans= m_ctrl->systemLang();
    auto customTrans= m_ctrl->hasCustomFile();

    ui->m_customTranslation->setEnabled(!systemTrans);

    ui->m_availablesTranslationLbl->setEnabled(!systemTrans && !customTrans);
    ui->m_translationSelector->setEnabled(!systemTrans && !customTrans);

    ui->m_translationFileLbl->setEnabled(!systemTrans && customTrans);
    ui->m_translationFileEdit->setEnabled(!systemTrans && customTrans);
}

void PreferencesDialog::show(PreferenceTab tab)
{
    static QHash<PreferenceTab, int> tabData{{PreferenceTab::General, 0},
                                             {PreferenceTab::Player, 1},
                                             {PreferenceTab::Themes, 2},
                                             {PreferenceTab::Diagnostic, 3}};
    ui->tabWidget->setCurrentIndex(tabData.value(tab));
    load();
    QDialog::show();
}
void PreferencesDialog::save() const
{
    m_preferences->registerValue("MainWindow::MustBeChecked", ui->m_checkUpdate->isChecked());
    m_preferences->registerValue("defaultPermissionMap", ui->m_defaultMapModeCombo->currentIndex());

    // messaging
    m_preferences->registerValue("MessagingShowTime", ui->m_showTimeCheckBox->isChecked());
    m_preferences->registerValue("MessagingColorTime", ui->m_timeColorBtn->color());

    // General
    QColor color;
    int opacity= ui->m_opacitySlider->value();
    color.setRgb(opacity, opacity, opacity);
    m_preferences->registerValue("Fog_color", color);
    m_preferences->registerValue("Mask_color", ui->m_fogColor->color());
    m_preferences->registerValue("PictureAdjust", ui->m_pictureAdjust->isChecked());
    m_preferences->registerValue("FullScreenAtStarting", ui->m_fullScreenCheckbox->isChecked());
    m_preferences->registerValue("maxSizeForCuttingDiceCmd", ui->m_maxLenghtCommand->value());
    m_preferences->registerValue("hideLongCommand", ui->m_hideLongCommand->isChecked());
    m_preferences->registerValue("shortNameInTabMode", ui->m_shortNameCb->isChecked());
    m_preferences->registerValue("MaxLengthTabName", ui->m_tabTitleLength->value());

    m_preferences->registerValue("waitingTimeBetweenPage", ui->m_displayTimePage->value());

    // theme
    m_preferences->registerValue("currentTheme", ui->m_themeComboBox->currentText());

    m_preferences->registerValue(QStringLiteral("LogDebug"), ui->m_debugLogInfo->isChecked());
    m_preferences->registerValue(QStringLiteral("LogResearch"), ui->m_logUniversityResearch->isChecked());
    m_preferences->registerValue(QStringLiteral("dataCollection"), ui->m_enableDataCollection->isChecked());

    qDebug() << "list" << m_musicDirectories[0]->stringList();
    m_preferences->registerValue(Core::preferences::KEY_DIRECTORY_AP1, m_musicDirectories[0]->stringList());
    m_preferences->registerValue(Core::preferences::KEY_DIRECTORY_AP2, m_musicDirectories[1]->stringList());
    m_preferences->registerValue(Core::preferences::KEY_DIRECTORY_AP3, m_musicDirectories[2]->stringList());

    m_ctrl->savePreferences();
}
void PreferencesDialog::load()
{
    // Direcotry PATH
    ui->m_translationFileEdit->setMode(false);
    ui->m_translationFileEdit->setFilter("Translation File: (*.qm)");

    ui->m_translationFileEdit->setPath(m_preferences->value("currentTranslationFile", "").toString());
    ui->m_checkUpdate->setChecked(m_preferences->value("MainWindow::MustBeChecked", true).toBool());

    ui->m_hideTipsOfTheDay->setChecked(m_preferences->value("MainWindow::neverDisplayTips", false).toBool());

    ui->m_heartBeat->setChecked(m_preferences->value("HeartBeatStatus", false).toBool());
    ui->m_hbFrequency->setValue(m_preferences->value("HbFrequency", 60).toInt());

    ui->m_shortNameCb->setChecked(m_preferences->value("shortNameInTabMode", false).toBool());
    ui->m_tabTitleLength->setValue(m_preferences->value("MaxLengthTabName", 10).toInt());

    ui->m_displayTimePage->setValue(m_preferences->value("waitingTimeBetweenPage", 300).toInt());

    ui->m_highLightPenWidth->setValue(m_preferences->value("VMAP::highlightPenWidth", 6).toInt());
    ui->m_mapItemHighlightColor->setColor(
        m_preferences->value("VMAP::highlightColor", QColor(Qt::red)).value<QColor>());

    ui->m_systemTranslation->setChecked(m_ctrl->systemLang());
    ui->m_customTranslation->setChecked(m_ctrl->hasCustomFile());
    ui->m_translationSelector->setCurrentIndex(m_ctrl->currentLangIndex());
    ui->m_translationFileEdit->setPath(m_ctrl->customFilePath());

    ////////////////////////
    // MAP
    ///////////////////////
    // Fog of WAR
    QColor fog= m_preferences->value("Fog_color", QColor(Qt::black)).value<QColor>();
    ui->m_opacitySlider->setValue(m_preferences->value("Fog_opacity", fog.red()).toInt());
    ui->m_opacitySpin->setValue(m_preferences->value("Fog_opacity", fog.red()).toInt());
    ui->m_fogColor->setColor(m_preferences->value("Mask_color", QColor(Qt::darkMagenta)).value<QColor>());
    ui->m_fullScreenCheckbox->setChecked(m_preferences->value("FullScreenAtStarting", true).toBool());
    ui->m_maxLenghtCommand->setValue(m_preferences->value("maxSizeForCuttingDiceCmd", 100).toInt());
    ui->m_hideLongCommand->setChecked(m_preferences->value("hideLongCommand", false).toBool());

    // Default Permission
    ui->m_defaultMapModeCombo->setCurrentIndex(m_preferences->value("defaultPermissionMap", 0).toInt());
    ui->m_pictureAdjust->setChecked(m_preferences->value("PictureAdjust", true).toBool());

    // theme
    // initializeStyle();
    ui->m_backgroundImage->setMode(false);
    ui->m_backgroundImage->setFilter(tr("Images (*.png *.xpm *.jpg *.gif *.bmp)"));

    // Messaging
    ui->m_showTimeCheckBox->setChecked(m_preferences->value("MessagingShowTime", false).toBool());
    ui->m_timeColorBtn->setColor(m_preferences->value("MessagingColorTime", QColor(Qt::darkGreen)).value<QColor>());

    // LOG
    ui->m_debugLogInfo->setChecked(m_preferences->value(QStringLiteral("LogDebug"), false).toBool());
    ui->m_logUniversityResearch->setChecked(m_preferences->value(QStringLiteral("LogResearch"), false).toBool());
    ui->m_enableDataCollection->setChecked(m_preferences->value(QStringLiteral("dataCollection"), false).toBool());

    // music directory
    qDebug() << "list " << m_preferences->value(Core::preferences::KEY_DIRECTORY_AP1, {}).toStringList();
    m_musicDirectories[0]->setStringList(m_preferences->value(Core::preferences::KEY_DIRECTORY_AP1, {}).toStringList());
    m_musicDirectories[1]->setStringList(m_preferences->value(Core::preferences::KEY_DIRECTORY_AP2, {}).toStringList());
    m_musicDirectories[2]->setStringList(m_preferences->value(Core::preferences::KEY_DIRECTORY_AP3, {}).toStringList());

    // m_ctrl->loadPreferences();
    updateTheme();
}

void PreferencesDialog::editColor(QModelIndex index)
{
    QColor color= QColorDialog::getColor(index.data(Qt::DecorationRole).value<QColor>(), this);
    if(!color.isValid())
        return;

    m_ctrl->setColorCurrentTheme(color, index.row());

    auto theme= m_ctrl->currentTheme();
    if(nullptr == theme)
        return;
    // TODO
}

void PreferencesDialog::updateTheme()
{
    RolisteamTheme* theme= m_ctrl->currentTheme();

    ui->m_themeNameLineEdit->setText(theme->getName());
    ui->m_backgroundImage->setPath(theme->getBackgroundImage());

    ui->m_bgColorPush->blockSignals(true);
    ui->m_bgColorPush->setColor(theme->getBackgroundColor());
    ui->m_bgColorPush->blockSignals(false);

    ui->m_positioningComboBox->blockSignals(true);
    ui->m_positioningComboBox->setCurrentIndex(theme->getBackgroundPosition());
    ui->m_positioningComboBox->blockSignals(false);

    QString defaultStyle= theme->getStyleName();

    ui->m_styleCombo->blockSignals(true);
    ui->m_styleCombo->setCurrentIndex(ui->m_styleCombo->findText(defaultStyle.toUpper(), Qt::MatchContains));
    ui->m_styleCombo->blockSignals(false);

    ui->m_diceHighlightColorBtn->setColor(theme->getDiceHighlightColor());

    // qApp->setStyle(theme->getStyle());
    // qApp->setPalette(theme->getPalette());
    // applyBackground();
    // qApp->setStyleSheet(theme->getCss());
}
void PreferencesDialog::setStyle()
{
    auto style= QStyleFactory::create(ui->m_styleCombo->currentText());
    m_ctrl->setCurrentThemeStyle(style);
    // qApp->setStyle(style);
}
void PreferencesDialog::editCss()
{
    auto theme= m_ctrl->currentTheme();

    if(nullptr == theme)
        return;

    bool ok= false;
    QString text= QInputDialog::getMultiLineText(this, tr("Css Editor"), tr("Css"), theme->getCss(), &ok);
    if(ok)
    {
        m_ctrl->setCurrentThemeCss(text);
        // qApp->setStyleSheet(theme->getCss());
    }
}

void PreferencesDialog::dupplicateTheme(bool selectNew)
{
    int i= ui->m_themeComboBox->currentIndex();
    m_ctrl->dupplicateTheme(i, selectNew);
}
void PreferencesDialog::setTitleAtCurrentTheme()
{
    m_ctrl->setCurrentThemeTitle(ui->m_themeNameLineEdit->text());
}
void PreferencesDialog::backgroundChanged()
{
    m_ctrl->setCurrentThemeBackground(ui->m_backgroundImage->path(), ui->m_positioningComboBox->currentIndex(),
                                      ui->m_bgColorPush->color());
    // applyBackground();setCurrentThemeBackground
}

void PreferencesDialog::performDiag()
{
    ui->m_diagDisplay->clear();

    QList<QByteArray> format(QImageReader::supportedImageFormats());

    QString htmlResult;
    QString linePattern= tr("<li>Image Format : %1 value %2</li>");
    htmlResult= tr("<h2>Supported Image Formats:</h2><ul>");
    QString result= "";
    for(int i= 0; i < format.size(); ++i)
    {
        result+= linePattern.arg(i).arg(QString(format.at(i)));
    }
    htmlResult+= result;
    htmlResult+= tr("</ul>End of Image Format");
    result= "";
    QFontDatabase database;
    htmlResult+= tr("<h2>Font families:</h2><ul>");
    linePattern= "<li>%1</li>";
    for(auto& family : database.families())
    {
        result+= linePattern.arg(family);
    }
    htmlResult+= result;
    htmlResult+= tr("</ul>End of Font families");
#ifdef HAVE_SOUND
    result= "";
    htmlResult+= tr("<h2>Audio file formats Support:</h2><ul>");
    QStringList commonAudioMimeType;
    commonAudioMimeType << "audio/basic"
                        << "audio/L24"
                        << "audio/mid"
                        << "audio/mpeg"
                        << "audio/mp4"
                        << "audio/x-aiff"
                        << "audio/x-mpegurl"
                        << "audio/vnd.rn-realaudio"
                        << "audio/ogg"
                        << "audio/vorbis"
                        << "audio/vnd.wav"
                        << "audio/x-ms-wma"
                        << "audio/x-wav"
                        << "audio/wav"
                        << "audio/webm"
                        << "audio/flac"; //
                                         /*  for(const QString &type : commonAudioMimeType)
                                           {
                                               switch (QMediaPlayer::hasSupport(type))
                                               {
                                               case QMultimedia::NotSupported:
                                                   result+=tr("<li>Unsupported format: %1</li>").arg(type);
                                                   break;
                                               case QMultimedia::MaybeSupported:
                                                   result+=tr("<li>Maybe supported format: %1</li>").arg(type);
                                                   break;
                                               case QMultimedia::ProbablySupported:
                                               case QMultimedia::PreferredService:
                                                   result+= tr("<li>Supported format: %1</li>").arg(type);
                                                   break;
                                               }
                                           }*/
    htmlResult+= result;
    htmlResult+= tr("</ul>End of Supported Audio file formats");

#endif
    ui->m_diagDisplay->setHtml(htmlResult);
}

void PreferencesDialog::testAliasCommand()
{
    // ui->m_convertedCmdEdit->setText(m_ctrl->convertAlias(ui->m_cmdDiceEdit->text()));
}

void PreferencesDialog::exportTheme()
{
    int i= ui->m_themeComboBox->currentIndex();
    auto name= m_ctrl->themeName(i);

    QString pathExport= QFileDialog::getSaveFileName(
        this, tr("Export Rolisteam Theme"),
        m_preferences->value("ThemeDirectory", QDir::homePath()).toString().append("/%1.rskin").arg(name),
        tr("Rolisteam Theme: %1").arg("*.rskin"));

    m_ctrl->exportTheme(pathExport, i);
}

bool PreferencesDialog::importTheme()
{
    QString pathImport= QFileDialog::getOpenFileName(
        this, tr("Import Rolisteam Theme"), m_preferences->value("ThemeDirectory", QDir::homePath()).toString(),
        tr("Rolisteam Theme: %1").arg("*.rskin"));

    updateTheme();
    ui->m_themeComboBox->setCurrentIndex(ui->m_themeComboBox->count() - 1);

    return false;
}

void PreferencesDialog::deleteTheme()
{
    m_ctrl->removeTheme(ui->m_themeComboBox->currentIndex());
}

void PreferencesDialog::addDirectory()
{
    auto act= qobject_cast<QAction*>(sender());
    if(!act)
        return;

    auto id= act->data().toInt();

    auto dir= QFileDialog::getExistingDirectory(this, tr("Add new directory"), QDir::homePath());

    if(dir.isEmpty())
        return;

    auto list= m_musicDirectories[id]->stringList();
    list.append(dir);
    m_musicDirectories[id]->setStringList(list);
    save();
}

QModelIndex PreferencesDialog::currentIndexFromCurrentList(int i)
{
    QModelIndex index;
    switch(i)
    {
    case First:
        index= ui->m_directoriesList1->currentIndex();
        break;
    case Second:
        index= ui->m_directoriesList2->currentIndex();
        break;
    case Third:
        index= ui->m_directoriesList3->currentIndex();
        break;
    }
    return index;
}

void PreferencesDialog::removeDirectory()
{
    auto act= qobject_cast<QAction*>(sender());
    if(!act)
        return;

    auto id= act->data().toInt();
    auto idx= currentIndexFromCurrentList(id);
    if(!idx.isValid())
        return;

    m_musicDirectories[id]->removeRows(idx.row(), 1);
    save();
}

void PreferencesDialog::upDirectory()
{
    auto act= qobject_cast<QAction*>(sender());
    if(!act)
        return;

    auto id= act->data().toInt();
    auto idx= currentIndexFromCurrentList(id);
    if(!idx.isValid())
        return;
    auto src= idx.row();
    auto dest= src - 1;
    auto list= m_musicDirectories[id]->stringList();

    if(dest < 0 || src >= list.size())
        return;

    list.swapItemsAt(src, dest);
    m_musicDirectories[id]->setStringList(list);
    save();
}

void PreferencesDialog::downDirectory()
{
    auto act= qobject_cast<QAction*>(sender());
    if(!act)
        return;

    auto id= act->data().toInt();
    auto idx= currentIndexFromCurrentList(id);
    if(!idx.isValid())
        return;

    auto src= idx.row();
    auto dest= src + 1;
    auto list= m_musicDirectories[id]->stringList();

    if(src < 0 || dest >= list.size())
        return;

    list.swapItemsAt(src, dest);
    m_musicDirectories[id]->setStringList(list);
    save();
}
