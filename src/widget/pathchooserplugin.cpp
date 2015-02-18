#include "pathchooserplugin.h"
#include "pathchooser.h"
#include <QtPlugin>

PathChooserPlugin::PathChooserPlugin(QObject *parent) :
    QObject(parent)
{
    initialized = false;
}



 void PathChooserPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;

     initialized = true;
 }

 bool PathChooserPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *PathChooserPlugin::createWidget(QWidget *parent)
 {
     return new PathChooser(parent);
 }

 QString PathChooserPlugin::name() const
 {
     return "PathChooser";
 }

 QString PathChooserPlugin::group() const
 {
     return "Display Widgets [Examples]";
 }

 QIcon PathChooserPlugin::icon() const
 {
     return QIcon();
 }

 QString PathChooserPlugin::toolTip() const
 {
     return "";
 }

 QString PathChooserPlugin::whatsThis() const
 {
     return "";
 }

 bool PathChooserPlugin::isContainer() const
 {
     return false;
 }

 QString PathChooserPlugin::domXml() const
 {
     return "<ui language=\"c++\">\n"
            " <widget class=\"PathChooser\" name=\"PathChooser\">\n"
            "  <property name=\"geometry\">\n"
            "   <rect>\n"
            "    <x>0</x>\n"
            "    <y>0</y>\n"
            "    <width>100</width>\n"
            "    <height>25</height>\n"
            "   </rect>\n"
            "  </property>\n"
            "  <property name=\"toolTip\" >\n"
            "   <string>The current time</string>\n"
            "  </property>\n"
            "  <property name=\"whatsThis\" >\n"
            "   <string>The analog clock widget displays the current time.</string>\n"
            "  </property>\n"
            " </widget>\n"
            "</ui>\n";
 }

 QString PathChooserPlugin::includeFile() const
 {
     return "pathchooser.h";
 }

 Q_EXPORT_PLUGIN2(pathchooserplugin, PathChooserPlugin)
