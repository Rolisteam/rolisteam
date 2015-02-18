#ifndef DIRCHOOSERPLUGIN_H
#define DIRCHOOSERPLUGIN_H


#include <QDesignerCustomWidgetInterface>
#include <QObject>

class DirChooserPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    DirChooserPlugin();

    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    QString domXml() const;
    QString group() const;
    QString includeFile() const;
    QString name() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget *createWidget(QWidget *parent);
    void initialize(QDesignerFormEditorInterface *core);

private:
    bool m_initialized;
};

#endif // DIRCHOOSERPLUGIN_H
