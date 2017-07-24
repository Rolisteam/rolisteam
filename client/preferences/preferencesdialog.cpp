/*************************************************************************
 *     Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
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

#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QImageReader>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QInputDialog>
#include <QJsonDocument>
#include <QItemEditorCreatorBase>
#include "widgets/filepathdelegateitem.h"

#ifdef HAVE_SOUND
#include <QMediaPlayer>
#endif

#include "ui_preferencesdialogbox.h"

#include "map/newemptymapdialog.h"
#include "diceparser/diceparser.h"
#define GRAY_SCALE 191


CheckBoxDelegate::CheckBoxDelegate(bool aRedCheckBox, QObject *parent)
{
    m_editor = new CenteredCheckBox();
    connect( m_editor, SIGNAL(commitEditor()),
             this, SLOT(commitEditor()) );
}

QWidget* CheckBoxDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    CenteredCheckBox* cb = new CenteredCheckBox(parent);
    return cb;
}
void CheckBoxDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
    CenteredCheckBox* cb = qobject_cast<CenteredCheckBox*>(editor);
    bool checked = index.data().toBool();
    cb->setCheckedDelegate(!checked);
}
void CheckBoxDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
    CenteredCheckBox* cb = qobject_cast<CenteredCheckBox*>(editor);
    model->setData(index,cb->isCheckedDelegate());
    QStyledItemDelegate::setEditorData(editor, index);
}
QSize CheckBoxDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QSize a = QStyledItemDelegate::sizeHint(option,index);
    a.setHeight(30);
    a.setWidth(150);
    return a;
}

void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    if (!index.isValid())
    {
        return;
    }

    QStyleOptionViewItemV4 opt = option;
    QStyledItemDelegate::initStyleOption(&opt, index);

    QVariant var= index.data();

    bool checked = var.toBool();
    QVariant color= index.data(Qt::BackgroundRole);

    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    m_editor->setCheckedDelegate(checked);

    m_editor->resize( option.rect.size() );
    m_editor->setAutoFillBackground(false);
    painter->translate(option.rect.topLeft());
    m_editor->render(painter, QPoint(), QRegion(), QWidget::DrawChildren );


    painter->restore();
}
void CheckBoxDelegate::commitEditor()
{
    CenteredCheckBox *editor = qobject_cast<CenteredCheckBox *>(sender());
    //	std::cout<<"commitEditor "<<(editor==m_editor)<<"  "<<editor->isCheckedDelegate()<<std::endl;
    emit commitData(editor);
}

/*********************
 * ColorListEditor *
 *********************/

ColorListEditor::ColorListEditor(QWidget *widget) : QComboBox(widget)
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
}

void ColorListEditor::populateList()
{
    QStringList colorNames = QColor::colorNames();

    for (int i = 0; i < colorNames.size(); ++i)
    {
        QColor color(colorNames[i]);

        insertItem(i, colorNames[i]);
        setItemData(i, color, Qt::DecorationRole);
    }
}

/*********************
 * ColorDelegate *
 *********************/


ColorDelegate::ColorDelegate( QObject* parent )
{

}

QWidget* ColorDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    return new ColorListEditor(parent);
}

void ColorDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
    ColorListEditor* cb = qobject_cast<ColorListEditor*>(editor);
    QColor checked = index.data().value<QColor>();
    cb->setColor(checked);
}

void ColorDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
    ColorListEditor* cb = qobject_cast<ColorListEditor*>(editor);
    if(NULL!=cb)
    {
        QColor color = cb->color();
        model->setData(index,color);
    }
}


/*********************
 * PreferencesDialog *
 *********************/

