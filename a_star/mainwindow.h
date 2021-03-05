#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>
#include <QTest>
#include <algorithm>
#include "node.h"
#include "scene.h"
#include <QDebug>
using namespace std;

class Node;
class Scene;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void create_grid();

    Node* get_current_node(vector<Node*>& open_list);

    void a_star(bool is_animated = true);

    void retrace_path(Node* cur_node, bool is_animated = true);

    Node* start_ = nullptr;
    Node* finish_ = nullptr;


    vector<Node*> get_children(Node* cur_node);

    void clear_board(bool obstacles = false);

    void change_button_state(bool enabled = true);

    void set_edge_node(vector<int> pos, Node* node, QColor clr);

private slots:
    void on_start_button_clicked();
    void on_clear_button_clicked();

    void on_clear_path_button_clicked();

    void no_animation_path();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui_;
    Scene* scene_;

    Node* new_node;
    vector<vector<Node*>> grid_;

    bool path_ = false;

    int SCENE_HEIGHT;
    int SCENE_WIDTH;
    int NUM_OF_ROWS;// = SCENE_HEIGHT / NODE_SIDE;
    int NUM_OF_COLUMNS;// = SCENE_WIDTH / NODE_SIDE;

protected:
    void showEvent(QShowEvent *ev);
    void resizeEvent(QResizeEvent *e);
};

#endif // MAINWINDOW_HH
