#ifndef SHEETPROPERTIES_H
#define SHEETPROPERTIES_H

#include <QDialog>
#include <QStringListModel>
namespace Ui {
class SheetProperties;
}

class SheetProperties : public QDialog
{
    Q_OBJECT

public:
    explicit SheetProperties(QWidget *parent = nullptr);
    ~SheetProperties();

    bool isNoAdaptation() const;
    void setNoAdaptation(bool noAdaptation);

    QString getAdditionalHeadCode() const;
    void setAdditionalHeadCode(const QString &additionalCode);

    QString getAdditionalBottomCode() const;
    void setAdditionalBottomCode(const QString &additionalBottomCode);

    QString getAdditionalImport() const;
    void setAdditionalImport(const QString &additionalImport);

    qreal getFixedScale() const;
    void setFixedScale(const qreal &fixedScale);


    void reset();

    QStringList getFontUri() const;
    void setFontUri(const QStringList &fontUri);

private:
    Ui::SheetProperties *ui;
    QStringList m_fontUri;
    QStringListModel m_model;
};

#endif // SHEETPROPERTIES_H