PreferencesDialog::PreferencesDialog(QWidget * parent, Qt::WindowFlags f)
    : QDialog(parent, f), ui(new Ui::PreferencesDialogBox())
{
    ui->setupUi(this);
    m_currentThemeIsEditable = false;
    NewEmptyMapDialog dialog;
    ui->m_defaultMapModeCombo->addItems(dialog.getPermissionData());

    m_preferences = PreferencesManager::getInstance();

    m_aliasModel = new DiceAliasModel();
    ui->m_tableViewAlias->setModel(m_aliasModel);
    m_diceParser = new DiceParser();
    m_aliasModel->setAliases(m_diceParser->getAliases());


    m_stateModel = new CharacterStateModel();
    ui->m_stateView->setModel(m_stateModel);

    QHeaderView* horizontalHeader = ui->m_tableViewAlias->horizontalHeader();
    horizontalHeader->setSectionResizeMode(DiceAliasModel::PATTERN,QHeaderView::ResizeToContents);
    horizontalHeader->setSectionResizeMode(DiceAliasModel::VALUE,QHeaderView::Stretch);
    horizontalHeader->setSectionResizeMode(DiceAliasModel::METHOD,QHeaderView::ResizeToContents);
     horizontalHeader->setSectionResizeMode(DiceAliasModel::COMMENT,QHeaderView::ResizeToContents);
    ui->m_tableViewAlias->setItemDelegateForColumn(DiceAliasModel::METHOD,new CheckBoxDelegate());
    ui->m_tableViewAlias->setItemDelegateForColumn(DiceAliasModel::DISABLE,new CheckBoxDelegate());

    m_paletteModel = new PaletteModel();
    m_paletteModel->setPalette(palette());
    ui->m_paletteTableView->setModel(m_paletteModel);
    horizontalHeader = ui->m_paletteTableView->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0,QHeaderView::Stretch);

    connect(this, SIGNAL(accepted()), this, SLOT(save()));

    connect(ui->m_startDiag,SIGNAL(clicked()),this,SLOT(performDiag()));
    //ui->m_fogColor->setTransparency(true);

    //set general panel as default.
    ui->tabWidget->setCurrentIndex(0);

    //aliases
    connect(ui->m_addDiceAliasAct,SIGNAL(clicked()),this,SLOT(manageAliasAction()));
    connect(ui->m_delDiceAliasAct,SIGNAL(clicked()),this,SLOT(manageAliasAction()));
    connect(ui->m_upDiceAliasAct,SIGNAL(clicked()),this,SLOT(manageAliasAction()));
    connect(ui->m_downDiceAliasAct,SIGNAL(clicked()),this,SLOT(manageAliasAction()));
    connect(ui->m_topDiceAliasAct,SIGNAL(clicked()),this,SLOT(manageAliasAction()));
    connect(ui->m_bottomDiceAliasAct,SIGNAL(clicked()),this,SLOT(manageAliasAction()));
    connect(ui->m_testPushButton,SIGNAL(clicked()),this,SLOT(testAliasCommand()));


    //States
    connect(ui->m_addCharacterStateAct,SIGNAL(clicked()),this,SLOT(manageStateAction()));
    connect(ui->m_delCharceterStateAct,SIGNAL(clicked()),this,SLOT(manageStateAction()));
    connect(ui->m_upCharacterStateAct,SIGNAL(clicked()),this,SLOT(manageStateAction()));
    connect(ui->m_downCharacterStateAct,SIGNAL(clicked()),this,SLOT(manageStateAction()));
    connect(ui->m_topCharacterStateAct,SIGNAL(clicked()),this,SLOT(manageStateAction()));
    connect(ui->m_bottomCharacterStateAct,SIGNAL(clicked()),this,SLOT(manageStateAction()));



    // Misc
    setSizeGripEnabled(true);
    setWindowTitle(QString("%1 - %2").arg(m_preferences->value("Application_Name","rolisteam").toString(),tr("Preferences")));
    setWindowModality(Qt::ApplicationModal);

    connect(ui->m_heartBeat,SIGNAL(clicked(bool)),this,SLOT(manageHeartBeat()));
    connect(ui->m_hbFrequency,SIGNAL(valueChanged(int)),this,SLOT(manageHeartBeat()));


    //Messaging
    connect(ui->m_showTimeCheckBox,SIGNAL(clicked(bool)),this,SLOT(manageMessagingPref()));
    connect(ui->m_timeColorBtn,SIGNAL(clicked(bool)),this,SLOT(manageMessagingPref()));
    connect(ui->m_maxLenghtCommand,SIGNAL(valueChanged(int)),this,SLOT(manageMessagingPref()));
    connect(ui->m_hideLongCommand,SIGNAL(toggled(bool)),this,SLOT(manageMessagingPref()));


    // background
    connect(ui->m_positioningComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(backgroundChanged()));
    connect(ui->m_bgColorPush, SIGNAL(colorChanged(QColor)), this, SLOT(backgroundChanged()));
    connect(ui->m_backgroundImage,SIGNAL(pathChanged()),this,SLOT(backgroundChanged()));
    //connect(ui->m_backgroundImage,ç)

    //themes
    connect(ui->m_copyThemeButton,SIGNAL(clicked()),this,SLOT(dupplicateTheme()));
    connect(ui->m_themeNameLineEdit,SIGNAL(textEdited(QString)),this,SLOT(setTitleAtCurrentTheme()));


    connect(ui->m_paletteTableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editColor(QModelIndex)));

    connect(ui->m_cssEdit,SIGNAL(clicked()),this,SLOT(editCss()));
    connect(ui->m_exportBtn,SIGNAL(clicked()),this,SLOT(exportTheme()));
    connect(ui->m_importBtn,SIGNAL(clicked()),this,SLOT(importTheme()));
    connect(ui->m_deleteTheme,SIGNAL(clicked()),this,SLOT(deleteTheme()));

    ui->m_stateView->setItemDelegateForColumn(CharacterStateModel::COLOR,new ColorDelegate());
    ui->m_stateView->setItemDelegateForColumn(CharacterStateModel::PICTURE,new FilePathDelegateItem());

}

