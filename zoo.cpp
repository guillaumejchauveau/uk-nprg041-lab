#include <iostream>
#include <list>
#include <memory>

class Animal {
 public:
  virtual ~Animal() {};
  virtual void sound() = 0;
};

class Wolf : public Animal {
  int size;
 public:
  Wolf(int size) : size{size} {}
  ~Wolf() override {
    std::cout << "lol" << std::endl;
  }
  void sound() override {
    std::cout << "Howl!" << this->size << std::endl;
  }
};

class Chicken : public Animal {
  int tastiness;
 public:
  Chicken(int tastiness) : tastiness{tastiness} {}
  void sound() {
    std::cout << "kdjsqiodjqs" << this->tastiness << std::endl;
  }
};

class Kangaroo : public Animal {
  std::unique_ptr<Animal> passenger;
 public:
  Kangaroo(std::unique_ptr<Animal> passenger) : passenger{move(passenger)} {}
  void sound() {
    std::cout << "Boing ";
    passenger->sound();
  }
};

class Shark : public Animal {
  std::list<std::unique_ptr<Animal>> eaten;
};
