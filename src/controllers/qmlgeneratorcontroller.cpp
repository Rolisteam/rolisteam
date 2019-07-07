#include "qmlgeneratorcontroller.h"

#include <QFontDatabase>
#include <QJsonArray>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWidget>
#include <QTemporaryFile>

#include "codeeditor.h"
#include "imagecontroller.h"
#include "rolisteamimageprovider.h"

QmlGeneratorController::QmlGeneratorController(CodeEditor* codeEditor, QTreeView* view, QObject* parent)
    : QObject(parent), m_model(new FieldModel), m_codeEdit(codeEditor), m_view(view), m_mockCharacter(new MockCharacter)
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
    connect(m_codeEdit, &CodeEditor::textChanged, this, [this]() { setTextEdited(true); });
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

bool QmlGeneratorController::flickable() const
{
    return m_flickable;
}

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

void QmlGeneratorController::setFlickable(bool flickable)
{
    if(m_flickable == flickable)
        return;

    m_flickable= flickable;
    emit flickableChanged(m_flickable);
    emit dataChanged();
}

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

void QmlGeneratorController::load(const QJsonObject& obj, EditorController* ctrl)
{
    m_headCode= obj["additionnalHeadCode"].toString("");
    if(m_headCode.isEmpty())
        m_headCode= obj["additionnalCode"].toString("");
    m_importCode= obj["additionnalImport"].toString("");
    m_fixedScaleSheet= obj["fixedScale"].toDouble(1.0);
    m_bottomCode= obj["additionnalBottomCode"].toString("");
    m_flickableSheet= obj["flickable"].toBool(false);

    const auto fonts= obj["fonts"].toArray();
    for(const auto obj : fonts)
    {
        const auto font= obj.toObject();
        const auto fontData= QByteArray::fromBase64(font["data"].toString("").toLatin1());
        QFontDatabase::addApplicationFontFromData(fontData);

        m_fonts << font["name"].toString();
    }

    m_model->load(obj, ctrl);
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

void QmlGeneratorController::save(QJsonObject& obj) const
{
    // Get datamodel
    QJsonObject data;
    m_model->save(data);
    obj["data"]= data;

    // qml file
    QString qmlFile= m_codeEdit->document()->toPlainText();
    obj["qml"]= qmlFile;
    obj["additionnalHeadCode"]= m_headCode;
    obj["additionnalImport"]= m_importCode;
    obj["fixedScale"]= m_fixedScaleSheet;
    obj["additionnalBottomCode"]= m_bottomCode;
    obj["flickable"]= m_flickableSheet;

    QJsonArray fonts;
    for(QString fontUri : m_fonts)
    {
        QFile file(fontUri);
        if(file.open(QIODevice::ReadOnly))
        {
            QJsonObject font;
            font["name"]= fontUri;
            QByteArray array= file.readAll();
            font["data"]= QString(array.toBase64());
            fonts.append(font);
        }
    }
    obj["fonts"]= fonts;
    m_textEdited= false;
}

FieldModel* QmlGeneratorController::fieldModel() const
{
    return m_model.get();
}

void QmlGeneratorController::clearData()
{
    m_headCode= "";
    m_importCode= "";
    m_fixedScaleSheet= 1.0;
    m_bottomCode= "";
    m_flickableSheet= false;
    m_fonts.clear();

    m_model->clearModel();
    m_codeEdit->clear();

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
void QmlGeneratorController::showQML(QQuickWidget* quickView, ImageController* imgCtrl)
{
    QString data;
    generateQML(imgCtrl, data);
    m_codeEdit->setPlainText(data);
    runQmlFromCode(quickView, imgCtrl);
}

void QmlGeneratorController::runQmlFromCode(QQuickWidget* quickView, ImageController* imgCtrl)
{
    QString data= m_codeEdit->toPlainText();

    setTextEdited(false);
    auto provider= imgCtrl->getNewProvider();

    QTemporaryFile file;
    if(file.open()) // QIODevice::WriteOnly
    {
        file.write(data.toUtf8());
        file.close();
    }

    quickView->engine()->clearComponentCache();
    quickView->engine()->addImageProvider(QLatin1String("rcs"), provider);
    QList<CharacterSheetItem*> list= m_model->children();
    for(CharacterSheetItem* item : list)
    {
        quickView->engine()->rootContext()->setContextProperty(item->getId(), item);
    }
    quickView->engine()->rootContext()->setContextProperty("_character", m_mockCharacter.get());

    connect(quickView->engine(), &QQmlEngine::warnings, this, &QmlGeneratorController::errors);

    connect(quickView, &QQuickWidget::statusChanged, this, [this, quickView](QQuickWidget::Status status) {
        if(status == QQuickWidget::Error)
            emit errors(quickView->errors());
    });

    quickView->setSource(QUrl::fromLocalFile(file.fileName()));
    emit errors(quickView->errors());
    quickView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    QObject* root= quickView->rootObject();
    connect(root, SIGNAL(rollDiceCmd(QString, bool)), this, SLOT(rollDice(QString, bool)));
    connect(root, SIGNAL(rollDiceCmd(QString)), this, SLOT(rollDice(QString)));
}

void QmlGeneratorController::generateQML(const ImageController* ctrl, QString& qml)
{
    QTextStream text(&qml);
    QSize size= ctrl->backgroundSize();
    qreal ratio= 1;
    qreal ratioBis= 1;
    bool hasImage= false;

    if(size.isValid())
    {
        ratio= static_cast<qreal>(size.width()) / static_cast<qreal>(size.height());
        ratioBis= static_cast<qreal>(size.height()) / static_cast<qreal>(size.width());
        hasImage= true;
    }

    QString key= ctrl->uuid();
    QStringList keyParts= key.split('_');
    if(!keyParts.isEmpty())
    {
        key= keyParts[0];
    }
    text << "import QtQuick 2.4\n";
    text << "import QtQuick.Layouts 1.3\n";
    text << "import QtQuick.Controls 2.3\n";
    text << "import Rolisteam 1.0\n";
    text << "import \"qrc:/resources/qml/\"\n";
    if(!m_importCode.isEmpty())
    {
        text << "   " << m_importCode << "\n";
    }
    text << "\n";
    if(m_flickableSheet)
    {
        text << "Flickable {\n";
        text << "    id:root\n";
        text << "    contentWidth: imagebg.width;\n   contentHeight: imagebg.height;\n";
        text << "    boundsBehavior: Flickable.StopAtBounds;\n";
    }
    else
    {
        text << "Item {\n";
        text << "    id:root\n";
    }
    if(hasImage)
    {
        text << "    property alias realscale: imagebg.realscale\n";
    }
    text << "    focus: true\n";
    text << "    property int page: 0\n";
    text << "    property int maxPage:" << m_lastPageId << "\n";
    text << "    onPageChanged: {\n";
    text << "        page=page>maxPage ? maxPage : page<0 ? 0 : page\n";
    text << "    }\n";
    if(!m_headCode.isEmpty())
    {
        text << "   " << m_headCode << "\n";
    }
    text << "    Keys.onLeftPressed: --page\n";
    text << "    Keys.onRightPressed: ++page\n";
    text << "    signal rollDiceCmd(string cmd, bool alias)\n";
    text << "    signal showText(string text)\n";
    text << "    MouseArea {\n";
    text << "         anchors.fill:parent\n";
    text << "         onClicked: root.focus = true\n";
    text << "     }\n";
    if(hasImage)
    {
        text << "    Image {\n";
        text << "        id:imagebg"
             << "\n";
        text << "        objectName:\"imagebg\""
             << "\n";
        text << "        property real iratio :" << ratio << "\n";
        text << "        property real iratiobis :" << ratioBis << "\n";
        if(m_flickableSheet)
        {
            text << "       property real realscale: " << m_fixedScaleSheet << "\n";
            text << "       width: sourceSize.width*realscale"
                 << "\n";
            text << "       height: sourceSize.height*realscale"
                 << "\n";
        }
        else
        {
            text << "       property real realscale: width/" << size.width() << "\n";
            text << "       width:(parent.width>parent.height*iratio)?iratio*parent.height:parent.width"
                 << "\n";
            text << "       height:(parent.width>parent.height*iratio)?parent.height:iratiobis*parent.width"
                 << "\n";
        }
        text << "       source: \"image://rcs/" + key + "_background_%1.jpg\".arg(root.page)"
             << "\n";
        m_model->generateQML(text, 1, false);
        text << "\n";
        text << "  }\n";
    }
    else
    {
        if(m_flickableSheet)
        {
            text << "    property real realscale: " << m_fixedScaleSheet << "\n";
        }
        else
        {
            text << "    property real realscale: 1\n";
        }
        m_model->generateQML(text, 1, false);
    }
    if(!m_bottomCode.isEmpty())
    {
        text << "   " << m_bottomCode << "\n";
    }
    text << "}\n";
    text.flush();
}
void QmlGeneratorController::rollDice(QString cmd)
{
    emit reportLog(tr("The dice command %1 has been sent.").arg(cmd), LogController::Features);
}

void QmlGeneratorController::rollDice(QString cmd, bool b)
{
    if(b)
        emit reportLog(tr("The dice command %1 has been sent with aliases").arg(cmd), LogController::Features);
    else
        emit reportLog(tr("The dice command %1 has been sent with no aliases").arg(cmd), LogController::Features);
}
