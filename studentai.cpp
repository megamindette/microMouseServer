
#include "micromouseserver.h"
#include <assert.h>
#include <iostream>
#include <map>
#include <stack>
#include <vector>

using namespace std;

ostream &operator<<(ostream &o,
                    const microMouseServer::CellAttributes_ &cell_attributes) {
  o << "<" << cell_attributes.dead_end_;
  return o;
}

ostream &operator<<(ostream &o, const Cell_ &cell) {
  o << "<" << cell.x_ << "," << cell.y_ << ">" << cell.deadend_ << " "
    << cell.visited_;
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
          coords_cell_map_.find(pair<int, int>(i, j));
      if (coords_cell_map_.end() != iter) {
        const CellAttributes_ &status = (*iter).second;
        if (status.dead_end_) {
          deadend = true;
        }
      }
      if (deadend) {
        if (i == 19 && j == 19)
          visual_[i][j] = 'E';
        else {
          visual_[i][j] = 'D';
        }
      } else {
        if (path_[i][j].visited_) {
          if (i == 19 && j == 19) {
            visual_[i][j] = 'E';
          } else {
            visual_[i][j] = '1';
          }
        } else {
          if (i == 0 && j == 0)
            visual_[i][j] = 'B';
          else
            visual_[i][j] = '0';
        }
      }
    }
  }

  cout << "TRANSPOSED VECTOR" << endl;
  for (int col = MAZE_WIDTH - 1; col >= 0; --col) {
    for (int row = 0; row < MAZE_HEIGHT; ++row) {
      cout << visual_[row][col];
    }
    cout << endl;
  }
}

bool operator==(const Cell_ &lhs, const Cell_ &rhs) {
  return lhs.x_ == rhs.x_ && lhs.y_ == rhs.y_ && lhs.deadend_ == rhs.deadend_ &&
         lhs.visited_ == rhs.visited_;
}

// Preconditions: guarantee that  isWallForward() returns false;
bool microMouseServer::IsDeadEndAhead() {
  assert(!isWallForward());
  bool ret_value = false;
  bool result = moveForward();
  assert(result);
  map<pair<int, int>, CellAttributes_>::iterator iter = coords_cell_map_.find(
      pair<int, int>(maze->mouseX() - 1, maze->mouseY() - 1));
  if (coords_cell_map_.end() != iter && (*iter).second.dead_end_) {
    ret_value = true;
  }
  reverse();
  moveForward();
  reverse();
  return ret_value;
}

int microMouseServer::get_wall_count() {
  if (0 == maze->mouseX() - 1 && 0 == maze->mouseY() - 1) return 2;
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
  if (isWallForward()) {  // Checks back wall
    ++wall_count;
  }
  reverse();  // Turns back around to original orientation

  return wall_count;
}

// Debugging helper function
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

void microMouseServer::solve() {
  bool hug_right_wall = true;
  bool deadend = false;
  bool unsolved = true;
  while (hug_right_wall && unsolved) {
    if (maze->mouseX() == MAZE_WIDTH && maze->mouseY() == MAZE_HEIGHT) {
      unsolved = false;
      break;
    }
    if (deadend) {
      coords_cell_map_[pair<int, int>(maze->mouseX() - 1, maze->mouseY() - 1)]
          .dead_end_ = true;
      path_[maze->mouseX() - 1][maze->mouseY() - 1].visited_ = true;
      reverse();
      deadend = false;
    }

    if (!isWallRight()) {
      prev_ = Cell_(maze->mouseX() - 1, maze->mouseY() - 1, false);

      turnRight();

      bool result =
          moveForward();  // TODO: limit # of moveForward()'s to one call
      assert(result);

      map<pair<int, int>, CellAttributes_>::iterator iter_prev =
          coords_cell_map_.find(pair<int, int>(prev_.x_, prev_.y_));
      bool deadend_prev = false;
      if (coords_cell_map_.end() != iter_prev &&
          (*iter_prev).second.dead_end_) {
        deadend_prev = true;
      }

      if (deadend_prev &&
          get_wall_count() == 2) {  // Is this path leading out of a dead end?
        coords_cell_map_[pair<int, int>(maze->mouseX() - 1, maze->mouseY() - 1)]
            .dead_end_ = true;
        prev_ = Cell_(maze->mouseX() - 1, maze->mouseY() - 1, true);
      }

      if (result) {  // Were we successfully able to move forward?
        path_[maze->mouseX() - 1][maze->mouseY() - 1].visited_ = true;
      } else {
        assert(true);
      }
    } else if (!isWallForward()) {
      prev_ = Cell_(maze->mouseX() - 1, maze->mouseY() - 1, false);

      bool deadend_current = false;
      map<pair<int, int>, CellAttributes_>::iterator iter =
          coords_cell_map_.find(
              pair<int, int>(maze->mouseX() - 1, maze->mouseY() - 1));
      if (coords_cell_map_.end() != iter && (*iter).second.dead_end_) {
        deadend_current = true;
      }

      prev_ = Cell_(maze->mouseX() - 1, maze->mouseY() - 1, deadend_current);

      bool result = moveForward();  // TODO: see above
      assert(result);

      if (deadend_current && get_wall_count() == 2) {
        coords_cell_map_[pair<int, int>(maze->mouseX() - 1, maze->mouseY() - 1)]
            .dead_end_ = true;
      }

      assert(result);
      path_[maze->mouseX() - 1][maze->mouseY() - 1].visited_ = true;
    } else {
      while (isWallForward()) {
        turnRight();
        if (!isWallForward() && IsDeadEndAhead()) continue;
      }
    }
    if (get_wall_count() == 3) {
      coords_cell_map_[pair<int, int>(maze->mouseX() - 1, maze->mouseY() - 1)]
          .dead_end_ = true;
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
  solve();
  dumpBlacklistedCells();

  string message("Reached target:");
  message += prev_.x_;
  message += " ";
  message += prev_.y_;
  printUI(message.c_str());

  foundFinish();
}
