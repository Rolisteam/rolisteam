#ifndef QMLGENERATORCONTROLLER_H
#define QMLGENERATORCONTROLLER_H

#include <QList>
#include <QObject>
#include <QQmlError>
#include <memory>

#include "data/mockcharacter.h"
#include "fieldmodel.h"
#include <common/logcontroller.h>

class CodeEditor;
class QTreeView;
class ImageModel;
class QQuickWidget;
class ImageController;
class CharacterController;
class QmlGeneratorController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString headCode READ headCode WRITE setHeadCode NOTIFY headCodeChanged)
    Q_PROPERTY(QString bottomCode READ bottomCode WRITE setBottomCode NOTIFY bottomCodeChanged)
    Q_PROPERTY(QString importCode READ importCode WRITE setImportCode NOTIFY importCodeChanged)
    Q_PROPERTY(qreal fixedScale READ fixedScale WRITE setFixedScale NOTIFY fixedScaleChanged)
    Q_PROPERTY(bool fillWidth READ fillWidth WRITE setFillWidth NOTIFY fillWidthChanged FINAL)
    Q_PROPERTY(QStringList fonts READ fonts WRITE setFonts NOTIFY fontsChanged)
    Q_PROPERTY(bool textEdited READ textEdited NOTIFY textEditedChanged)
    Q_PROPERTY(QString uuidCharacter READ uuidCharacter WRITE setUuidCharacter NOTIFY uuidCharacterChanged)
    Q_PROPERTY(QString qmlCode READ qmlCode WRITE setQmlCode NOTIFY qmlCodeChanged)
    Q_PROPERTY(FieldModel* fieldModel READ fieldModel CONSTANT)
public:
    explicit QmlGeneratorController(QObject* parent= nullptr);

    QString headCode() const;
    QString bottomCode() const;
    QString importCode() const;
    qreal fixedScale() const;
    bool textEdited() const;

    void clearData();

    // void load(const QJsonObject& obj, EditorController* ctrl);

    FieldModel* fieldModel() const;

    MockCharacter* mockCharacter() const;

    QStringList fonts() const;
    void generateQML(const ImageController* ctrl);

    QString uuidCharacter() const;

    const QString& qmlCode() const;
    void setQmlCode(const QString& newQmlCode);

    bool fillWidth() const;
    void setFillWidth(bool newFillWidth);

signals:
    void dataChanged();
    void headCodeChanged(QString headCode);
    void bottomCodeChanged(QString bottomCode);
    void importCodeChanged(QString importCode);
    void fixedScaleChanged(qreal fixedScale);
    void textEditedChanged();
    void fontsChanged(QStringList fonts);
    void errors(const QList<QQmlError>& warning);
    void sectionChanged(Section* section);
    void reportLog(const QString& log, LogController::LogLevel level= LogController::Features);
    void uuidCharacterChanged(QString uuidCharacter);
    void qmlCodeChanged();

    void fillWidthChanged();

public slots:
    void setHeadCode(QString headCode);
    void setBottomCode(QString bottomCode);
    void setImportCode(QString importCode);
    void setFixedScale(qreal fixedScale);
    void setLastPageId(unsigned int pageId);
    void setTextEdited(bool t);
    void setFonts(QStringList fonts);
    void setUuidCharacter(QString uuidCharacter);

private:
    std::unique_ptr<FieldModel> m_model;
    std::unique_ptr<MockCharacter> m_mockCharacter;
    QString m_headCode;
    QString m_bottomCode;
    QString m_importCode;
    qreal m_fixedScale= 1.0;
    QStringList m_fonts;
    unsigned int m_lastPageId= 0;
    mutable bool m_textEdited= false;
    QString m_uuidCharacter;
    QString m_qmlCode;
    bool m_fillWidth;
};

#endif // QMLGENERATORCONTROLLER_H
