#include "node.h"
#include <QGraphicsItemAnimation>


Node::Node(std::vector<int> coordinates, Node *parent):
    coordinates_(coordinates), parent_(parent), is_walkable(true)
{
    this->setBrush(brush_);

}

QRectF Node::boundingRect() const{
    return QRectF(0,0,NODE_SIDE, NODE_SIDE);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

    QRectF rect = boundingRect();
    painter->fillRect(rect, brush_);
    painter->drawRect(rect);

}

void Node::update_g(){
    if(this->parent_ != nullptr){
        this->g_ = parent_->g_ + 1;
    }
    else{
        this->g_ = 1;
    }

}

void Node::update_h(Node *finish){
    this->h_ = abs(this->row_ - finish->row_)+abs(this->column_ - finish->column_);
}

void Node::update_f(){
    this->f_ = h_+ g_;
}

void Node::update_color(QColor color){
    brush_ = color;
    update();
}

void Node::clear(){
    g_ = 0;
    h_ = 0;
    f_ = 0;
    parent_ = nullptr;
    update_color(Qt::white);
    is_walkable = true;

}

void Node::setAnchor(QPointF anc){
    anchor = anc;
}

void Node::modify_obstacle(QPointF &pos, Qt::MouseButton &btn){
    int mouse_row = floor(pos.x()/NODE_SIDE);
    int mouse_column = floor(pos.y()/NODE_SIDE);
    if(mouse_row == this->row_ and mouse_column == this->column_){
        if(btn == Qt::LeftButton){
            if(brush_ != Qt::black){
                brush_ = Qt::black;
                is_walkable = false;
            }
        }
        else if(btn == Qt::RightButton){
            if(brush_ == Qt::black){
                brush_ = Qt::white;
                is_walkable = true;
            }
        }

        update();
    }
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsRectItem::mouseMoveEvent(event);

}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    QList<QGraphicsItem*> col_items = collidingItems();
    if(!col_items.empty()){
        Node* host_node = nullptr;
        // algorithm for finding the closest node
        qreal shortest_dist = 10000;
        for(auto item:col_items){
            if(dynamic_cast<Node*>(item) != nullptr){
                QLineF line(item->sceneBoundingRect().center(),
                            this->sceneBoundingRect().center());
                if(line.length() < shortest_dist){
                    shortest_dist = line.length();
                    host_node = dynamic_cast<Node*>(item);
                }
            }
        }
        host_node->is_walkable = true;
        this->setPos(host_node->pos());
        this->anchor = host_node->pos();
        this->row_ = host_node->row_;
        this->column_ = host_node->column_;
        this->coordinates_ = {row_, column_};

    }
    else{
        this->setPos(anchor);
    }
    emit node_moved();
    QGraphicsRectItem::mouseReleaseEvent(event);

}
