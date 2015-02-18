#include "dirchooserplugin.h"
#include <QtPlugin>
#include "dirchooser.h"


DirChooserPlugin::DirChooserPlugin()
{
}
bool DirChooserPlugin::isContainer() const
{
    return false;
}
bool DirChooserPlugin::isInitialized() const
{
    return m_initialized;
}
QIcon DirChooserPlugin::icon() const
{
    return QIcon();
}
QString DirChooserPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
                " <widget class=\"DirChooser\" name=\"dirChooser\">\n"
                "  <property name=\"geometry\">\n"
                "   <rect>\n"
                "    <x>0</x>\n"
                "    <y>0</y>\n"
                "    <width>100</width>\n"
                "    <height>20</height>\n"
                "   </rect>\n"
                "  </property>\n"
                " </widget>\n"
                "</ui>";
}
QString DirChooserPlugin::group() const
{
    return "Rolisteam Widgets";
}
QString DirChooserPlugin::includeFile() const
{
    return "dirchooser.h";
}
QString DirChooserPlugin::name() const
{
return "DirChooser";
}
QString DirChooserPlugin::toolTip() const
{
return "";
}
QString DirChooserPlugin::whatsThis() const
{
return "";
}
QWidget* DirChooserPlugin::createWidget(QWidget *parent)
{
    return new DirChooser(parent);
}

void DirChooserPlugin::initialize(QDesignerFormEditorInterface *)
{
    if(m_initialized)
        return;

    m_initialized = true;
}

Q_EXPORT_PLUGIN2(dirchooserplugin, DirChooserPlugin)