PreferencesDialog::~PreferencesDialog()
{
    // QObject should do it right for us already.
}

void PreferencesDialog::manageHeartBeat()
{
    m_preferences->registerValue("HeartBeatStatus",ui->m_heartBeat->isChecked());
    m_preferences->registerValue("HbFrequency",ui->m_hbFrequency->value());
}
void PreferencesDialog::manageMessagingPref()
{
    m_preferences->registerValue("MessagingShowTime",ui->m_showTimeCheckBox->isChecked());
    m_preferences->registerValue("MessagingColorTime",ui->m_timeColorBtn->color());
    m_preferences->registerValue("maxSizeForCuttingDiceCmd",ui->m_maxLenghtCommand->value());
    m_preferences->registerValue("hideLongCommand",ui->m_hideLongCommand->isChecked());
}

void PreferencesDialog::updateUi(bool isGM)
{
    if(NULL!=m_aliasModel)
    {
        m_aliasModel->setGM(isGM);
        m_stateModel->setGM(isGM);
    }
}

void PreferencesDialog::show()
{
    load();
    QDialog::show();
}
void PreferencesDialog::save() const
{
    m_preferences->registerValue("MusicDirectoryGM",ui->m_musicDirGM->path());
    m_preferences->registerValue("MusicDirectoryPlayer",ui->m_musicDirPlayer->path());
    m_preferences->registerValue("ImageDirectory",ui->m_pictureDir->path());
    m_preferences->registerValue("MapDirectory",ui->m_mapDir->path());
    m_preferences->registerValue("SessionDirectory",ui->m_scenarioDir->path());
    m_preferences->registerValue("MinutesDirectory",ui->m_minuteDir->path());
    m_preferences->registerValue("ChatDirectory",ui->m_chatDir->path());
    m_preferences->registerValue("currentTranslationFile",ui->m_translationFileEdit->path());
    m_preferences->registerValue("MainWindow_MustBeChecked",ui->m_checkUpdate->isChecked());
    m_preferences->registerValue("defaultPermissionMap",ui->m_defaultMapModeCombo->currentIndex());
    m_preferences->registerValue("CharacterSheetDirectory",ui->m_characterSheetDir->path());

    //messaging
    m_preferences->registerValue("MessagingShowTime",ui->m_showTimeCheckBox->isChecked());
    m_preferences->registerValue("MessagingColorTime",ui->m_timeColorBtn->color());

    QColor color;
    int opacity=ui->m_opacitySlider->value();
    color.setRgb(opacity,opacity,opacity);
    m_preferences->registerValue("Fog_color", color);
    m_preferences->registerValue("Mask_color", ui->m_fogColor->color());
    m_preferences->registerValue("PictureAdjust",ui->m_pictureAdjust->isChecked());
    m_preferences->registerValue("FullScreenAtStarting",ui->m_fullScreenCheckbox->isChecked());
    m_preferences->registerValue("maxSizeForCuttingDiceCmd",ui->m_maxLenghtCommand->value());
    m_preferences->registerValue("hideLongCommand",ui->m_hideLongCommand->isChecked());

    //theme
    m_preferences->registerValue("currentTheme", ui->m_themeComboBox->currentText());
    m_preferences->registerValue("ThemeNumber",m_themes.size());
    int i = 0;

    foreach(RolisteamTheme* tmp, m_themes)
    {
        m_preferences->registerValue(QString("Theme_%1_name").arg(i),tmp->getName());
        QVariant var;
        var.setValue<QPalette>(tmp->getPalette());
        m_preferences->registerValue(QString("Theme_%1_palette").arg(i),var);
        m_preferences->registerValue(QString("Theme_%1_stylename").arg(i),tmp->getStyleName());
        m_preferences->registerValue(QString("Theme_%1_bgColor").arg(i),tmp->getBackgroundColor());
        m_preferences->registerValue(QString("Theme_%1_bgPath").arg(i),tmp->getBackgroundImage());
        m_preferences->registerValue(QString("Theme_%1_bgPosition").arg(i),tmp->getBackgroundPosition());
        m_preferences->registerValue(QString("Theme_%1_css").arg(i),tmp->getCss());
        m_preferences->registerValue(QString("Theme_%1_removable").arg(i),tmp->isRemovable());
        // m_preferences->registerValue(QString("Theme_%1_css").arg(i),tmp->getName());
        ++i;
    }

    //DiceSystem
    QList<DiceAlias*>* aliasList = m_aliasModel->getAliases();
    m_preferences->registerValue("DiceAliasNumber",aliasList->size());
    for(int i = 0; i < aliasList->size() ; ++i)
    {
        DiceAlias* tmpAlias = aliasList->at(i);
        m_preferences->registerValue(QString("DiceAlias_%1_command").arg(i),tmpAlias->getCommand());
        m_preferences->registerValue(QString("DiceAlias_%1_value").arg(i),tmpAlias->getValue());
        m_preferences->registerValue(QString("DiceAlias_%1_type").arg(i),tmpAlias->isReplace());
        m_preferences->registerValue(QString("DiceAlias_%1_enable").arg(i),tmpAlias->isEnable());
    }

    //State
    QList<CharacterState*>* stateList = m_stateModel->getCharacterStates();
    m_preferences->registerValue("CharacterStateNumber",stateList->size());
    for(int i = 0; i < stateList->size() ; ++i)
    {
        CharacterState* tmpState = stateList->at(i);
        m_preferences->registerValue(QString("CharacterState_%1_label").arg(i),tmpState->getLabel());
        m_preferences->registerValue(QString("CharacterState_%1_color").arg(i),tmpState->getColor());
        m_preferences->registerValue(QString("CharacterState_%1_pixmap").arg(i),tmpState->getImage());
    }


}
void PreferencesDialog::load()
{
    //Direcotry PATH
    ui->m_translationFileEdit->setMode(false);
    ui->m_translationFileEdit->setFilter("Translation File: (*.qm)");
    ui->m_musicDirGM->setPath(m_preferences->value("MusicDirectoryGM",QDir::homePath()).toString());
    ui->m_musicDirPlayer->setPath(m_preferences->value("MusicDirectoryPlayer",QDir::homePath()).toString());
    ui->m_pictureDir->setPath(m_preferences->value("ImageDirectory",QDir::homePath()).toString());
    ui->m_mapDir->setPath(m_preferences->value("MapDirectory",QDir::homePath()).toString());
    ui->m_scenarioDir->setPath(m_preferences->value("SessionDirectory",QDir::homePath()).toString());
    ui->m_minuteDir->setPath(m_preferences->value("MinutesDirectory",QDir::homePath()).toString());
    ui->m_chatDir->setPath(m_preferences->value("ChatDirectory",QDir::homePath()).toString());
    ui->m_characterSheetDir->setPath(m_preferences->value("CharacterSheetDirectory",QDir::homePath()).toString());

    ui->m_translationFileEdit->setPath(m_preferences->value("currentTranslationFile","").toString());
    ui->m_checkUpdate->setChecked(m_preferences->value("MainWindow_MustBeChecked",true).toBool());

    ui->m_heartBeat->setChecked(m_preferences->value("HeartBeatStatus",false).toBool());
    ui->m_hbFrequency->setValue(m_preferences->value("HbFrequency",60).toInt());

    ////////////////////////
    //MAP
    ///////////////////////
    //Fog of WAR
    QColor fog=m_preferences->value("Fog_color",QColor(Qt::black)).value<QColor>();
    ui->m_opacitySlider->setValue(m_preferences->value("Fog_opacity",fog.red()).toInt());
    ui->m_opacitySpin->setValue(m_preferences->value("Fog_opacity",fog.red()).toInt());
    ui->m_fogColor->setColor(m_preferences->value("Mask_color",QColor(Qt::darkMagenta)).value<QColor>());
    ui->m_fullScreenCheckbox->setChecked(m_preferences->value("FullScreenAtStarting",true).toBool());
    ui->m_maxLenghtCommand->setValue(m_preferences->value("maxSizeForCuttingDiceCmd",100).toInt());
    ui->m_hideLongCommand->setChecked(m_preferences->value("hideLongCommand",false).toBool());


    //Default Permission
    ui->m_defaultMapModeCombo->setCurrentIndex(m_preferences->value("defaultPermissionMap",0).toInt());
    ui->m_pictureAdjust->setChecked(m_preferences->value("PictureAdjust",true).toBool());


    //theme
    //initializeStyle();
    ui->m_backgroundImage->setMode(false);
    ui->m_backgroundImage->setFilter(tr("Images (*.png *.xpm *.jpg *.gif *.bmp)"));

    //Messaging
    ui->m_showTimeCheckBox->setChecked(m_preferences->value("MessagingShowTime",false).toBool());
    ui->m_timeColorBtn->setColor(m_preferences->value("MessagingColorTime",QColor(Qt::darkGreen)).value<QColor>());

    updateTheme();
}

