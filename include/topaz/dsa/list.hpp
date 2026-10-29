#pragma once

#include "../assert.hpp"

#include <cstddef>
#include <cassert>
#include <iterator>
#include <memory>
#include <utility>

namespace tpz::dsa {

/// @brief Internal node of List. Owns the next node; points back to the previous one.
/// @tparam T Element type.
template<class T>
struct Node {
public:
  T val_;
  Node<T>* prev_;
  std::unique_ptr<Node<T>> next_;

  Node(T val, Node<T>* prev = nullptr, std::unique_ptr<Node<T>> next = nullptr) 
    : val_(val), prev_(prev), next_(std::move(next)) {}
};

/// @brief Doubly linked, non-intrusive list.
///
/// Each element lives in its own heap-allocated node, and the list owns a copy of every element.
/// List can be moved but not copied. A moved-from list is valid, empty, and can be used again.
/// Destroying the list destroys every element exactly once. Destruction is iterative, so a very
/// long list does not overflow the stack.
///
/// @tparam T Element type. Must be copy-constructible. Does not need a default constructor.
template<class T>
class List {
private:
  std::unique_ptr<Node<T>> head_;
  Node<T>* tail_;
  std::size_t size_;

public:
  /// @brief Creates an empty list. Does not allocate.
  List() noexcept : head_(nullptr), tail_(nullptr), size_(0) {}

  List(const List& other) = delete;

  List(List&& other) noexcept {
    head_ = std::move(other.head_);
    tail_ = other.tail_;
    size_ = other.size_;
  }

  List& operator=(const List& other) = delete;

  List& operator=(List&& other) noexcept {
    head_ = std::move(other.head_);
    tail_ = other.tail_;
    size_ = other.size_;
  }

  /// @brief Bidirectional iterator over the list's elements.
  ///
  /// Dereferencing yields `T&`, so the list works with range-for and `<algorithm>`.
  /// An iterator stays valid until the element it refers to is removed. Adding or removing other
  /// elements does not invalidate it.
  struct Iterator {
    // mandatory C++20/23 public type aliases
    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_concept  = std::bidirectional_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

    Node<T>* node_;
    pointer ptr_;

    reference operator*() const { return *ptr_; }
    pointer operator->() { return ptr_; }

    // prefix increment (++it)
    Iterator& operator++() {
      node_ = node_->next_.get();
      ptr_ = !node_ ? nullptr : &node_->val_;
      return *this;
    }

    // postfix increment (it++)
    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    // prefix decrement (--it)
    Iterator& operator--() {
      node_ = node_->prev_;
      ptr_ = !node_ ? nullptr : &node_->val_;
      return *this;
    }

    // postfix decrement (it--)
    Iterator operator--(int) {
      Iterator tmp = *this;
      --(*this);
      return tmp;
    }

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) { 
      return lhs.ptr_ == rhs.ptr_ && lhs.node_ == rhs.node_;
    }
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) { 
      return lhs.ptr_ != rhs.ptr_ || lhs.node_ != rhs.node_;
    }
  };

  /// @brief Returns an iterator to the first element.
  /// @return Iterator to the first element, or end() if the list is empty.
  Iterator begin() { return Iterator{ head_.get(), !head_ ? nullptr : &head_->val_ }; }

  /// @brief Returns the past-the-end iterator.
  /// @return Iterator one past the last element. It must not be dereferenced.
  Iterator end() { return Iterator{ nullptr, nullptr }; }

  /// @brief Inserts a copy of @p val at the front of the list.
  /// @param val Value to copy into the new first element.
  /// @note O(1). Allocates one node. Does not invalidate any iterators.
  void push_front(const T& val) {
    if (size_ == 0) {
      head_ = std::make_unique<Node<T>>(val);
      tail_ = head_.get();
    }
    else {
      Node<T>* old_head = head_.get();
      auto new_head = std::make_unique<Node<T>>(val, nullptr, std::move(head_));
      old_head->prev_ = new_head.get();
      head_ = std::move(new_head);
    }

    size_++;
  }

  /// @brief Inserts a copy of @p val at the back of the list.
  /// @param val Value to copy into the new last element.
  /// @note O(1). Allocates one node. Does not invalidate any iterators.
  void push_back(const T& val) {
    if (size_ == 0) {
      head_ = std::make_unique<Node<T>>(val);
      tail_ = head_.get();
    }
    else {
      Node<T>* new_tail = new Node<T>(val, tail_, nullptr);
      tail_->next_.reset(new_tail);
      tail_ = new_tail;
    }

    size_++;
  }

  /// @brief Removes and destroys the first element.
  /// @pre `!empty()`
  /// @note O(1). Invalidates only iterators to the removed element.
  void pop_front() {
    TPZ_ASSERT(head_ != nullptr);

    if (size_ == 1) {
      head_.reset(nullptr);
      tail_ = nullptr;
    }
    else {
      head_ = std::move(head_->next_);
      head_->prev_ = nullptr;
    }

    size_--;
  }
  
  /// @brief Removes and destroys the last element.
  /// @pre `!empty()`
  /// @note O(1). Invalidates only iterators to the removed element.
  void pop_back() {
    TPZ_ASSERT(head_ != nullptr);

    if (size_ == 1) {
      head_.reset(nullptr);
      tail_ = nullptr;
    }
    else {
      tail_ = tail_->prev_;
      tail_->next_.reset(nullptr);
    }

    size_--;
  }

  /// @brief Removes and destroys the element at @p it.
  /// @param it Iterator to the element to remove.
  /// @return Iterator to the element after the removed one, or end() if the removed element was
  ///         last.
  /// @pre @p it refers to an element of this list (and is not end()).
  /// @note O(1). Invalidates only @p it and copies of it.
  Iterator erase(Iterator it) {
    TPZ_ASSERT(head_ != nullptr);
    TPZ_ASSERT(it.node_ != nullptr && it.ptr_ != nullptr);

    Iterator next_it;

    if (size_ == 1) {
      head_.reset(nullptr);
      tail_ = nullptr;
      next_it = end();
    }
    else if (it == begin()) {
      pop_front();
      next_it = begin();
    }
    else if (it.node_ == tail_) {
      pop_back();
      next_it = end();
    }
    else {
      next_it = Iterator { it.node_->next_.get(), &it.node_->next_->val_ };
      it.node_->next_->prev_ = it.node_->prev_;
      it.node_->prev_->next_ = std::move(it.node_->next_);
    }

    size_--;
    return next_it;
  }

  /// @brief Returns the first element.
  /// @return Reference to the first element.
  /// @pre `!empty()`
  const T& front() const {
    assert(!empty());
    return head_->val_;
  }

  /// @brief Returns the last element.
  /// @return Reference to the last element.
  /// @pre `!empty()`
  const T& back() const {
    assert(!empty());
    return tail_->val_;
  }

  /// @brief Returns the number of elements. O(1).
  std::size_t size() const noexcept { return size_; }

  /// @brief Returns whether the list has no elements. O(1).
  bool empty() const noexcept { return size_ == 0; }

  ~List() noexcept {}
};

} // namespace tpz::dsa
