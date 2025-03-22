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
#include "qmlgeneratorvisitor.h"

QmlGeneratorController::QmlGeneratorController(QObject* parent)
    : QObject(parent), m_model(new FieldModel), m_mockCharacter(new MockCharacter)
{
    connect(m_mockCharacter.get(), &MockCharacter::dataChanged, this,
            [this](const QString& name)
            {
                emit reportLog(tr("The character value %1 has been defined to %2")
                                   .arg(name)
                                   .arg(m_mockCharacter->property(name.toStdString().c_str()).toString()),
                               LogController::Features);
            });
    connect(m_mockCharacter.get(), &MockCharacter::log, this,
            [this](const QString& log) { emit reportLog(log, LogController::Features); });

    connect(m_model.get(), &FieldModel::modelChanged, this,
            [this]()
            {
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
    // qWarning("Floating point comparison needs context sanity check");
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
    m_fixedScale= 1.0;
    m_bottomCode= "";
    // m_flickableSheet= false;
    m_fonts.clear();

    m_model->clearModel();

    m_mockCharacter.reset(new MockCharacter);
    connect(m_mockCharacter.get(), &MockCharacter::dataChanged, this,
            [this](const QString& name)
            {
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
    QString key= ctrl->uuid();
    QStringList keyParts= key.split('_');
    auto rect= m_model->childrenRect().size();

    QSize size= ctrl->backgroundSize();
    bool hasImage= size.isValid();
    if(hasImage)
        rect= size;

    if(!keyParts.isEmpty())
    {
        key= keyParts[0];
    }
    QmlGeneratorVisitor visitor;
    visitor.setIndentation(1);
    visitor.setIsTable(false);
    auto mainItem= hasImage ? QStringLiteral("Image") : QStringLiteral("Item");
    auto source= hasImage ? QStringLiteral("source: \"image://rcs/%1_background_%2.jpg\".arg(sheetCtrl.currentPage)")
                                .arg(key, QString("%1")) :
                            QString("");
    auto baseWidth= hasImage ? QStringLiteral("main.width") : QString::number(rect.width());

    auto res
        = visitor.generateSheet(m_importCode, m_headCode, m_bottomCode, m_lastPageId, mainItem, source, rect.width(),
                                rect.height(), baseWidth, m_fillWidth, m_fixedScale, m_model->getRootSection());
    setQmlCode(res);
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

bool QmlGeneratorController::fillWidth() const
{
    return m_fillWidth;
}

void QmlGeneratorController::setFillWidth(bool newFillWidth)
{
    if(m_fillWidth == newFillWidth)
        return;
    m_fillWidth= newFillWidth;
    emit fillWidthChanged();
}
