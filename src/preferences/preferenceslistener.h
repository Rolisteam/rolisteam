#ifndef PREFERENCESLISTENER_H
#define PREFERENCESLISTENER_H

#include <QString>

class PreferencesListener
{
public:
    PreferencesListener();
    virtual void preferencesHasChanged(QString) = 0;


};

#endif // PREFERENCESLISTENER_H
