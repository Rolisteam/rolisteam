#include "qmlgeneratorcontroller.h"

#include <QFontDatabase>
#include <QJsonArray>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWidget>
#include <QTemporaryFile>

#include "charactercontroller.h"
#include "charactersheet/rolisteamimageprovider.h"
#include "charactersheet_formula/formulamanager.h"
#include "codeeditor.h"
#include "imagecontroller.h"

QmlGeneratorController::QmlGeneratorController(QObject* parent)
    : QObject(parent), m_model(new FieldModel), m_mockCharacter(new MockCharacter)
{
    connect(m_mockCharacter.get(), &MockCharacter::dataChanged, this, [this](const QString& name) {
        emit reportLog(tr("The character value %1 has been defined to %2")
                           .arg(name)
                           .arg(m_mockCharacter->property(name.toStdString().c_str()).toString()),
                       LogController::Features);
    });
    connect(m_mockCharacter.get(), &MockCharacter::log, this,
            [this](const QString& log) { emit reportLog(log, LogController::Features); });

    connect(m_model.get(), &FieldModel::modelChanged, this, [this]() {
        emit sectionChanged(m_model->getRootSection());
        emit dataChanged();
    });
}

QString QmlGeneratorController::headCode() const
{
    return m_headCode;
}

QString QmlGeneratorController::bottomCode() const
{
    return m_bottomCode;
}

QString QmlGeneratorController::importCode() const
{
    return m_importCode;
}

/*bool QmlGeneratorController::flickable() const
{
    return m_flickableSheet;
}*/

qreal QmlGeneratorController::fixedScale() const
{
    return m_fixedScale;
}

bool QmlGeneratorController::textEdited() const
{
    return m_textEdited;
}
void QmlGeneratorController::setHeadCode(QString headCode)
{
    if(m_headCode == headCode)
        return;

    m_headCode= headCode;
    emit headCodeChanged(m_headCode);
    emit dataChanged();
}

void QmlGeneratorController::setBottomCode(QString bottomCode)
{
    if(m_bottomCode == bottomCode)
        return;

    m_bottomCode= bottomCode;
    emit bottomCodeChanged(m_bottomCode);
    emit dataChanged();
}

void QmlGeneratorController::setImportCode(QString importCode)
{
    if(m_importCode == importCode)
        return;

    m_importCode= importCode;
    emit importCodeChanged(m_importCode);
    emit dataChanged();
}

// void QmlGeneratorController::setFlickable(bool flickable)
//{
/*   if(m_flickableSheet == flickable)
       return;

   m_flickableSheet= flickable;
   emit flickableChanged(m_flickableSheet);*/
// emit dataChanged();
//}

void QmlGeneratorController::setFixedScale(qreal fixedScale)
{
    qWarning("Floating point comparison needs context sanity check");
    if(qFuzzyCompare(m_fixedScale, fixedScale))
        return;

    m_fixedScale= fixedScale;
    emit fixedScaleChanged(m_fixedScale);
    emit dataChanged();
}

void QmlGeneratorController::setLastPageId(unsigned int pageId)
{
    m_lastPageId= pageId;
}

QStringList QmlGeneratorController::fonts() const
{
    return m_fonts;
}

void QmlGeneratorController::setTextEdited(bool t)
{
    if(t == m_textEdited)
        return;
    m_textEdited= t;
    emit textEditedChanged();
    emit dataChanged();
}
void QmlGeneratorController::setFonts(QStringList fonts)
{
    if(m_fonts == fonts)
        return;

    m_fonts= fonts;
    emit fontsChanged(m_fonts);
    emit dataChanged();
}

FieldModel* QmlGeneratorController::fieldModel() const
{
    return m_model.get();
}

MockCharacter* QmlGeneratorController::mockCharacter() const
{
    return m_mockCharacter.get();
}

void QmlGeneratorController::clearData()
{
    m_headCode= "";
    m_importCode= "";
    m_fixedScaleSheet= 1.0;
    m_bottomCode= "";
    // m_flickableSheet= false;
    m_fonts.clear();

    m_model->clearModel();

    m_mockCharacter.reset(new MockCharacter);
    connect(m_mockCharacter.get(), &MockCharacter::dataChanged, this, [this](const QString& name) {
        emit reportLog(tr("The character value %1 has been defined to %2")
                           .arg(name)
                           .arg(m_mockCharacter->property(name.toStdString().c_str()).toString()),
                       LogController::Features);
    });
    connect(m_mockCharacter.get(), &MockCharacter::log, this,
            [this](const QString& log) { emit reportLog(log, LogController::Features); });
}

