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

#include "variablesGlobales.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QImageReader>
#include <QFontDatabase>

#ifdef HAVE_SOUND
    #include <QMediaPlayer>
#endif

#include "ui_preferencesdialogbox.h"

#include "map/newemptymapdialog.h"
#include "diceparser/diceparser.h"



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
 * PreferencesDialog *
 *********************/

PreferencesDialog::PreferencesDialog(QWidget * parent, Qt::WindowFlags f)
    : QDialog(parent, f), ui(new Ui::PreferencesDialogBox())
{
    ui->setupUi(this);

    ui->m_defaultMapModeCombo->addItems(NewEmptyMapDialog::getPermissionData());


    m_preferences = PreferencesManager::getInstance();

    m_aliasModel = new DiceAliasModel();
    ui->m_tableViewAlias->setModel(m_aliasModel);
    m_diceParser = new DiceParser();
    m_aliasModel->setAliases(m_diceParser->getAliases());

    QHeaderView* horizontalHeader = ui->m_tableViewAlias->horizontalHeader();
    horizontalHeader->setSectionResizeMode(DiceAliasModel::PATTERN,QHeaderView::ResizeToContents);
    horizontalHeader->setSectionResizeMode(DiceAliasModel::VALUE,QHeaderView::Stretch);
    horizontalHeader->setSectionResizeMode(DiceAliasModel::METHOD,QHeaderView::ResizeToContents);
    ui->m_tableViewAlias->setItemDelegateForColumn(DiceAliasModel::METHOD,new CheckBoxDelegate());

    connect(this, SIGNAL(accepted()), this, SLOT(save()));
    connect(ui->m_positioningComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(applyBackground()));
    connect(ui->m_startDiag,SIGNAL(clicked()),this,SLOT(performDiag()));
    //ui->m_fogColor->setTransparency(true);

	//aliases
    connect(ui->m_addDiceAliasAct,SIGNAL(clicked()),this,SLOT(managedAction()));
    connect(ui->m_delDiceAliasAct,SIGNAL(clicked()),this,SLOT(managedAction()));
    connect(ui->m_upDiceAliasAct,SIGNAL(clicked()),this,SLOT(managedAction()));
    connect(ui->m_downDiceAliasAct,SIGNAL(clicked()),this,SLOT(managedAction()));
    connect(ui->m_topDiceAliasAct,SIGNAL(clicked()),this,SLOT(managedAction()));
    connect(ui->m_bottomDiceAliasAct,SIGNAL(clicked()),this,SLOT(managedAction()));
    connect(ui->m_testPushButton,SIGNAL(clicked()),this,SLOT(testAliasCommand()));

    // Misc
    setSizeGripEnabled(true);
    setWindowTitle(QString("%1 - %2").arg(m_preferences->value("Application_Name","rolisteam").toString(),tr("Preferences")));
    setWindowModality(Qt::ApplicationModal);


    m_preferences->registerListener("isPlayer",m_aliasModel);
    m_aliasModel->setGM(!m_preferences->value("isPlayer",false).toBool());
}

PreferencesDialog::~PreferencesDialog()
{
    // QObject should do it right for us already.
}

void PreferencesDialog::show()
{
    load();
    QDialog::show();
}