void PreferencesDialog::editColor(QModelIndex index)
{
    QColor color = QColorDialog::getColor(index.data(Qt::DecorationRole).value<QColor>());
    if(color.isValid())
    {
        m_paletteModel->setColor(index,color);
        int i = ui->m_themeComboBox->currentIndex();
        if((i>=0)&&(i<m_themes.size()))
        {
            RolisteamTheme* theme = m_themes.at(i);
            theme->setPalette(m_paletteModel->getPalette());
            qApp->setPalette(theme->getPalette());
        }
    }
}

void PreferencesDialog::initializePostSettings()
{
    //theme
    int size = m_preferences->value("ThemeNumber",0).toInt();
    if(size>0)
    {
        for(int i = 0; i< size; ++i)
        {
            QString name = m_preferences->value(QString("Theme_%1_name").arg(i),QString()).toString();
            QPalette pal = m_preferences->value(QString("Theme_%1_palette").arg(i),QPalette()).value<QPalette>();
            QString style = m_preferences->value(QString("Theme_%1_stylename").arg(i),"fusion").toString();
            QColor color = m_preferences->value(QString("Theme_%1_bgColor").arg(i),QColor(GRAY_SCALE,GRAY_SCALE,GRAY_SCALE)).value<QColor>();
            QString path = m_preferences->value(QString("Theme_%1_bgPath").arg(i),"").toString();
            int pos = m_preferences->value(QString("Theme_%1_bgPosition").arg(i),0).toInt();
            QString css = m_preferences->value(QString("Theme_%1_css").arg(i),"").toString();
            bool isRemovable = m_preferences->value(QString("Theme_%1_removable").arg(i),false).toBool();
            RolisteamTheme* tmp = new RolisteamTheme(pal,name,css,QStyleFactory::create(style),path,pos,color,isRemovable);
            m_themes.append(tmp);
            // m_preferences->registerValue(QString("Theme_%1_css").arg(i),tmp->getName());
        }
    }
    else //theme provided by default
    {
        //normal
        m_themes.append(new RolisteamTheme(QPalette(),tr("default"),"",QStyleFactory::create("fusion"),":/resources/icons/workspacebackground.jpg",0,QColor(GRAY_SCALE,GRAY_SCALE,GRAY_SCALE),false));

        //DarkOrange
        QFile styleFile(":/stylesheet/resources/stylesheet/darkorange.qss");
        styleFile.open(QFile::ReadOnly);
        QByteArray bytes = styleFile.readAll();
        QString css(bytes);
        m_themes.append(new RolisteamTheme(QPalette(),tr("darkorange"),css,QStyleFactory::create("fusion"),":/resources/icons/workspacebackground.jpg",0,QColor(GRAY_SCALE,GRAY_SCALE,GRAY_SCALE),false));

        //DarkFusion
        QPalette palette;
        palette.setColor(QPalette::Window, QColor(53,53,53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(15,15,15));
        palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
        palette.setColor(QPalette::ToolTipBase, Qt::white);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(53,53,53));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::BrightText, Qt::red);

        palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
        palette.setColor(QPalette::HighlightedText, Qt::black);
        palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);

        m_themes.append(new RolisteamTheme(palette,tr("darkfusion"),"",QStyleFactory::create("fusion"),":/resources/icons/workspacebackground.jpg",0,QColor(GRAY_SCALE,GRAY_SCALE,GRAY_SCALE),false));
    }
    ui->m_themeComboBox->clear();

    foreach (RolisteamTheme* theme, m_themes)
    {
        ui->m_themeComboBox->addItem(theme->getName());
        if(!theme->isRemovable())
        {
            ui->m_themeComboBox->setItemIcon(ui->m_themeComboBox->count()-1,QIcon(":/resources/icons/lock.png"));
        }

    }
    ui->m_styleCombo->clear();
    ui->m_styleCombo->addItems(QStyleFactory::keys());

    QString theme = m_preferences->value("currentTheme","Default").toString();
    ui->m_themeComboBox->setCurrentText(theme);

    updateTheme();

    connect(ui->m_themeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTheme()));
    connect(ui->m_styleCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(setStyle()));

    //DiceSystem
    size = m_preferences->value("DiceAliasNumber",0).toInt();
    for(int i = 0; i < size ; ++i)
    {
        QString cmd = m_preferences->value(QString("DiceAlias_%1_command").arg(i),"").toString();
        QString value = m_preferences->value(QString("DiceAlias_%1_value").arg(i),"").toString();
        bool replace = m_preferences->value(QString("DiceAlias_%1_type").arg(i),true).toBool();
        bool enable = m_preferences->value(QString("DiceAlias_%1_enable").arg(i),true).toBool();

        DiceAlias* tmpAlias = new DiceAlias(cmd,value,replace,enable);
        m_aliasModel->addAlias(tmpAlias);
    }
    if(size==0)
    {//default alias
        m_aliasModel->addAlias(new DiceAlias("l5r","D10k"));
        m_aliasModel->addAlias(new DiceAlias("l5R","D10K"));
        m_aliasModel->addAlias(new DiceAlias("DF","D[-1-1]"));
        m_aliasModel->addAlias(new DiceAlias("nwod","D10e10c[>7]"));
        m_aliasModel->addAlias(new DiceAlias("(.*)wod(.*)","\\1d10e[=10]c[>=\\2]-@c[=1]",false));
    }


    //DiceSystem
    size = m_preferences->value("CharacterStateNumber",0).toInt();
    for(int i = 0; i < size ; ++i)
    {
        QString label = m_preferences->value(QString("CharacterState_%1_label").arg(i),"").toString();
        QColor color = m_preferences->value(QString("CharacterState_%1_color").arg(i),"").value<QColor>();
        QPixmap pixmap = m_preferences->value(QString("CharacterState_%1_pixmap").arg(i),true).value<QPixmap>();


        CharacterState* tmpState = new CharacterState();
        tmpState->setLabel(label);
        tmpState->setColor(color);
        tmpState->setImage(pixmap);
        m_stateModel->addState(tmpState);
    }



    //State - healthy , lightly Wounded , Seriously injured , dead, Sleeping, bewitched
    if(0==size)
    {
        //healthy
        CharacterState* state = new CharacterState();
        state->setColor(Qt::black);
        state->setLabel(tr("Healthy"));
        m_stateModel->addState(state);

        state = new CharacterState();
        state->setColor(QColor(255, 100, 100));
        state->setLabel(tr("Lightly Wounded"));
        m_stateModel->addState(state);

        state = new CharacterState();
        state->setColor(QColor(255, 0, 0));
        state->setLabel(tr("Seriously injured"));
        m_stateModel->addState(state);

        state = new CharacterState();
        state->setColor(Qt::gray);
        state->setLabel(tr("Dead"));
        m_stateModel->addState(state);

        state = new CharacterState();
        state->setColor(QColor(80, 80, 255));
        state->setLabel(tr("Sleeping"));
        m_stateModel->addState(state);


        state = new CharacterState();
        state->setColor(QColor(0, 200, 0));
        state->setLabel(tr("Bewitched"));
        m_stateModel->addState(state);
    }

}

