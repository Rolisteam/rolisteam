#include "session.h"

Session::Session()
{
}
Session::Session(const Session& m)
{

}

Session::~Session()
{

}
Chapter* Session::addChapter()
{
return NULL;
}

CleverURI* Session::addRessource(QString uri, CleverURI::ContentType type, Chapter* chapter)
{
    return new CleverURI();
}

Chapter* Session::getCurrentChapter()
{
return NULL;
}
