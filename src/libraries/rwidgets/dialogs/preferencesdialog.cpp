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
#include <media/mediatype.h>

#ifdef HAVE_SOUND
#include <QMediaPlayer>
#endif

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

    ui->m_defaultMapModeCombo->addItem(tr("GM Only"), Core::PermissionMode::GM_ONLY);
    ui->m_defaultMapModeCombo->addItem(tr("PC Move"), Core::PermissionMode::PC_MOVE);
    ui->m_defaultMapModeCombo->addItem(tr("PC All"), Core::PermissionMode::PC_ALL);

    connect(m_ctrl, &PreferencesController::preferencesChanged, this,
            [this]()
            {
                m_preferences= m_ctrl->preferences();
                load();
            });
    m_preferences= m_ctrl->preferences();
    load();

    ui->m_themeComboBox->setModel(m_ctrl->themeModel());
    connect(ui->m_themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int pos) { m_ctrl->setCurrentThemeIndex(static_cast<std::size_t>(pos)); });
    // m_paletteModel->setPalette(palette());
    connect(m_ctrl, &PreferencesController::currentThemeIndexChanged, this,
            [this]()
            {
                if(m_ctrl->currentTheme())
                    ui->m_paletteTableView->setModel(m_ctrl->currentTheme()->paletteModel());
            });

    if(m_ctrl->currentTheme())
    {
        ui->m_paletteTableView->setModel(m_ctrl->currentTheme()->paletteModel());
        auto horizontalHeader= ui->m_paletteTableView->horizontalHeader();
        horizontalHeader->setSectionResizeMode(0, QHeaderView::Stretch);
    }

    connect(this, &PreferencesDialog::accepted, this, &PreferencesDialog::save);

    connect(ui->m_startDiag, &QPushButton::clicked, this, &PreferencesDialog::performDiag);
    // i18n
    connect(ui->m_systemTranslation, &QCheckBox::clicked, this, &PreferencesDialog::updateTranslationPref);
    connect(ui->m_customTranslation, &QCheckBox::clicked, this, &PreferencesDialog::updateTranslationPref);

    connect(ui->m_systemTranslation, &QCheckBox::toggled, m_ctrl, &PreferencesController::setSystemLang);
    connect(ui->m_customTranslation, &QCheckBox::toggled, m_ctrl, &PreferencesController::setHasCustomFile);
    connect(ui->m_translationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), m_ctrl,
            &PreferencesController::setCurrentLangIndex);
    connect(ui->m_translationFileEdit, &FileDirChooser::pathChanged, m_ctrl,
            [this](const QUrl& url) { m_ctrl->setCustomFile(url.toLocalFile()); });

    ui->m_translationSelector->setModel(m_ctrl->languageModel());
    // set general panel as default.
    ui->tabWidget->setCurrentIndex(0);
    connect(ui->m_highLightPenWidth, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [=]()
            {
                if(!m_preferences)
                    return;
                m_preferences->registerValue("VMAP::highlightPenWidth", ui->m_highLightPenWidth->value(), true);
            });
    connect(ui->m_mapItemHighlightColor, &ColorButton::colorChanged, this,
            [=]()
            {
                if(!m_preferences)
                    return;
                m_preferences->registerValue("VMAP::highlightColor", ui->m_mapItemHighlightColor->color(), true);
            });

    connect(ui->m_hideTipsOfTheDay, &QCheckBox::clicked, this,
            [this]()
            {
                if(!m_preferences)
                    return;
                m_preferences->registerValue("MainWindow::neverDisplayTips", ui->m_hideTipsOfTheDay->isChecked(),
                                             false);
            });
    // Misc
    setSizeGripEnabled(true);
    if(!m_preferences)
        return;
    setWindowTitle(
        QString("%1 - %2").arg(m_preferences->value("Application_Name", "rolisteam").toString(), tr("Preferences")));
    setWindowModality(Qt::ApplicationModal);

    connect(ui->m_heartBeat, SIGNAL(clicked(bool)), this, SLOT(manageHeartBeat()));
    connect(ui->m_hbFrequency, SIGNAL(valueChanged(int)), this, SLOT(manageHeartBeat()));
    connect(ui->m_displayTimePage, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
            [=](int val) { m_preferences->registerValue("waitingTimeBetweenPage", val, true); });
    // Messaging
    connect(ui->m_saveChatRoomCB, &QCheckBox::toggled, this, &PreferencesDialog::manageMessagingPref);

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

    connect(ui->m_paletteTableView, &QTableView::doubleClicked, this, &PreferencesDialog::editColor);

    connect(ui->m_cssEdit, SIGNAL(clicked()), this, SLOT(editCss()));
    connect(ui->m_exportBtn, SIGNAL(clicked()), this, SLOT(exportTheme()));
    connect(ui->m_importBtn, SIGNAL(clicked()), this, SLOT(importTheme()));
    connect(ui->m_deleteTheme, SIGNAL(clicked()), this, SLOT(deleteTheme()));

    connect(m_ctrl, &PreferencesController::currentLangIndexChanged, this,
            [this]() { ui->m_translationSelector->setCurrentIndex(m_ctrl->currentLangIndex()); });

    updateTranslationPref();
}

PreferencesDialog::~PreferencesDialog() {}

