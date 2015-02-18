#ifndef RGRAPHICSVIEW_H
#define RGRAPHICSVIEW_H

#include <QGraphicsView>
#include "map.h"

class RGraphicsView : public QGraphicsView
{
public:
    RGraphicsView(Map *map);
};

#endif // RGRAPHICSVIEW_H
