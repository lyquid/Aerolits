#pragma once

#include <utility> // std::move, std::exchange

namespace ktp{

template<class T>
class ObjectPool {

 public:

  ObjectPool(std::size_t size) noexcept: size_(size) { inflatePool(); }
  ObjectPool(const ObjectPool& other) noexcept { *this = other; }
  ObjectPool(ObjectPool&& other) noexcept { *this = std::move(other); }
  ~ObjectPool() { delete[] pool_; }

  // WARNING! this operators haven't been tested yet.
  ObjectPool& operator=(const ObjectPool& other) {
    if (this != &other) {
      active_count_ = other.active_count_;
      first_available_ = nullptr;
      size_ = other.size_;
      delete[] pool_;
      pool_ = new PoolUnit[size_];
      for (std::size_t i = 0; i < size_; ++i) {
        pool_[i].active_ = other.pool_[i].active_;
        pool_[i].object_ = other.pool_[i].object_;
        if (&other.pool_[i] == other.first_available_) {
          first_available_ = &pool_[i];
        }
        if (!other.pool_[i].next_) {
          pool_[i].next_ = nullptr;
        } else {
          for (std::size_t j = 0; j < size_; ++j) {
            if (other.pool_[i].next_ == &other.pool_[j]) {
              pool_[i].next_ = &pool_[j];
              break;
            }
          }
        }
      }
    }
    return *this;
  }
  ObjectPool& operator=(ObjectPool&& other) noexcept {
    if (this != &other) {
      // copy/move members
      active_count_ = other.active_count_;
      size_ = other.size_;
      // clean up memory
      delete[] pool_;
      // tricky things
      first_available_ = std::exchange(other.first_available_, nullptr);
      pool_ = std::exchange(other.pool_, nullptr);
    }
    return *this;
  }

  auto& operator[](std::size_t index) { return pool_[index]; }
  auto operator[](std::size_t index) const { return pool_[index]; }

  /**
   * @return The number of objects that are currently active.
   */
  inline auto activeCount() const { return active_count_; }

  /**
   * @brief Use this to access the requested index in the pool.
   *        This checks for bounds and returns nullptr if out of bounds.
   * @param index The desired index to be returned.
   * @return A pointer to the index requested or nullptr.
   */
  inline auto at(std::size_t index) const {
    return index < size_ ? &pool_[index] : nullptr;
  }

  /**
   * @return The number of objects that can be stored in the pool.
   */
  inline auto capacity() const { return size_; }

  /**
   * @brief Sets all the objects to inactive state and reconstruct the list
   *        of pointers. It doesn't free any memory at all.
   */
  void clear() {
    first_available_ = &pool_[0];
    for (std::size_t i = 0; i < size_; ++i) {
      pool_[i].active_ = false;
      pool_[i].next_ = &pool_[i + 1];
    }
    pool_[size_ - 1].next_ = nullptr;
    active_count_ = 0;
  }

  /**
   * @brief If there's an object available, it gets activated and returned as
   *        pointer. This doesn't actually create anything.
   * @return A pointer to the first available object in the pool or *WARNING*
   *         nullptr if there's no object available.
   */
  T* create() {
    if (first_available_) {
      first_available_->active_ = true;
      const auto aux {&first_available_->object_};
      first_available_ = first_available_->next_;
      ++active_count_;
      return aux;
    } else {
      return nullptr;
    }
  }

  /**
   * @brief Deactivates the requested object and sets it to be the first
   *        available. It doesn't destroy or delete anything.
   * @param index The index of the object to be deactivated.
   */
  void destroy(std::size_t index) {
    if (index < size_) {
      pool_[index].active_ = false;
      pool_[index].next_ = first_available_;
      first_available_ = &pool_[index];
      --active_count_;
    }
  }

 private:

  struct PoolUnit {
    bool      active_ {false};
    T         object_ {};
    PoolUnit* next_ {nullptr};
  };

  /**
   * @brief Allocates the necessary memory for the pool and
   *        adjusts the pointers to the next element.
   */
  void inflatePool() {
    delete[] pool_;
    pool_ = new PoolUnit[size_];
    clear();
  }

  PoolUnit* first_available_ {nullptr};
  PoolUnit* pool_ {nullptr};

  std::size_t active_count_ {0};
  std::size_t size_;
};

} // namespace ktp