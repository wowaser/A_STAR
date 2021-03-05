#include "mainwindow.h"
#include "ui_mainwindow.h"


using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);

    scene_ = new Scene(this);
    ui_->graphicsView->setScene(scene_);

    //  Workaround of fetching view dimensions on initialization
    //  (see showEvent)
    this->show();

    //  Handling start and finish nodes separately
    vector<int> START = {5,5};
    vector<int> FINISH = {NUM_OF_COLUMNS-6, NUM_OF_ROWS-6};

    start_ = new Node(START);
    finish_ = new Node(FINISH);

    set_edge_node(START, start_, "#8015A3");
    set_edge_node(FINISH, finish_, "#E5E347");

    scene_->starting_node = start_;
    scene_->finish_node = finish_;

    //  Setting up comboBox
    ui_->comboBox->addItem("A*");
    ui_->comboBox->addItem("Dijkstra");
    ui_->comboBox->addItem("Greedy BFS");

    ui_->textBrowser->setText("A* guarantees the shortest path");
}


MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::showEvent(QShowEvent *ev){
    QMainWindow::showEvent(ev);

    SCENE_HEIGHT = ui_->graphicsView->height();
    SCENE_WIDTH = ui_->graphicsView->width();
    NUM_OF_ROWS = SCENE_HEIGHT / NODE_SIDE;
    NUM_OF_COLUMNS = SCENE_WIDTH / NODE_SIDE;

    create_grid();

}

void MainWindow::resizeEvent(QResizeEvent *e){
    QMainWindow::resizeEvent(e);
    ui_->graphicsView->fitInView(ui_->graphicsView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);

    scene_->setSceneRect(scene_->itemsBoundingRect());
}

void MainWindow::set_edge_node(vector<int> pos, Node *node, QColor clr){
    scene_->addItem(node);
    node->setRect(node->boundingRect());
    node->setPos(pos[0]*NODE_SIDE, pos[1]*NODE_SIDE);
    node->update_color(clr);
    node->setFlag(QGraphicsRectItem::ItemIsSelectable);
    node->setFlag(QGraphicsRectItem::ItemIsMovable);
    connect(node, SIGNAL(node_moved()), this, SLOT(no_animation_path()));
}


void MainWindow::create_grid(){
    // creating a 2-D vector
    for(int i = 0; i<NUM_OF_COLUMNS;++i){
        vector<Node*> single_row;
        for(int j = 0;j<NUM_OF_ROWS;++j){
            vector<int> position = {i, j};
            new_node = new Node(position);
            single_row.push_back(new_node);
            scene_->addItem(new_node);
            new_node->setRect(new_node->boundingRect());
            new_node->setPos(position[0]*NODE_SIDE, position[1]*NODE_SIDE);

            // Scene signal to continuously draw/erase obstacles with a mouse
            connect(scene_, SIGNAL(draw(QPointF&, Qt::MouseButton&)), new_node, SLOT(modify_obstacle(QPointF&, Qt::MouseButton&)));

        }
        grid_.push_back(single_row);
    }
}

Node* MainWindow::get_current_node(vector<Node *>& open_list){
    // aka get node in open list with lowest f_cost
    Node* cur_node = open_list[0];
    for(Node* node:open_list){
        if(node->f_ < cur_node->f_){
            cur_node = node;
        }
        // If two nodes have the same f cost, we choose the one closer to the goal
        else if(node->f_ == cur_node->f_){
            if(node->h_ < cur_node->h_){
                cur_node = node;
            }
        }
    }
    return cur_node;
}

