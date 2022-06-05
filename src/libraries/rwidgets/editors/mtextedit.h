#ifndef _MTEXTEDIT_H_
#define _MTEXTEDIT_H_

#include "rwidgets_global.h"
#include <QImage>
#include <QMimeData>
#include <QTextEdit>
class RWIDGET_EXPORT MTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    MTextEdit(QWidget* parent= nullptr);

    void dropImage(const QImage& image, const QString& format);

protected:
    bool canInsertFromMimeData(const QMimeData* source) const;
    void insertFromMimeData(const QMimeData* source);
    QMimeData* createMimeDataFromSelection() const;
};

#endif
