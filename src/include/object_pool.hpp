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

/**
 * @brief Basic pool. It never modifies the addressess of the stored contents.
 * @tparam T The type to be stored on the pool.
 */
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
      pool_            = std::exchange(other.pool_, nullptr);
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
  * @brief Checks if a given poolunit is active.
  * @param index The index to check.
  * @return True if the poolunit is active.
  */
  auto active(std::size_t index) const { return pool_[index].active_; }

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

 private:

  /**
   * @brief Allocates the necessary memory for the pool.
   */
  void inflatePool() { pool_ = new PoolUnit<T>[capacity_]; }

  PoolUnit<T>* first_available_ {nullptr};
  PoolUnit<T>* pool_ {nullptr};

  std::size_t active_count_ {0};
  std::size_t capacity_;
};

/**
 * @brief Pool that keeps track of the highest active index, which is useful to
 * avoid having to traverse all the pool. It prioritizes activating lower indices.
 * It never modifies the addressess of the stored contents.
 * @tparam T The type to be stored on the pool.
 */
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
  * @brief Checks if a given poolunit is active.
  * @param index The index to check.
  * @return True if the poolunit is active.
  */
  auto active(std::size_t index) const { return pool_[index].active_; }

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
      // we are interested in filling the lowest indices first.
      if (first_available_ < pool_[index].next_) {
        // first_available_ is lower so it should not be changed
        // we store the address of the current next
        const auto aux {first_available_->next_};
        // first_available_ now points to the newly deactivated poolunit
        first_available_->next_ = &pool_[index];
        // thread back the pointers
        pool_[index].next_ = aux;
      } else {
        // first_available_ is higher so we can change it
        pool_[index].next_ = first_available_;
        first_available_ = &pool_[index];
      }
      --active_count_;
      // if the highest_active_index_ is the one we are deactivating, we need to update it
      if (highest_active_index_ == index) {
        // if the index is already 0, return
        if (index == 0) return;
        // I'm sure there are better solutions for finding the previous highest_active_index.
        // reverse loop iterating based on overflow by:
        // https://jdhao.github.io/2017/10/07/loop-forward-backward-with-cpp-vector/
        for (std::size_t i = highest_active_index_ - 1; i != (size_t)-1; --i) {
          if (pool_[i].active_) {
            highest_active_index_ = i;
            return;
          }
        }
        // if we haven't found any other active unit it's because this one was
        // the last active unit, so we set to 0 highest_active_index_
        highest_active_index_ = 0u;
      }
    }
  }

  /**
   * @return The highest index of the active elements in the pool.
   *  ***CAUTION*** 0 can be either active or inactive >:(
   */
  auto highestActiveIndex() const { return highest_active_index_; }

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
