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
#include "preferences/palettemodel.h"
#include "widgets/filepathdelegateitem.h"
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QImageReader>
#include <QInputDialog>
#include <QItemEditorCreatorBase>
#include <QJsonDocument>
#include <QPushButton>
#include <QStyleFactory>
#include <QVBoxLayout>

#ifdef HAVE_SOUND
#include <QMediaPlayer>
#endif

#include "ui_preferencesdialogbox.h"

#include "diceparser.h"
#include "map/newemptymapdialog.h"

CheckBoxDelegate::CheckBoxDelegate(bool aRedCheckBox, QObject* parent)
{
    Q_UNUSED(aRedCheckBox)
    Q_UNUSED(parent)
    m_editor= new CenteredCheckBox();
    // m_editor->setParent(parent);
    connect(m_editor, SIGNAL(commitEditor()), this, SLOT(commitEditor()));
}

QWidget* CheckBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    CenteredCheckBox* cb= new CenteredCheckBox(parent);
    return cb;
}
void CheckBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    CenteredCheckBox* cb= qobject_cast<CenteredCheckBox*>(editor);
    bool checked= index.data().toBool();
    cb->setCheckedDelegate(!checked);
}
void CheckBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    CenteredCheckBox* cb= qobject_cast<CenteredCheckBox*>(editor);
    model->setData(index, cb->isCheckedDelegate());
    QStyledItemDelegate::setEditorData(editor, index);
}
QSize CheckBoxDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize a= QStyledItemDelegate::sizeHint(option, index);
    a.setHeight(30);
    a.setWidth(150);
    return a;
}

void CheckBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();

    if(!index.isValid())
    {
        return;
    }

    // QStyleOptionViewItemV4 opt = option;
    // QStyledItemDelegate::initStyleOption(&option, index);

    QVariant var= index.data();

    bool checked= var.toBool();
    // QVariant color= index.data(Qt::BackgroundRole);

    if(option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    m_editor->setCheckedDelegate(checked);

    m_editor->resize(option.rect.size());
    m_editor->setAutoFillBackground(false);
    painter->translate(option.rect.topLeft());
    m_editor->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);

    painter->restore();
}
void CheckBoxDelegate::commitEditor()
{
    CenteredCheckBox* editor= qobject_cast<CenteredCheckBox*>(sender());
    //	std::cout<<"commitEditor "<<(editor==m_editor)<<"  "<<editor->isCheckedDelegate()<<std::endl;
    emit commitData(editor);
}

/*********************
 * ColorListEditor *
 *********************/

ColorListEditor::ColorListEditor(QWidget* widget) : QComboBox(widget)
{
    populateList();
}

QColor ColorListEditor::color() const
{
    return qvariant_cast<QColor>(itemData(currentIndex(), Qt::DecorationRole));
}

void ColorListEditor::setColor(QColor color)
{
    setCurrentIndex(findData(color, int(Qt::DecorationRole)));
    emit colorChanged();
}

void ColorListEditor::populateList()
{
    QStringList colorNames= QColor::colorNames();

    for(int i= 0; i < colorNames.size(); ++i)
    {
        QColor color(colorNames[i]);

        insertItem(i, colorNames[i]);
        setItemData(i, color, Qt::DecorationRole);
    }
}

/*********************
 * ColorDelegate *
 *********************/

ColorDelegate::ColorDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

QWidget* ColorDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return new ColorListEditor(parent);
}

void ColorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    Q_UNUSED(index)
    ColorListEditor* cb= qobject_cast<ColorListEditor*>(editor);
    QColor checked= index.data().value<QColor>();
    cb->setColor(checked);
}

void ColorDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    ColorListEditor* cb= qobject_cast<ColorListEditor*>(editor);
    if(nullptr != cb)
    {
        QColor color= cb->color();
        model->setData(index, color);
    }
}

/*********************
 * PreferencesDialog *
 *********************/

