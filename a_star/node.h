#ifndef NODE_HH
#define NODE_HH
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QObject>
#include <vector>
#include <cmath>

const int NODE_SIDE = 13;
using namespace std;

class Node: public QObject, public QGraphicsRectItem

{
    Q_OBJECT

public:
    Node(std::vector<int> coordinates, Node* parent = nullptr);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    std::vector<int> coordinates_;

    Node* parent_;

    bool is_walkable;

    int row_ = coordinates_.at(0);
    int column_ = coordinates_.at(1);

    int g_ = 0;
    int h_ = 0;
    int f_ = 0;

    void update_g();

    void update_h(Node* finish);

    void update_f();

    QBrush brush_ = (Qt::white);

    void update_color(QColor color);

    void clear();

    QPointF anchor;
    void setAnchor(QPointF anc);

public slots:
    void modify_obstacle(QPointF& pos, Qt::MouseButton& btn);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void node_moved();

};

#endif // NODE_HH
