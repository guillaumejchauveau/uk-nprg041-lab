#include <deque>
#include <iostream>
#include <memory>
#include <set>
#include <vector>

#define BLANK_CELL 16
#define WIDTH 4
#define HEIGHT 4

class State {
private:
  std::vector<int> cells;
  std::string path;

public:
  State(const std::vector<int> &cells, const std::string &path) {
    this->cells = cells;
    this->path = path;
  }
  const std::string &getPath() const { return this->path; }
  bool isValid() const {
    int i = 1;
    for (int cell : this->cells) {
      if (cell != i) {
        return false;
      }
      i++;
    }
    return true;
  }
  bool operator<(const State &other) const { return this->cells < other.cells; }
  bool operator==(const State &other) const {
    auto this_it = this->cells.cbegin();
    auto other_it = other.cells.cbegin();
    while (this_it != this->cells.cend()) {
      if (*this_it != *other_it) {
        return false;
      }
      this_it++;
      other_it++;
    }
    return true;
  }
  bool operator!=(const State &other) const {
    auto this_it = this->cells.cbegin();
    auto other_it = other.cells.cbegin();
    while (this_it != this->cells.cend()) {
      if (*this_it != *other_it) {
        return true;
      }
      this_it++;
      other_it++;
    }
    return false;
  }
  std::vector<State> createNextStates() const {
    int blankCellIndex = 0;
    for (int cell : this->cells) {
      if (cell == BLANK_CELL) {
        break;
      }
      blankCellIndex++;
    }
    int blankCellPosX = blankCellIndex % WIDTH;
    int blankCellPosY = blankCellIndex / WIDTH;
    auto nextStates = std::vector<State>();

    if (blankCellPosX > 0) {
      auto newCells = std::vector<int>(this->cells);
      newCells[blankCellIndex] = this->cells[blankCellIndex - 1];
      newCells[blankCellIndex - 1] = BLANK_CELL;
      auto newPath = std::string(this->path);
      newPath.append("R");
      auto newState = State(newCells, newPath);
      nextStates.push_back(newState);
    }

    if (blankCellPosX < WIDTH - 1) {
      auto newCells = std::vector<int>(this->cells);
      newCells[blankCellIndex] = this->cells[blankCellIndex + 1];
      newCells[blankCellIndex + 1] = BLANK_CELL;
      auto newPath = std::string(this->path);
      newPath.append("L");
      auto newState = State(newCells, newPath);
      nextStates.push_back(newState);
    }

    if (blankCellPosY > 0) {
      auto newCells = std::vector<int>(this->cells);
      newCells[blankCellIndex] = this->cells[blankCellIndex - WIDTH];
      newCells[blankCellIndex - WIDTH] = BLANK_CELL;
      auto newPath = std::string(this->path);
      newPath.append("D");
      auto newState = State(newCells, newPath);
      nextStates.push_back(newState);
    }

    if (blankCellPosY < HEIGHT - 1) {
      auto newCells = std::vector<int>(this->cells);
      newCells[blankCellIndex] = this->cells[blankCellIndex + WIDTH];
      newCells[blankCellIndex + WIDTH] = BLANK_CELL;
      auto newPath = std::string(this->path);
      newPath.append("U");
      auto newState = State(newCells, newPath);
      nextStates.push_back(newState);
    }

    return nextStates;
  }
};

class Resolver {
private:
  std::deque<State> pendingStates;
  std::set<State> visitedStates;

public:
  explicit Resolver(const State &state) {
    if (state.isValid()) {
      throw std::invalid_argument("State is already valid");
    }
    this->pendingStates.push_back(state);
  }

  std::string resolve() {
    while (true) {
      auto state = std::move(this->pendingStates.front());
      this->pendingStates.pop_front();
      for (const auto &nextState : state.createNextStates()) {
        if (nextState.isValid()) {
          return nextState.getPath();
        }

        if (!this->visitedStates.count(nextState)) {
          this->pendingStates.push_back(nextState);
        }
      }
      this->visitedStates.insert(std::move(state));
      if (this->pendingStates.empty()) {
        throw std::exception();
      }
    }
  }
};

int main() {
  auto cells = std::vector<int>();
  /*cells.push_back(1);
  cells.push_back(2);
  cells.push_back(3);
  cells.push_back(4);
  cells.push_back(5);
  cells.push_back(6);
  cells.push_back(7);
  cells.push_back(8);
  cells.push_back(9);
  cells.push_back(10);
  cells.push_back(11);
  cells.push_back(12);
  cells.push_back(13);
  cells.push_back(14);
  cells.push_back(BLANK_CELL);
  cells.push_back(15);*/

  cells.push_back(10);
  cells.push_back(5);
  cells.push_back(6);
  cells.push_back(14);
  cells.push_back(4);
  cells.push_back(12);
  cells.push_back(2);
  cells.push_back(1);
  cells.push_back(13);
  cells.push_back(8);
  cells.push_back(11);
  cells.push_back(15);
  cells.push_back(3);
  cells.push_back(BLANK_CELL);
  cells.push_back(9);
  cells.push_back(7);
  auto state = State(cells, "");
  auto l = state.createNextStates();
  auto resolver = Resolver(state);
  std::cout << resolver.resolve() << std::endl;

  return 0;
}