void PreferencesDialog::updateTheme()
{
    int i = ui->m_themeComboBox->currentIndex();
    if((i>=0)&&(i<m_themes.size()))
    {
        RolisteamTheme* theme = m_themes.at(i);
        m_paletteModel->setPalette(theme->getPalette());
        ui->m_themeNameLineEdit->setText(theme->getName());
        ui->m_backgroundImage->setPath(theme->getBackgroundImage());

        ui->m_bgColorPush->blockSignals(true);
        ui->m_bgColorPush->setColor(theme->getBackgroundColor());
        ui->m_bgColorPush->blockSignals(false);

        ui->m_positioningComboBox->blockSignals(true);
        ui->m_positioningComboBox->setCurrentIndex(theme->getBackgroundPosition());
        ui->m_positioningComboBox->blockSignals(false);

        QString defaultStyle = theme->getStyleName();

        ui->m_styleCombo->blockSignals(true);
        ui->m_styleCombo->setCurrentIndex(ui->m_styleCombo->findText(defaultStyle.toUpper(), Qt::MatchContains));
        ui->m_styleCombo->blockSignals(false);


        qApp->setStyleSheet(theme->getCss());

        qApp->setStyle(theme->getStyle());
        qApp->setPalette(theme->getPalette());
        applyBackground();
    }

}
void PreferencesDialog::setStyle()
{
    int i = ui->m_themeComboBox->currentIndex();
    if((i>=0)&&(i<m_themes.size()))
    {
        RolisteamTheme* theme = m_themes.at(i);
        theme->setStyle(QStyleFactory::create(ui->m_styleCombo->currentText()));
        qApp->setStyle(theme->getStyle());
    }
}
void PreferencesDialog::editCss()
{
    RolisteamTheme* theme =getCurrentRemovableTheme();

    if(NULL==theme)
    {
        return;
    }
    bool ok=false;
    QString text = QInputDialog::getMultiLineText(this, tr("Css Editor"),tr("Css"), theme->getCss(),&ok);
    if((ok)&&(theme->isRemovable()))
    {
        theme->setCss(text);
        qApp->setStyleSheet(theme->getCss());
    }


}

