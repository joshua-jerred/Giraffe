#ifndef FRAME_HPP_
#define FRAME_HPP_

#include <mutex>
#include <unordered_map>

namespace data {

/**
 * @brief A generic frame (map) for storing data.
 * @details This class is thread safe.
 *
 * @tparam ID - The identifier type.
 * @tparam DATA - The data type.
 */
template <class ID, class DATA> class Frame {
public:
  Frame() = default;
  ~Frame() = default;

  /**
   * @brief Insert or replace an item.
   */
  void insert(const ID &key, const DATA &value) {
    std::lock_guard<std::mutex> lock(frame_lock_);
    frame_.insert_or_assign(key, value);
  }

  /**
   * @brief Returns true if an item is in the frame, false if it is not.
   * @return true
   * @return false
   */
  bool in(const ID &key) const {
    std::lock_guard<std::mutex> lock(frame_lock_);
    return frame_.contains(key);
  }

  /**
   * @brief Get an item from the frame.
   *
   * @param id - The identifier of the item to get.
   * @param data - The data to be returned.
   * @return true - If the item was found.
   * @return false - If the item was not found.
   */
  bool get(const ID key, DATA &data) const {
    std::lock_guard<std::mutex> lock(frame_lock_);
    if (frame_.contains(key)) {
      data = frame_.at(key);
      return true;
    }
    return false;
  }

  /**
   * @brief Clears all contents of the frame.
   */
  void clear() {
    std::lock_guard<std::mutex> lock(frame_lock_);
    frame_.clear();
  }

  /**
   * @brief Returns the total items in the frame.
   */
  int size() const {
    std::lock_guard<std::mutex> lock(frame_lock_);
    return frame_.size();
  }

  /**
   * @brief Removes an item from the frame.
   *
   * @param id - The identifier of the item to remove.
   * @return true - If the item was removed.
   * @return false - If the item was not removed.
   */
  bool remove(const ID &key) {
    std::lock_guard<std::mutex> lock(frame_lock_);
    return frame_.erase(key);
  }

  std::unordered_map<ID, DATA> getAll() const {
    std::lock_guard<std::mutex> lock(frame_lock_);
    return frame_;
  }

protected:
  std::unordered_map<ID, DATA> frame_ = std::unordered_map<ID, DATA>();

private:
  mutable std::mutex frame_lock_ = std::mutex();
};

} // namespace data

#endif /* FRAME_HPP_ */