void PreferencesDialog::manageHeartBeat()
{
    if(!m_preferences)
        return;
    m_preferences->registerValue("HeartBeatStatus", ui->m_heartBeat->isChecked());
    m_preferences->registerValue("HbFrequency", ui->m_hbFrequency->value());
}
void PreferencesDialog::manageMessagingPref()
{
    if(!m_preferences)
        return;
    m_preferences->registerValue("Messaging::SaveChatrooms", ui->m_saveChatRoomCB->isChecked());
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
    if(!m_preferences)
        return;

    // messaging
    m_preferences->registerValue("Messaging::SaveChatrooms", ui->m_saveChatRoomCB->isChecked());

    // General
    QColor color;
    int opacity= ui->m_opacitySlider->value();
    color.setRgb(opacity, opacity, opacity);
    m_preferences->registerValue("Fog_color", color);
    m_preferences->registerValue("Mask_color", ui->m_fogColor->color());
    m_preferences->registerValue("PictureAdjust", ui->m_pictureAdjust->isChecked());
    m_preferences->registerValue("FullScreenAtStarting", ui->m_fullScreenCheckbox->isChecked());
    m_preferences->registerValue("shortNameInTabMode", ui->m_shortNameCb->isChecked());
    m_preferences->registerValue("MaxLengthTabName", ui->m_tabTitleLength->value());

    m_preferences->registerValue("waitingTimeBetweenPage", ui->m_displayTimePage->value());

    // theme
    m_preferences->registerValue("currentTheme", ui->m_themeComboBox->currentText());

    // external tools
    m_preferences->registerValue("RcsePath", ui->m_rcsePath->localFile());
    m_preferences->registerValue("MindmapPath", ui->m_mindmapPath->localFile());
    m_preferences->registerValue("textEditorPath", ui->m_textEditorPath->localFile());

    m_preferences->registerValue("RcseParam", ui->m_rcseParams->text());
    m_preferences->registerValue("MindmapParam", ui->m_mindmapParam->text());
    m_preferences->registerValue("textEditorParam", ui->m_textEditorParam->text());

    m_ctrl->savePreferences();
}
void PreferencesDialog::load()
{
    if(!m_preferences)
        return;

    // Direcotry PATH
    ui->m_translationFileEdit->setMode(FileDirChooser::OpenExistingFile);
    ui->m_translationFileEdit->setFilter("Translation File: (*.qm)");

    ui->m_translationFileEdit->setUrl(
        QUrl::fromUserInput(m_preferences->value("currentTranslationFile", "").toString()));
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
    ui->m_translationFileEdit->setUrl(QUrl::fromUserInput(m_ctrl->customFilePath()));

    ////////////////////////
    // MAP
    ///////////////////////
    // Fog of WAR
    QColor fog= m_preferences->value("Fog_color", QColor(Qt::black)).value<QColor>();
    ui->m_opacitySlider->setValue(m_preferences->value("Fog_opacity", fog.red()).toInt());
    ui->m_opacitySpin->setValue(m_preferences->value("Fog_opacity", fog.red()).toInt());
    ui->m_fogColor->setColor(m_preferences->value("Mask_color", QColor(Qt::darkMagenta)).value<QColor>());
    ui->m_fullScreenCheckbox->setChecked(m_preferences->value("FullScreenAtStarting", true).toBool());

    // Default Permission
    ui->m_defaultMapModeCombo->setCurrentIndex(
        ui->m_defaultMapModeCombo->findData(m_preferences->value("defaultPermissionMap", 0).toInt()));
    ui->m_pictureAdjust->setChecked(m_preferences->value("PictureAdjust", true).toBool());

    // theme
    // initializeStyle();
    ui->m_backgroundImage->setMode(FileDirChooser::OpenExistingFile);
    ui->m_backgroundImage->setFilter(tr("Images (*.png *.xpm *.jpg *.gif *.bmp)"));

    ui->m_saveChatRoomCB->setChecked(m_preferences->value("Messaging::SaveChatrooms", false).toBool());

    // m_ctrl->loadPreferences();
    updateTheme();

    QString pExt;
#if Q_OS_WIN
    pExt= ".exe";
#endif

    QString path= QString("%1/rcse%2").arg(QCoreApplication::applicationDirPath(), pExt);
    if(!QFileInfo::exists(path))
        path= QString();
    ui->m_rcsePath->setUrl(m_preferences->value("RcsePath", path).toString());
    ui->m_rcsePath->setMode(FileDirChooser::OpenExistingFile);
    ui->m_rcseParams->setText(m_preferences->value("RcseParam", "-f %1").toString());

    path= QString("%1/mindmapbin%2").arg(QCoreApplication::applicationDirPath(), pExt);
    if(!QFileInfo::exists(path))
        path= QString();
    ui->m_mindmapPath->setUrl(m_preferences->value("MindmapPath", path).toString());
    ui->m_mindmapPath->setMode(FileDirChooser::OpenExistingFile);
    ui->m_mindmapParam->setText(m_preferences->value("MindmapParam", "-f %1").toString());

    path= QString();
    ui->m_textEditorPath->setUrl(m_preferences->value("textEditorPath", path).toString());
    ui->m_textEditorPath->setMode(FileDirChooser::OpenExistingFile);
    ui->m_textEditorParam->setText(m_preferences->value("textEditorParam", "%1").toString());
}

void PreferencesDialog::editColor(const QModelIndex& index)
{
    QColor color= QColorDialog::getColor(index.data(Qt::DecorationRole).value<QColor>(), this);
    if(!color.isValid())
        return;

    m_ctrl->setColorCurrentTheme(color, index.row());
}

void PreferencesDialog::updateTheme()
{
    RolisteamTheme* theme= m_ctrl->currentTheme();

    ui->m_themeNameLineEdit->setText(theme->getName());
    ui->m_backgroundImage->setUrl(QUrl::fromUserInput(theme->getBackgroundImage()));

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
    m_ctrl->setCurrentThemeBackground(ui->m_backgroundImage->url().toLocalFile(),
                                      ui->m_positioningComboBox->currentIndex(), ui->m_bgColorPush->color());
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