void PreferencesDialog::dupplicateTheme(bool selectNew)
{
    int i = ui->m_themeComboBox->currentIndex();
    if((i>=0)&&(i<m_themes.size()))
    {
        RolisteamTheme* theme = m_themes.at(i);
        QString str = theme->getName();
        str.append(tr(" (copy)"));
        RolisteamTheme* newTheme = new RolisteamTheme(theme->getPalette(),str,theme->getCss(),theme->getStyle(),theme->getBackgroundImage(),theme->getBackgroundPosition(),theme->getBackgroundColor(),true);

        m_themes.append(newTheme);
        if(selectNew)
        {
            ui->m_themeComboBox->addItem(str);
            ui->m_themeComboBox->setCurrentIndex(m_themes.size()-1);
        }
    }

}
void PreferencesDialog::setTitleAtCurrentTheme()
{
    RolisteamTheme* theme =getCurrentRemovableTheme();

    if(NULL==theme)
    {
        return;
    }
    if(!theme->isRemovable())
    {
        dupplicateTheme();
    }
    if(theme->isRemovable())
    {
        theme->setName(ui->m_themeNameLineEdit->text());
        int i = ui->m_themeComboBox->currentIndex();
        ui->m_themeComboBox->setItemText(i,theme->getName());
    }
}
void PreferencesDialog::backgroundChanged()
{
    RolisteamTheme* theme =getCurrentRemovableTheme(false);
    if(NULL==theme)
    {
        return;
    }
    if(theme->isRemovable())
    {
        theme->setBackgroundImage(ui->m_backgroundImage->path());
        //ui->m_backgroundImage->setPath(path);
        theme->setBackgroundColor(ui->m_bgColorPush->color());
        theme->setBackgroundPosition(ui->m_positioningComboBox->currentIndex());

        /*int i = ui->m_themeComboBox->currentIndex();*/
        ui->m_themeComboBox->insertItem(m_themes.indexOf(theme),theme->getName());
        ui->m_themeComboBox->setCurrentIndex(m_themes.indexOf(theme));
        //applyBackground();
    }
}

