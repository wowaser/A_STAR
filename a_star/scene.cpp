#include "scene.h"

Scene::Scene(QWidget *parent){
    this->setParent(parent);
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event){
    event_sent_to_start = false;
    event_sent_to_finish = false;

    QPointF mouse_pos = event->scenePos();
    int mouse_row = floor(mouse_pos.x()/NODE_SIDE);
    int mouse_column = floor(mouse_pos.y()/NODE_SIDE);

    if(mouse_row == starting_node->row_ and mouse_column == starting_node->column_){
        event_sent_to_start = true;
    }

    else if(mouse_row == finish_node->row_ and mouse_column == finish_node->column_){
        event_sent_to_finish = true;
    }

    else{
        // Left mouse button to draw, right mouse button to erase
        button_ = event->button();
        emit draw(mouse_pos, button_);
    }
    QGraphicsScene::mousePressEvent(event);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if(event_sent_to_start){
        sendEvent(starting_node, event);
    }
    else if(event_sent_to_finish){
        sendEvent(finish_node, event);
    }
    else{
        QPointF mouse_pos = event->scenePos();
        // signal is sent to nodes at the mouse movement coordinates
        emit draw(mouse_pos, button_);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event_sent_to_start){
        sendEvent(starting_node, event);
    }
    else if(event_sent_to_finish){
        sendEvent(finish_node, event);
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

