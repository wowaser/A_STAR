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

private slots:
    void on_start_button_clicked();
    void on_clear_button_clicked();

    void on_clear_path_button_clicked();

    void no_animation_path();

    //void on_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui_;

    Scene* scene_;

    const int SCENE_DIM = 450;

    Node* new_node;

    vector<vector<Node*>> grid_;

    bool path_ = false;

};

#endif // MAINWINDOW_HH
