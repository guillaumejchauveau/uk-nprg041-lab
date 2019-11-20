#include <vector>

template<typename T>
class SVector : public std::vector<T> {
 private:
  class RefProxy {
    T *ptr;
   public:
    RefProxy(T *other = nullptr) : ptr{other} {}

    RefProxy &operator=(const T &rvalue) {
      if (ptr) {
        *ptr = rvalue;
      }
      return *this;
    }
    operator T() {
      if (ptr) {
        return *ptr;
      }
      return T();
    }
  };

  class SizeProxy {
    SVector<T> *v_p;
   public:
    SizeProxy(SVector *v_p) : v_p{v_p} {}

    SizeProxy &operator=(size_t length) {
      this->v_p->resize(length);
      return *this;
    }

    operator size_t() {
      return static_cast<std::vector<T> *> (this->v_p)->size();
    }

  };

 public:
  SizeProxy size = this;

  RefProxy operator[](size_t index) {
    if (index >= this->size) {
      return RefProxy(new T());
    }
    return RefProxy(this->data() + index);
  }
};