PreferencesDialog::PreferencesDialog(PreferencesController* controller, QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f), ui(new Ui::PreferencesDialogBox()), m_ctrl(controller)
{
    ui->setupUi(this);
    m_currentThemeIsEditable= false;
    NewEmptyMapDialog dialog;
    ui->m_defaultMapModeCombo->addItems(dialog.getPermissionData());

    m_preferences= PreferencesManager::getInstance();

    ui->m_tableViewAlias->setModel(m_ctrl->diceAliasModel());
    /*m_diceParser= new DiceParser();
    m_aliasModel->setAliases(m_diceParser->getAliases());*/

    ui->m_stateView->setModel(m_ctrl->characterStateModel());
    ui->m_themeComboBox->setModel(m_ctrl->themeModel());
    connect(ui->m_themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int pos) { m_ctrl->setCurrentThemeIndex(static_cast<std::size_t>(pos)); });

    QHeaderView* horizontalHeader= ui->m_tableViewAlias->horizontalHeader();
    horizontalHeader->setSectionResizeMode(DiceAliasModel::PATTERN, QHeaderView::ResizeToContents);
    horizontalHeader->setSectionResizeMode(DiceAliasModel::VALUE, QHeaderView::Stretch);
    horizontalHeader->setSectionResizeMode(DiceAliasModel::METHOD, QHeaderView::ResizeToContents);
    horizontalHeader->setSectionResizeMode(DiceAliasModel::COMMENT, QHeaderView::ResizeToContents);
    ui->m_tableViewAlias->setItemDelegateForColumn(DiceAliasModel::METHOD, new CheckBoxDelegate());
    ui->m_tableViewAlias->setItemDelegateForColumn(DiceAliasModel::DISABLE, new CheckBoxDelegate());

    // m_paletteModel->setPalette(palette());
    ui->m_paletteTableView->setModel(m_ctrl->paletteModel());
    horizontalHeader= ui->m_paletteTableView->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(this, &PreferencesDialog::accepted, m_ctrl, &PreferencesController::savePreferences);

    connect(ui->m_startDiag, SIGNAL(clicked()), this, SLOT(performDiag()));
    // ui->m_fogColor->setTransparency(true);

    // set general panel as default.
    ui->tabWidget->setCurrentIndex(0);

    // aliases
    connect(ui->m_addDiceAliasAct, &QToolButton::clicked, m_ctrl, &PreferencesController::addAlias);
    connect(ui->m_delDiceAliasAct, &QToolButton::clicked, this,
            [this]() { m_ctrl->deleteAlias(ui->m_tableViewAlias->currentIndex()); });
    connect(ui->m_upDiceAliasAct, &QToolButton::clicked, this,
            [this]() { m_ctrl->moveAlias(ui->m_tableViewAlias->currentIndex(), PreferencesController::UP); });
    connect(ui->m_downDiceAliasAct, &QToolButton::clicked, this,
            [this]() { m_ctrl->moveAlias(ui->m_tableViewAlias->currentIndex(), PreferencesController::DOWN); });
    connect(ui->m_topDiceAliasAct, &QToolButton::clicked, this,
            [this]() { m_ctrl->moveAlias(ui->m_tableViewAlias->currentIndex(), PreferencesController::TOP); });
    connect(ui->m_bottomDiceAliasAct, &QToolButton::clicked, this,
            [this]() { m_ctrl->moveAlias(ui->m_tableViewAlias->currentIndex(), PreferencesController::BOTTOM); });
    connect(ui->m_testPushButton, &QToolButton::clicked, this, &PreferencesDialog::testAliasCommand);

    connect(ui->m_importDiceBtn, &QPushButton::clicked, this, [this]() {
        auto filename= QFileDialog::getOpenFileName(this, tr("Import Dice Aliases or States"),
                                                    m_preferences->value("DataDirectory", QDir::homePath()).toString(),
                                                    tr("Supported Rule files (*.rr *.json)"));
        m_ctrl->importData(filename);
    });

    connect(ui->m_exportDiceBtn, &QPushButton::clicked, this, [=]() {
        auto filename= QFileDialog::getSaveFileName(this, tr("Export Dice Aliases or States"),
                                                    m_preferences->value("DataDirectory", QDir::homePath()).toString(),
                                                    tr("Supported Rule files (*.rr *.json)"));
        if(filename.isEmpty())
            return;

        if(!filename.endsWith(QStringLiteral(".json")) && !filename.endsWith(QStringLiteral(".rr")))
            filename.append(QStringLiteral(".rr"));
        m_ctrl->exportData(filename);
    });

    // States
    connect(ui->m_addCharacterStateAct, &QToolButton::clicked, m_ctrl, &PreferencesController::addState);
    connect(ui->m_delCharceterStateAct, &QToolButton::clicked, m_ctrl,
            [this]() { m_ctrl->deleteState(ui->m_stateView->currentIndex()); });
    connect(ui->m_upCharacterStateAct, &QToolButton::clicked, this,
            [this]() { m_ctrl->moveState(ui->m_stateView->currentIndex(), PreferencesController::UP); });

    connect(ui->m_downCharacterStateAct, &QToolButton::clicked, this,
            [this]() { m_ctrl->moveState(ui->m_stateView->currentIndex(), PreferencesController::DOWN); });
    connect(ui->m_topCharacterStateAct, &QToolButton::clicked, this,
            [this]() { m_ctrl->moveState(ui->m_stateView->currentIndex(), PreferencesController::TOP); });
    connect(ui->m_bottomCharacterStateAct, &QToolButton::clicked, this,
            [this]() { m_ctrl->moveState(ui->m_stateView->currentIndex(), PreferencesController::BOTTOM); });

    connect(ui->m_highLightPenWidth, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [=]() { m_preferences->registerValue("VMAP::highlightPenWidth", ui->m_highLightPenWidth->value(), true); });
    connect(ui->m_mapItemHighlightColor, &ColorButton::colorChanged, this, [=]() {
        m_preferences->registerValue("VMAP::highlightColor", ui->m_mapItemHighlightColor->color(), true);
    });

    connect(ui->m_hideTipsOfTheDay, &QCheckBox::clicked, this, [=]() {
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
    connect(ui->m_backgroundImage, SIGNAL(pathChanged()), this, SLOT(backgroundChanged()));
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

    ui->m_stateView->setItemDelegateForColumn(CharacterStateModel::COLOR, new ColorDelegate(this));
    ui->m_stateView->setItemDelegateForColumn(CharacterStateModel::PICTURE, new FilePathDelegateItem(this));
}

PreferencesDialog::~PreferencesDialog()
{
    if(nullptr != m_diceParser)
    {
        delete m_diceParser;
        m_diceParser= nullptr;
    }
}

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

void PreferencesDialog::show()
{
    load();
    QDialog::show();
}
void PreferencesDialog::save() const
{
    m_preferences->registerValue("MusicDirectoryPlayer_0", ui->m_musicDirPath->path());
    m_preferences->registerValue("MusicDirectoryPlayer_1", ui->m_musicDirPath2->path());
    m_preferences->registerValue("MusicDirectoryPlayer_2", ui->m_musicDirPath3->path());
    m_preferences->registerValue("ImageDirectory", ui->m_pictureDir->path());
    m_preferences->registerValue("MapDirectory", ui->m_mapDir->path());
    m_preferences->registerValue("SessionDirectory", ui->m_scenarioDir->path());
    m_preferences->registerValue("MinutesDirectory", ui->m_minuteDir->path());
    m_preferences->registerValue("ChatDirectory", ui->m_chatDir->path());
    m_preferences->registerValue("currentTranslationFile", ui->m_translationFileEdit->path());
    m_preferences->registerValue("MainWindow::MustBeChecked", ui->m_checkUpdate->isChecked());
    m_preferences->registerValue("defaultPermissionMap", ui->m_defaultMapModeCombo->currentIndex());
    m_preferences->registerValue("CharacterSheetDirectory", ui->m_characterSheetDir->path());

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
    m_ctrl->savePreferences();
}
void PreferencesDialog::load()
{
    // Direcotry PATH
    ui->m_translationFileEdit->setMode(false);
    ui->m_translationFileEdit->setFilter("Translation File: (*.qm)");
    ui->m_musicDirPath->setPath(m_preferences->value("MusicDirectoryPlayer_0", QDir::homePath()).toString());
    ui->m_musicDirPath2->setPath(m_preferences->value("MusicDirectoryPlayer_1", QDir::homePath()).toString());
    ui->m_musicDirPath3->setPath(m_preferences->value("MusicDirectoryPlayer_2", QDir::homePath()).toString());
    ui->m_pictureDir->setPath(m_preferences->value("ImageDirectory", QDir::homePath()).toString());
    ui->m_mapDir->setPath(m_preferences->value("MapDirectory", QDir::homePath()).toString());
    ui->m_scenarioDir->setPath(m_preferences->value("SessionDirectory", QDir::homePath()).toString());
    ui->m_minuteDir->setPath(m_preferences->value("MinutesDirectory", QDir::homePath()).toString());
    ui->m_chatDir->setPath(m_preferences->value("ChatDirectory", QDir::homePath()).toString());
    ui->m_characterSheetDir->setPath(m_preferences->value("CharacterSheetDirectory", QDir::homePath()).toString());

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

    m_ctrl->loadPreferences();
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
    qApp->setPalette(theme->getPalette());
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

    qApp->setStyle(theme->getStyle());
    qApp->setPalette(theme->getPalette());
    // applyBackground();
    qApp->setStyleSheet(theme->getCss());
}
void PreferencesDialog::setStyle()
{
    auto style= QStyleFactory::create(ui->m_styleCombo->currentText());
    m_ctrl->setCurrentThemeStyle(style);
    qApp->setStyle(style);
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
        qApp->setStyleSheet(theme->getCss());
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
    // applyBackground();
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
    QString result= m_diceParser->convertAlias(ui->m_cmdDiceEdit->text());
    ui->m_convertedCmdEdit->setText(result);
}

void PreferencesDialog::exportTheme()
{
    int i= ui->m_themeComboBox->currentIndex();
    auto name= m_ctrl->themeName(i);

    QString pathExport= QFileDialog::getSaveFileName(
        this, tr("Export Rolisteam Theme"),
        m_preferences->value("ThemeDirectory", QDir::homePath()).toString().append("/%1.rskin").arg(name),
        tr("Rolisteam Theme: %1").arg("*.rskin"))

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
