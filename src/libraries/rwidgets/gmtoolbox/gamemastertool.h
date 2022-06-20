#ifndef GAMEMASTERTOOL_H
#define GAMEMASTERTOOL_H
#include "rwidgets_global.h"

class RWIDGET_EXPORT GameMasterTool
{
public:
    explicit GameMasterTool();

    virtual void readSettings();
    virtual void writeSettings();
};

#endif // GAMEMASTERTOOL_H