void PreferencesDialog::performDiag()
{
    ui->m_diagDisplay->clear();
    
    QList<QByteArray> format(QImageReader::supportedImageFormats());

    QString htmlResult;
    QString linePattern=tr("<li>Image Format : %1 value %2</li>");
    htmlResult = tr("<h2>Supported Image Formats:</h2><ul>");
    QString result="";
    for(int i=0 ; i < format.size() ; ++i)
    {
        result+=linePattern.arg(i).arg(QString(format.at(i)));
    }
    htmlResult +=result;
    htmlResult+= tr("</ul>End of Image Format");
    result="";
    QFontDatabase database;
    htmlResult+= tr("<h2>Font families:</h2><ul>");
    linePattern = "<li>%1</li>";
    foreach (const QString &family, database.families())
    {
        result+= linePattern.arg(family);
    }
    htmlResult +=result;
    htmlResult+= tr("</ul>End of Font families");
#ifdef HAVE_SOUND
    result="";
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
                        << "audio/flac";//
    foreach (const QString &type, commonAudioMimeType)
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
    }
    htmlResult +=result;
    htmlResult+= tr("</ul>End of Supported Audio file formats");

#endif
    ui->m_diagDisplay->setHtml(htmlResult);
}

void PreferencesDialog::manageAliasAction()
{
    QToolButton* act = qobject_cast<QToolButton*>(sender());

    if(act == ui->m_addDiceAliasAct)
    {
        m_aliasModel->appendAlias();
    }
    else
    {
        QModelIndex index = ui->m_tableViewAlias->currentIndex();
        if(act == ui->m_delDiceAliasAct)
        {
            m_aliasModel->deleteAlias(index);
        }
        else if(act == ui->m_upDiceAliasAct)
        {
            m_aliasModel->upAlias(index);
        }
        else if(act == ui->m_downDiceAliasAct)
        {
            m_aliasModel->downAlias(index);
        }
        else if(act == ui->m_topDiceAliasAct)
        {
            m_aliasModel->topAlias(index);
        }
        else if(act == ui->m_bottomDiceAliasAct)
        {
            m_aliasModel->bottomAlias(index);
        }
    }
}
void  PreferencesDialog::manageStateAction()
{
    QToolButton* act = qobject_cast<QToolButton*>(sender());

    if(act == ui->m_addCharacterStateAct)
    {
        m_stateModel->appendState();
    }
    else
    {
        QModelIndex index = ui->m_stateView->currentIndex();
        if(act == ui->m_delCharceterStateAct)
        {
            m_stateModel->deleteState(index);
        }
        else if(act == ui->m_upCharacterStateAct)
        {
            m_stateModel->upState(index);
        }
        else if(act == ui->m_downCharacterStateAct)
        {
            m_stateModel->downState(index);
        }
        else if(act == ui->m_topCharacterStateAct)
        {
            m_stateModel->topState(index);
        }
        else if(act == ui->m_bottomCharacterStateAct)
        {
            m_stateModel->bottomState(index);
        }
    }
}