QString QmlGeneratorController::uuidCharacter() const
{
    return m_uuidCharacter;
}

void QmlGeneratorController::setUuidCharacter(QString uuidCharacter)
{
    if(m_uuidCharacter == uuidCharacter)
        return;

    m_uuidCharacter= uuidCharacter;
    emit uuidCharacterChanged(m_uuidCharacter);
}

void QmlGeneratorController::generateQML(const ImageController* ctrl)
{
    QString data;
    QTextStream text(&data);
    QSize size= ctrl->backgroundSize();
    bool hasImage= size.isValid();

    auto rect= m_model->childrenRect();

    QString key= ctrl->uuid();
    QStringList keyParts= key.split('_');
    if(!keyParts.isEmpty())
    {
        key= keyParts[0];
    }
    text << "import QtQuick\n";
    text << "import QtQuick.Layouts\n";
    text << "import QtQuick.Controls\n";
    text << "import Rolisteam\n";
    text << "import CharacterSheet\n";
    text << "import Helper\n";

    if(!m_importCode.isEmpty())
    {
        text << "   " << m_importCode << "\n";
    }
    text << "\n";

    text << "Flickable {\n";
    text << "    id:root\n";
    if(hasImage)
        text << "    contentWidth: imagebg.width;\n    contentHeight: imagebg.height;\n";
    text << "    boundsBehavior: Flickable.StopAtBounds;\n";

    if(hasImage)
    {
        text << "    property alias realscale: imagebg.realscale\n";
    }
    text << "    SheetController {\n";
    text << "       id: sheetCtrl\n";
    text << "       appCtrl: AppCtrl\n";
    text << "       parentWidth: root.width\n";
    text << "       imageBgWidth: imagebg.baseWidth\n";
    text << "       pageMax: " << m_lastPageId << "\n";
    text << "    }\n";
    text << "    ContextualMenu {\n";
    text << "       id: _menu\n";
    text << "       ctrl: sheetCtrl\n";
    text << "    \n";
    text << "    }\n";

    text << "    focus: true\n";
    text << "    property int maxPage: sheetCtrl.pageMax\n";
    text << "    Keys.onLeftPressed: sheetCtrl.nextPage()\n";
    text << "    Keys.onRightPressed: sheetCtrl.previousPage()\n";
    text << "    MouseArea {\n";
    text << "         anchors.fill:parent\n";
    text << "         acceptedButtons: Qt.LeftButton | Qt.RightButton \n";
    text << "         onClicked: (mouse)=>{ \n";
    text << "         if(mouse.button & Qt.LeftButton) \n";
    text << "           root.focus = true\n";
    text << "         else if(mouse.button & Qt.RightButton)\n";
    text << "         {\n";
    text << "           _menu.x = mouse.x\n";
    text << "           _menu.y = mouse.y\n";
    text << "           _menu.open()\n";
    text << "         }\n";
    text << "       }\n";
    text << "    }\n";
    if(!m_headCode.isEmpty())
    {
        text << "   " << m_headCode << "\n";
    }
    if(hasImage)
    {
        text << "   Image {\n";
        text << "       id:imagebg\n";
        text << "       objectName:\"imagebg\"\n";
        text << "       property real realscale: sheetCtrl.zoomLevel\n";
        text << "       property real baseWidth: sourceSize.width\n";
        text << "       width: baseWidth*realscale\n";
        text << "       height: sourceSize.height*realscale\n";
        text << "       source: \"image://rcs/" + key + "_background_%1.jpg\".arg(sheetCtrl.currentPage)\n";
        m_model->generateQML(text, 1, false);
        text << "\n";
        text << "  }\n";
    }
    else
    {
        text << "   Item {\n";
        text << "       id: imagebg\n";
        text << "       property real realscale: sheetCtrl.zoomLevel\n";
        text << "       property real baseWidth: " << rect.width() << "\n";
        text << "       width: " << rect.width() << "*realscale\n";
        text << "       height: " << rect.height() << "*realscale\n";
        m_model->generateQML(text, 1, false);
        text << "   }\n";
    }
    if(!m_bottomCode.isEmpty())
    {
        text << "   " << m_bottomCode << "\n";
    }
    text << "}\n";
    text.flush();

    setQmlCode(data);
    setTextEdited(false);
}

const QString& QmlGeneratorController::qmlCode() const
{
    return m_qmlCode;
}

void QmlGeneratorController::setQmlCode(const QString& newQmlCode)
{
    if(m_qmlCode == newQmlCode)
        return;
    m_qmlCode= newQmlCode;
    emit qmlCodeChanged();
    setTextEdited(true);
}