void PreferencesDialog::load()
{
    static bool firstLoad = true;
    //Direcotry PATH
    ui->m_musicDirGM->setPath(m_preferences->value("MusicDirectoryGM",QDir::homePath()).toString());
    ui->m_musicDirPlayer->setPath(m_preferences->value("MusicDirectoryPlayer",QDir::homePath()).toString());
    ui->m_pictureDir->setPath(m_preferences->value("ImageDirectory",QDir::homePath()).toString());
    ui->m_mapDir->setPath(m_preferences->value("MapDirectory",QDir::homePath()).toString());
    ui->m_scenarioDir->setPath(m_preferences->value("SessionDirectory",QDir::homePath()).toString());
    ui->m_minuteDir->setPath(m_preferences->value("MinutesDirectory",QDir::homePath()).toString());
    ui->m_chatDir->setPath(m_preferences->value("ChatDirectory",QDir::homePath()).toString());
    ui->m_checkUpdate->setChecked(m_preferences->value("MainWindow_MustBeChecked",true).toBool());

    ////////////////////////
    //MAP
    ///////////////////////
    //Fog of WAR
    QColor fog=m_preferences->value("Fog_color",QColor(Qt::black)).value<QColor>();
    ui->m_opacitySlider->setValue(m_preferences->value("Fog_opacity",fog.red()).toInt());
    ui->m_opacitySpin->setValue(m_preferences->value("Fog_opacity",fog.red()).toInt());
    ui->m_fogColor->setColor(m_preferences->value("Mask_color",QColor(Qt::darkMagenta)).value<QColor>());

    //Default Permission
    ui->m_defaultMapModeCombo->setCurrentIndex(m_preferences->value("defaultPermissionMap",0).toInt());
    ui->m_pictureAdjust->setChecked(m_preferences->value("PictureAdjust",true).toBool());


    //Background
    ui->m_bgColorPush->setColor(m_preferences->value("BackGroundColor",QColor(191,191,191)).value<QColor>());
    ui->m_backgroundImage->setPath(m_preferences->value("PathOfBackgroundImage",":/resources/icons/workspacebackground.bmp").toString());
    ui->m_backgroundImage->setMode(false);
    ui->m_backgroundImage->setFilter(tr("Images (*.png *.xpm *.jpg *.gif *.bmp)"));
    //Positioning Bg
    ui->m_positioningComboBox->setCurrentIndex(m_preferences->value("BackGroundPositioning",0).value<int>());

    //DiceSystem
    if(firstLoad)
    {
        int size = m_preferences->value("DiceAliasNumber",0).toInt();
        for(int i = 0; i < size ; ++i)
        {
            QString cmd = m_preferences->value(QString("DiceAlias_%1_command").arg(i),"").toString();
            QString value = m_preferences->value(QString("DiceAlias_%1_value").arg(i),"").toString();
            bool replace = m_preferences->value(QString("DiceAlias_%1_type").arg(i),true).toBool();

            DiceAlias* tmpAlias = new DiceAlias(cmd,value,replace);
            m_aliasModel->addAlias(tmpAlias);
        }
    }
    firstLoad=false;

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
    m_preferences->registerValue("MainWindow_MustBeChecked",ui->m_checkUpdate->isChecked());
    m_preferences->registerValue("defaultPermissionMap",ui->m_defaultMapModeCombo->currentIndex());

    QColor color;
    int opacity=ui->m_opacitySlider->value();
    color.setRgb(opacity,opacity,opacity);
    m_preferences->registerValue("Fog_color", color);

    m_preferences->registerValue("Mask_color", ui->m_fogColor->color());
    m_preferences->registerValue("PictureAdjust",ui->m_pictureAdjust->isChecked());

    //Background
    m_preferences->registerValue("PathOfBackgroundImage",ui->m_backgroundImage->path());
    m_preferences->registerValue("BackGroundColor",ui->m_bgColorPush->color());
    m_preferences->registerValue("BackGroundPositioning",ui->m_positioningComboBox->currentIndex());

    //DiceSystem
    QList<DiceAlias*>* aliasList = m_aliasModel->getAliases();
    m_preferences->registerValue("DiceAliasNumber",aliasList->size());
    for(int i = 0; i < aliasList->size() ; ++i)
    {
        DiceAlias* tmpAlias = aliasList->at(i);
        m_preferences->registerValue(QString("DiceAlias_%1_command").arg(i),tmpAlias->getCommand());
        m_preferences->registerValue(QString("DiceAlias_%1_value").arg(i),tmpAlias->getValue());
        m_preferences->registerValue(QString("DiceAlias_%1_type").arg(i),tmpAlias->isReplace());
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
                        << "audio/flac";
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

void PreferencesDialog::managedAction()
{
    QPushButton* act = qobject_cast<QPushButton*>(sender());

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
void PreferencesDialog::testAliasCommand()
{
    QString result = m_diceParser->convertAlias(ui->m_cmdDiceEdit->text());
    ui->m_convertedCmdEdit->setText(result);
}
void PreferencesDialog::applyBackground()
{
    m_preferences->registerValue("PathOfBackgroundImage",ui->m_backgroundImage->path());
    m_preferences->registerValue("BackGroundColor",ui->m_bgColorPush->color());
    m_preferences->registerValue("BackGroundPositioning",ui->m_positioningComboBox->currentIndex());
}
void PreferencesDialog::sendOffAllDiceAlias(NetworkLink* link)
{
    if(NULL!=m_aliasModel)
    {
        m_aliasModel->sendOffAllDiceAlias(link);
    }
}