void PreferencesDialog::testAliasCommand()
{
    QString result = m_diceParser->convertAlias(ui->m_cmdDiceEdit->text());
    ui->m_convertedCmdEdit->setText(result);
}
void PreferencesDialog::applyBackground()
{
    int i = ui->m_themeComboBox->currentIndex();
    if((i>=0)&&(i<m_themes.size()))
    {
        RolisteamTheme* theme = m_themes.at(i);
        if(NULL!=theme)
        {
            m_currentThemeIsEditable = theme->isRemovable();
            theme->setBackgroundColor(ui->m_bgColorPush->color());
            theme->setBackgroundPosition(ui->m_positioningComboBox->currentIndex());
            theme->setBackgroundImage(ui->m_backgroundImage->path());


            m_preferences->registerValue("PathOfBackgroundImage",ui->m_backgroundImage->path());
            m_preferences->registerValue("BackGroundColor",ui->m_bgColorPush->color());
            m_preferences->registerValue("BackGroundPositioning",ui->m_positioningComboBox->currentIndex());
        }
    }
}
void PreferencesDialog::sendOffAllDiceAlias(NetworkLink* link)
{
    if(NULL!=m_aliasModel)
    {
        m_aliasModel->sendOffAllDiceAlias(link);
    }
}
void PreferencesDialog::sendOffAllState(NetworkLink* link)
{
    if(NULL!=m_aliasModel)
    {
        m_stateModel->sendOffAllCharacterState(link);
    }
}
void PreferencesDialog::exportTheme()
{
    int i = ui->m_themeComboBox->currentIndex();
    if((i>=0)&&(i<m_themes.size()))
    {
        RolisteamTheme* theme = m_themes.at(i);
        QString pathExport = QFileDialog::getSaveFileName(this,tr("Export Rolisteam Theme"),
                                                          m_preferences->value("ThemeDirectory",QDir::homePath()).toString().append("/%1.rskin").arg(theme->getName()),
                                                          tr("Rolisteam Theme: %1").arg("*.rskin"));
        if(!pathExport.isEmpty())
        {
            QFile exportFile(pathExport);
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

    }
}

bool PreferencesDialog::importTheme()
{
    QString pathImport = QFileDialog::getOpenFileName(this,tr("Import Rolisteam Theme"),m_preferences->value("ThemeDirectory",QDir::homePath()).toString(),tr("Rolisteam Theme: %1").arg("*.rskin"));
    if(!pathImport.isEmpty())
    {
        QFile importFile(pathImport);
        if(!importFile.open(QIODevice::ReadOnly))
        {
            return false;
        }
        QByteArray saveData = importFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        RolisteamTheme* theme = new RolisteamTheme(QPalette(),"","",QStyleFactory::create("fusion"),"",0,QColor(),false);
        theme->readFrom(loadDoc.object());
        m_paletteModel->readFrom(loadDoc.object());
        theme->setPalette(m_paletteModel->getPalette());


        ui->m_themeComboBox->addItem(theme->getName());
        m_themes.append(theme);
        ui->m_themeComboBox->setCurrentIndex(m_themes.size()-1);
        updateTheme();

        return true;
    }
}

RolisteamTheme* PreferencesDialog::getCurrentRemovableTheme(bool selectNew)
{
    int i = ui->m_themeComboBox->currentIndex();
    if((i>=0)&&(i<m_themes.size()))
    {
        RolisteamTheme* theme = m_themes.at(i);
        if(!theme->isRemovable())
        {
            dupplicateTheme(selectNew);
        }
        if(!selectNew)
        {
            i = ui->m_themeComboBox->currentIndex();
        }
        else
        {
            i = m_themes.size()-1;
        }
        if(i>=0)
        {
            theme = m_themes.at(i);
        }
        return theme;
    }
    else
        return NULL;
}

CharacterStateModel *PreferencesDialog::getStateModel() const
{
    return m_stateModel;
}

void PreferencesDialog::setStateModel(CharacterStateModel *stateModel)
{
    m_stateModel = stateModel;
}

void PreferencesDialog::deleteTheme()
{
    int i = ui->m_themeComboBox->currentIndex();
    if((i>=0)&&(i<m_themes.size()))
    {
        ui->m_themeComboBox->removeItem(i);
        m_themes.removeAt(i);
    }
}
