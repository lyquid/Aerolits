#pragma once

#include <utility> // std::move, std::exchange

namespace ktp {

template <typename T>
struct PoolUnit {
  bool         active_ {false};
  PoolUnit<T>* next_ {nullptr};
  T            object_ {};
};

template <typename T>
struct IndexedPoolUnit {
  bool                active_ {false};
  std::size_t         index_ {};
  IndexedPoolUnit<T>* next_ {nullptr};
  T                   object_ {};
};

template <class T>
class ObjectPool {

 public:

  ObjectPool(std::size_t capacity): capacity_(capacity) { inflatePool(); clear(); }
  ObjectPool(const ObjectPool& other) = delete;
  ObjectPool(ObjectPool&& other) { *this = std::move(other); }
  ~ObjectPool() { delete[] pool_; }

  ObjectPool& operator=(const ObjectPool& other) = delete;
  ObjectPool& operator=(ObjectPool&& other) {
    if (this != &other) {
      // move members
      active_count_ = other.active_count_;
      capacity_     = other.capacity_;
      // clean up memory
      delete[] pool_;
      // exchange pointers
      first_available_ = std::exchange(other.first_available_, nullptr);
      pool_ = std::exchange(other.pool_, nullptr);
    }
    return *this;
  }

  auto& operator[](std::size_t index) { return pool_[index].object_; }

  /**
   * @brief If there's an object available, it gets activated and returned as
   *        pointer. This doesn't actually create anything.
   * @return A pointer to the first available object in the pool or *WARNING*
   *         nullptr if there's no object available.
   */
  T* activate() {
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
   * @return The number of objects that are currently active.
   */
  auto activeCount() const { return active_count_; }

  /**
   * @brief Use this to access the requested index in the pool.
   *        This checks for bounds and returns nullptr if out of bounds.
   * @param index The desired index to be returned.
   * @return A pointer to the index requested or nullptr.
   */
  auto at(std::size_t index) { return index < capacity_ ? &pool_[index] : nullptr; }

  /**
   * @return The number of objects that can be stored in the pool.
   */
  auto capacity() const { return capacity_; }

  /**
   * @brief Sets all the objects to inactive state and reconstructs the list
   *        of pointers. It doesn't free any memory at all.
   */
  void clear() {
    first_available_ = &pool_[0];
    for (std::size_t i = 0; i < capacity_; ++i) {
      pool_[i].active_ = false;
      pool_[i].next_ = &pool_[i + 1];
    }
    pool_[capacity_ - 1].next_ = nullptr;
    active_count_ = 0;
  }

  /**
   * @brief Deactivates the requested object and sets it to be the first
   *        available. It doesn't destroy or delete anything.
   * @param index The index of the object to be deactivated.
   */
  void deactivate(std::size_t index) {
    if (index < capacity_) {
      pool_[index].active_ = false;
      pool_[index].next_ = first_available_;
      first_available_ = &pool_[index];
      --active_count_;
    }
  }

  /**
  * @brief Checks if a given poolunit is active.
  * @param The index to check.
  * @return True if the poolunit is active.
  */
  auto isActive(std::size_t index) const { return pool_[index].active_; }

 private:

  /**
   * @brief Allocates the necessary memory for the pool.
   */
  void inflatePool() { pool_ = new PoolUnit<T>[capacity_]; }

  PoolUnit<T>* first_available_ {nullptr};
  PoolUnit<T>* pool_ {nullptr};

  std::size_t active_count_ {0};
  std::size_t capacity_ {};
};

template <class T>
class IndexedObjectPool {

 public:

  IndexedObjectPool(std::size_t capacity): capacity_(capacity) { inflatePool(); clear(); }
  IndexedObjectPool(const IndexedObjectPool& other) = delete;
  IndexedObjectPool(IndexedObjectPool&& other) { *this = std::move(other); }
  ~IndexedObjectPool() { delete[] pool_; }

  IndexedObjectPool& operator=(const IndexedObjectPool& other) = delete;
  IndexedObjectPool& operator=(IndexedObjectPool&& other) {
    if (this != &other) {
      // move members
      active_count_         = other.active_count_;
      capacity_             = other.capacity_;
      highest_active_index_ = other.highest_active_index_;
      // clean up memory
      delete[] pool_;
      // exchange pointers
      first_available_ = std::exchange(other.first_available_, nullptr);
      pool_            = std::exchange(other.pool_, nullptr);
    }
    return *this;
  }

  auto& operator[](std::size_t index) { return pool_[index].object_ ; }

  /**
   * @brief If there's an object available, it gets activated and returned as
   *        pointer. This doesn't actually create anything.
   * @return A pointer to the first available object in the pool or *WARNING*
   *         nullptr if there's no object available.
   */
  T* activate() {
    if (first_available_) {
      first_available_->active_ = true;

      if (first_available_->index_ > highest_active_index_) {
        highest_active_index_ = first_available_->index_;
      }

      const auto aux {&first_available_->object_};
      first_available_ = first_available_->next_;
      ++active_count_;
      return aux;
    } else {
      return nullptr;
    }
  }

  /**
   * @return The number of objects that are currently active.
   */
  auto activeCount() const { return active_count_; }

  /**
   * @brief Use this to access the requested index in the pool.
   *        This checks for bounds and returns nullptr if out of bounds.
   * @param index The desired index to be returned.
   * @return A pointer to the index requested or nullptr.
   */
  auto at(std::size_t index) { return index < capacity_ ? &pool_[index] : nullptr; }

  /**
   * @return The number of objects that can be stored in the pool.
   */
  auto capacity() const { return capacity_; }

  /**
   * @brief Sets all the objects to inactive state and reconstructs the list
   *        of pointers. It doesn't free any memory at all.
   */
  void clear() {
    first_available_ = &pool_[0];
    for (std::size_t i = 0; i < capacity_; ++i) {
      pool_[i].active_ = false;
      pool_[i].index_  = i;
      pool_[i].next_   = &pool_[i + 1];
    }
    pool_[capacity_ - 1].next_ = nullptr;
    active_count_ = 0;
    highest_active_index_ = 0;
  }

  /**
   * @brief Deactivates the requested object and sets it to be the first
   *        available. It doesn't destroy or delete anything.
   * @param index The index of the object to be deactivated.
   */
  void deactivate(std::size_t index) {
    if (index < capacity_) {
      pool_[index].active_ = false;
      pool_[index].next_ = first_available_;
      first_available_ = &pool_[index];
      --active_count_;

      if (highest_active_index_ == pool_[index].index_) {
        for (std::size_t i = highest_active_index_ - 1; i >= 0; --i) {
          if (pool_[i].active_) {
            highest_active_index_ = i;
            return;
          }
        }
      }

    }
  }

  /**
   * @return The highest index of the active elements in the pool.
   */
  auto highestActiveIndex() { return highest_active_index_; }

 /**
  * @brief Checks if a given poolunit is active.
  * @param The index to check.
  * @return True if the poolunit is active.
  */
  auto isActive(std::size_t index) const { return pool_[index].active_; }

 private:

  /**
   * @brief Allocates the necessary memory for the pool.
   */
  void inflatePool() { pool_ = new IndexedPoolUnit<T>[capacity_]; }

  IndexedPoolUnit<T>* first_available_ {nullptr};
  IndexedPoolUnit<T>* pool_ {nullptr};

  std::size_t active_count_ {0};
  std::size_t capacity_;
  std::size_t highest_active_index_ {0};
};

} // namespace ktp
