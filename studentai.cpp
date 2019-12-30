
#include "micromouseserver.h"
#include <assert.h>
#include <iostream>
#include <map>
#include <stack>
#include <vector>
#include <windows.h>

using namespace std;

ostream &operator<<(ostream &o,
                    const microMouseServer::CellAttributes_ &cell_attributes) {
  o << "<" << cell_attributes.black_list_ << " " << cell_attributes.white_list_
    << " " << cell_attributes.visited_;
  return o;
}

ostream &operator<<(ostream &o, const CellStatus_ &cell_status) {
  o << "<" << cell_status.x_ << "," << cell_status.y_ << ">"
    << cell_status.deadend_ << " " << cell_status.critical_ << " "
    << cell_status.visited_;
  return o;
}

void microMouseServer::reverse() {
  turnRight();
  turnRight();  // how to turn around
}

void microMouseServer::dumpBlacklistedCells() {
  for (int i = 0; i < MAZE_HEIGHT; ++i) {
    for (int j = MAZE_WIDTH - 1; j >= 0; --j) {
      bool deadend = false;
      map<pair<int, int>, CellAttributes_>::iterator iter =
          pathway_.find(pair<int, int>(i, j));
      if (pathway_.end() != iter) {
        const CellAttributes_ &status = (*iter).second;
        if (status.black_list_) deadend = true;
      }
      if (deadend) {
        if (i == 19 && j == 19)
          black_list_[i][j] = 'E';
        else
          black_list_[i][j] = 'X';
      } else {
        if (path_[i][j].critical_) {
          black_list_[i][j] = '1';

        } else {
          if (i == 0 && j == 0)
            black_list_[i][j] = 'B';
          else
            black_list_[i][j] = '0';
        }
      }
    }
  }

  cout << "TRANSPOSED VECTOR" << endl;
  for (int col = MAZE_WIDTH - 1; col >= 0; --col) {
    for (int row = 0; row < MAZE_HEIGHT; ++row) {
      cout << black_list_[row][col];
    }
    cout << endl;
  }
}

bool operator==(const CellStatus_ &lhs, const CellStatus_ &rhs) {
  return lhs.x_ == rhs.x_ && lhs.y_ == rhs.y_ && lhs.deadend_ == rhs.deadend_ &&
         lhs.critical_ == rhs.critical_;
}

void microMouseServer::dumpCriticalPath() {
  for (int i = MAZE_WIDTH - 1; i >= 0; --i) {
    for (int j = 0; j < MAZE_HEIGHT; ++j) {
      cout << (path_[j][i].critical_ ? '1' : '0');
      if (path_[j][i].critical_) {
        black_list_[j][i] = '2';
      }
    }
    cout << endl;
  }
}

// Preconditions: guarantee that  isWallForward() returns false;
bool microMouseServer::deadEndAhead() {
  assert(!isWallForward());
  bool ret_value = false;
  bool result = moveForward();
  assert(result);
  map<pair<int, int>, CellAttributes_>::iterator iter =
      pathway_.find(pair<int, int>(maze->mouseX() - 1, maze->mouseY() - 1));
  if (pathway_.end() != iter && (*iter).second.black_list_) {
    ret_value = true;
  }
  reverse();
  result = moveForward(); //
  assert(result);
  reverse();
  return ret_value;
}

int microMouseServer::get_wall_count() /*TODO const*/ {
  int wall_count = 0;
  if (isWallLeft()) {
    ++wall_count;
  }
  if (isWallForward()) {
    ++wall_count;
  }
  if (isWallRight()) {
    ++wall_count;
  }

  reverse();
  if (isWallForward()) {
    ++wall_count;
  }
  reverse();

  return wall_count;
}

string microMouseServer::translate_dir() const {
  string direction;
  switch (maze->mouseDir()) {
    case dUP:
      direction = "UP";
      break;
    case dDOWN:
      direction = "DOWN";
      break;
    case dLEFT:
      direction = "LEFT";
      break;
    case dRIGHT:
      direction = "RIGHT";
      break;
    default:
      assert(true);
      break;
  }
  return direction;
}

void microMouseServer::JocelynsAlgorithm() {
  bool hug_right_wall = true;
  bool deadend = false;
  bool unsolved = true;
  while (hug_right_wall && unsolved) {
    if (maze->mouseX() == MAZE_WIDTH && maze->mouseY() == MAZE_HEIGHT) {
      unsolved = false;
      break;
    }
    if (deadend) {
      pathway_[pair<int, int>(maze->mouseX() - 1, maze->mouseY() - 1)]
          .black_list_ = true;
      path_[maze->mouseX() - 1][maze->mouseY() - 1].critical_;
      pathway_[pair<int, int>(maze->mouseX() - 1, maze->mouseY() - 1)]
          .visited_ = true;
      path_[maze->mouseX() - 1][maze->mouseY() - 1].visited_ = true;
      reverse();
      deadend = false;
    }
    if (!isWallRight()) {
      turnRight();
      bool result = moveForward(); // TODO
      assert(result);
      if (result) {
        pathway_[pair<int, int>(maze->mouseX() - 1, maze->mouseY() - 1)]
            .visited_ = true;
        path_[maze->mouseX() - 1][maze->mouseY() - 1].critical_ = true;

      } else {
        cout << "right-turn failed" << endl;
      }

    } else if (!isWallForward()) {
      bool result = moveForward(); // TODO: change from 2 calls of moveForward() to 1
      assert(result);
      pathway_[pair<int, int>(maze->mouseX() - 1, maze->mouseY() - 1)]
          .visited_ = true;
      path_[maze->mouseX() - 1][maze->mouseY() - 1].critical_ = true;

    } else {
      while (isWallForward()) {
        turnRight();
        if (!isWallForward() && deadEndAhead()) continue;
      }
    }
    if (get_wall_count() == 3) {
      deadend = true;
    }
  }
}

void microMouseServer::studentAI() {
  cout << "studentAI()" << endl;


  /*
   * The following are the eight functions that you can call. Feel free to
   *create your own fuctions as well. Remember that any solution that calls
   *moveForward more than once per call of studentAI() will have points
   *deducted.
   *
   *The following functions return if there is a wall in their respective
   *directions bool isWallLeft(); bool isWallRight(); bool isWallForward();
   *
   *The following functions move the mouse. Move forward returns if the mouse
   *was able to move forward and can be used for error checking bool
   *moveForward(); void turnLeft(); void turnRight();
   *
   * The following functions are called when you need to output something to the
   *UI or when you have finished the maze void foundFinish(); void printUI(const
   *char *mesg);
   */
  JocelynsAlgorithm();
  dumpBlacklistedCells();
  foundFinish();

}