void MainWindow::a_star(bool is_animated){
    on_comboBox_currentIndexChanged(ui_->comboBox->currentIndex());
    // init
    change_button_state(false);
    start_->update_h(finish_);
    start_->update_f();
    vector<Node*> open_list = {start_};
    vector<Node*> closed_list;
    int combo_value = ui_->comboBox->currentIndex();

    while(!open_list.empty()){

        // get_cur_node
        Node* cur_node = get_current_node(open_list);
        open_list.erase(remove(open_list.begin(), open_list.end(), cur_node), open_list.end());
        closed_list.push_back(cur_node);

        // Children
        for(Node* child:get_children(cur_node)){

            // if found
            if(child->coordinates_ == finish_->coordinates_){
                finish_->parent_ = cur_node;
                retrace_path(finish_, is_animated);
                change_button_state(true);
                path_ = true;
                return;
            }

            // if node is not an obstacle
            if(child->is_walkable){
                int inherited_g = cur_node->g_ + 1;

                // If in closed list AND shorter path to this node is already found:
                // DO NOTHING
                vector<Node*>::iterator closed_iter = std::find(closed_list.begin(), closed_list.end(), child);
                if(closed_iter != closed_list.end()){
                    if(child->g_ <= inherited_g){
                        continue;
                    }
                }

                // If not in open list OR current path to this node is shorter than existing one:
                // Add/update this node
                vector<Node*>::iterator open_iter = std::find(open_list.begin(), open_list.end(), child);
                if(open_iter == open_list.end() or child->g_ > inherited_g){
                    open_list.push_back(child);
                    child->parent_ = cur_node;
                    // A*
                    if(combo_value == 0){
                        child->update_g();
                        child->update_h(finish_);
                        child->update_f();
                    }
                    // Dijkstra
                    else if(combo_value == 1){
                        child->update_g();
                        child->update_f();
                    }
                    // Greedy BFS
                    else{
                        child->update_h(finish_);
                        child->update_f();
                    }

                    if(is_animated){
                        QTest::qWait(1);
                    }
                    child->update_color(Qt::cyan);
                    //open_list.push_back(child);
                }
            }
        }
    }
    change_button_state(true);
    path_ = true;
    ui_->textBrowser->setText("No path");
    return;
}


void MainWindow::retrace_path(Node* cur_node, bool is_animated){
    vector<Node*> path;
    while(cur_node != nullptr){
        path.push_back(cur_node);
        cur_node = cur_node->parent_;
    }
    path.pop_back();
    path.erase(path.begin());
    for(Node* node:path){
        if(is_animated){
            QTest::qWait(5);
        }
        node->update_color(Qt::red);
    }
}
// No diagonal movement allowed
vector<Node *> MainWindow::get_children(Node *cur_node){
    vector<Node*> children;
    int cur_row = cur_node->row_;
    int cur_column = cur_node->column_;

    // exception mechanism is used to determine whether
    // child nodes are within the grid_ bounds
    try{
        Node* node_left = grid_.at(cur_row).at(cur_column-1);
        children.push_back(node_left);
    }
    catch(exception& e){}

    try{
        Node* node_up = grid_.at(cur_row-1).at(cur_column);
        children.push_back(node_up);
    }
    catch(exception& e){}

    try{
        Node* node_right = grid_.at(cur_row).at(cur_column+1);
        children.push_back(node_right);
    }
    catch(exception& e){}

    try{
        Node* node_down = grid_.at(cur_row+1).at(cur_column);
        children.push_back(node_down);

    }
    catch(exception& e){}

    return children;
}
// Clear ether the path or the whole board, including obstacles
void MainWindow::clear_board(bool obstacles){
    for(auto row:grid_){
        for(Node* node:row){
            if(obstacles){
                node->clear();
            }
            else{
                if(node->brush_ != Qt::black){
                    node->clear();
                }
            }
        }
    }
    path_ = false;
}

void MainWindow::change_button_state(bool enabled){
    if(enabled){
        ui_->clear_button->setEnabled(true);
        ui_->start_button->setEnabled(true);
        ui_->clear_path_button->setEnabled(true);
        ui_->comboBox->setEnabled(true);
    }
    else{
        ui_->clear_button->setEnabled(false);
        ui_->start_button->setEnabled(false);
        ui_->clear_path_button->setEnabled(false);
        ui_->comboBox->setEnabled(false);
    }
}


void MainWindow::on_start_button_clicked(){
    clear_board();
    a_star(true);
}

void MainWindow::on_clear_button_clicked(){
    clear_board(true);
}

void MainWindow::on_clear_path_button_clicked(){
    clear_board();
}

void MainWindow::no_animation_path(){
    if(path_){
        clear_board();
        a_star(false);
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index){
    if(index == 0){
        ui_->textBrowser->setText("A* guarantees the shortest path");
    }
    else if(index == 1){
        ui_->textBrowser->setText("Dijkstra guarantees the shortest path");
    }
    else{
        ui_->textBrowser->setText("Greedy BFS does not guarantee the shortest path");
    }
}


