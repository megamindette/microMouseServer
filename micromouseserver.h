#ifndef MICROMOUSESERVER_H
#define MICROMOUSESERVER_H

#include <iostream>
#include <map>
#include <stack>
#include <vector>

#include "mazeConst.h"
#include "mazeBase.h"
#include "mazegui.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QEvent>

#include <QFile>

#include <QGraphicsSceneMouseEvent>
#include <QLineF>
#include <QTimer>

using namespace std;

namespace Ui {
class microMouseServer;
}

typedef struct CellStatus_ {
  int x_, y_;
  bool deadend_;
  bool critical_;
  bool visited_;
  inline CellStatus_()
      : x_(0), y_(0), deadend_(false), critical_(false), visited_(false) {}
  friend ostream &operator<<(ostream &out, const CellStatus_ &cell_status_);

} CellStatus;

class microMouseServer : public QMainWindow {
  Q_OBJECT

 public:
  explicit microMouseServer(QWidget *parent = 0);  // constructor
  ~microMouseServer();  // destructor

 private slots:
  void on_tabWidget_tabBarClicked(int index);
  void loadMaze();
  void saveMaze();
  void addLeftWall(QPoint cell);
  void addRightWall(QPoint cell);
  void addTopWall(QPoint cell);
  void addBottomWall(QPoint cell);
  void removeRightWall(QPoint cell);
  void removeLeftWall(QPoint cell);
  void removeTopWall(QPoint cell);
  void removeBottomWall(QPoint cell);
  void netComs();
  void connect2mouse();
  void startAI();
  void studentAI();

 private:
  bool isWallLeft();
  bool isWallRight();
  bool isWallForward();
  bool moveForward();
  void turnLeft();
  void turnRight();
  void foundFinish();
  void printUI(const char *mesg);

  void reverse();

  CellStatus_ path_[MAZE_WIDTH][MAZE_HEIGHT];
  vector<vector<char>> black_list_;

 public:
  typedef struct CellAttributes_ {
    bool black_list_;
    bool white_list_;
    bool visited_;
    inline CellAttributes_()
        : black_list_(false), white_list_(false), visited_(false) {}
    friend ostream &operator<<(ostream &out, const CellAttributes_ &cell);
  } CellAttributes;

 private:
  map<pair<int, int>, CellAttributes_> pathway_;
  int runs_;

  void dumpBlacklistedCells();
  void dumpCriticalPath();
  bool deadEndAhead();
  int get_wall_count();
  string translate_dir() const;
  void JocelynsAlgorithm(); // Solution algorithm

  QTimer *_comTimer;
  QTimer *_aiCallTimer;
  static const int _mDelay = 100;
  Ui::microMouseServer *ui;
  mazeGui *maze;
  std::vector<QGraphicsLineItem *> backgroundGrid;
  struct baseMapNode mazeData[MAZE_WIDTH][MAZE_HEIGHT];
  void connectSignals();
  void initMaze();
};

#endif  // MICROMOUSESERVER_H
