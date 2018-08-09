#ifndef READERSENDER_H
#define READERSENDER_H

#include <QObject>

class ReaderSender : public QObject
{
    Q_OBJECT
public:
    explicit ReaderSender(QObject *parent = nullptr);

signals:

public slots:
};

#endif // READERSENDER_H