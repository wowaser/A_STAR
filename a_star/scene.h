#ifndef SCENE_HH
#define SCENE_HH
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPoint>
#include <QObject>
#include <QWidget>
#include "node.h"

class Node;
class Scene: public QGraphicsScene
{
    Q_OBJECT
public:
    Scene(QWidget* parent = nullptr);

    Qt::MouseButton button_;

    Node* starting_node;
    Node* finish_node;

    bool event_sent_to_start = false;
    bool event_sent_to_finish = false;

signals:
    void draw(QPointF&, Qt::MouseButton&);

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);


};

#endif // SCENE_HH
