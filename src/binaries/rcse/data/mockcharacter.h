#ifndef MOCK_CHARACTER_H
#define MOCK_CHARACTER_H

#include <QObject>

class MockCharacter : public QObject
{
    Q_OBJECT
public:
    explicit MockCharacter(QObject* parent= nullptr);

protected:
    bool event(QEvent* event) override;
signals:
    void dataChanged(QString property);
    void log(QString log);

public slots:
    void setCurrentShape(int index);
    void addShape(const QString& name, const QString& path);

private:
    std::vector<std::pair<QString, QString>> m_properties;
};

#endif // MOCK_CHARACTER_H
