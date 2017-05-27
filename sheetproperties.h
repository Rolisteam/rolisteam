#ifndef SHEETPROPERTIES_H
#define SHEETPROPERTIES_H

#include <QDialog>

namespace Ui {
class SheetProperties;
}

class SheetProperties : public QDialog
{
    Q_OBJECT

public:
    explicit SheetProperties(QWidget *parent = 0);
    ~SheetProperties();

    bool isNoAdaptation() const;
    void setNoAdaptation(bool noAdaptation);

    QString getAdditionalCode() const;
    void setAdditionalCode(const QString &additionalCode);

    qreal getFixedScale() const;
    void setFixedScale(const qreal &fixedScale);

    bool getAdditionCodeAtTheBeginning() const;
    void setAdditionCodeAtTheBeginning(bool additionCodeAtTheBeginning);

    void reset();


private:
    Ui::SheetProperties *ui;
};

#endif // SHEETPROPERTIES_H
