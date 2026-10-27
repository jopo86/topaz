#pragma once

#include <cstddef>
#include <iterator>
#include <optional>
#include <memory>
#include <utility>

namespace tpz::dsa {

template<class T>
struct Node {
public:
  T val_;
  Node<T>* prev_;
  std::unique_ptr<Node<T>> next_;

  Node(T val, Node<T>* prev = nullptr, Node<T>* next = nullptr) 
    : val_(val), prev_(prev), next_(next) {}
};

template<class T>
class List {
private:
  std::unique_ptr<Node<T>> head_;
  Node<T>* tail_;
  std::size_t size_;

public:
  List() noexcept : head_(nullptr), tail_(nullptr), size_(0) {}

  // TODO: must work for range-for and <algorithm>
  struct Iterator {
    // mandatory C++20/23 public type aliases
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = Node<T>;
    using pointer           = Node<T>*;
    using reference         = Node<T>&;

    pointer ptr_;

    reference operator*() const { return *ptr_; }
    pointer operator->() { return ptr_; }

    // prefix increment (++it)
    Iterator& operator++() {
      ptr_ = ptr_->next_.get();
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
      ptr_ = ptr_->prev_;
      return *this;
    }

    // postfix decrement (it--)
    Iterator operator--(int) {
      Iterator tmp = *this;
      --(*this);
      return tmp;
    }

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) { return lhs.ptr_ == rhs.ptr_; }
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) { return lhs.ptr_ != rhs.ptr_; }
  };

  Iterator begin() { return Iterator{head_.get()}; }
  Iterator end() { return Iterator{nullptr}; }

  void push_front(const T& val) noexcept {
    if (size_ == 0) {
      head_ = std::make_unique<Node>(val);
      tail_ = head_.get();
    }
    else {
      std::unique_ptr<Node<T>> new_head = std::make_unique<Node>(val, nullptr, head_);
      head_->prev_ = new_head.get();
      head_ = std::move(new_head);
    }

    size_++;
  }

  void push_back(const T& val) noexcept {
    if (size_ == 0) {
      tail_ = new Node(val);
      head_ = std::unique_ptr<Node<T>>(tail_);
    }
    else {
      Node<T>* new_tail = new Node<T>(val, tail_, nullptr);
      tail_->next_ = std::unique_ptr<Node<T>>(new_tail);
      tail_ = new_tail;
    }

    size_++;
  }

  void pop_front() {
    assert(!empty());
    std::unique_ptr<Node<T>> new_head = std::move(head_->next_);
    head_ = std::move(new_head);
    if (head_ != nullptr) head_->prev_ = nullptr;

    size_--;
  }
  
  void pop_back() {
    assert(!empty());
    Node<T>* new_tail = tail_->prev_;
    tail_ = new_tail;
    if (tail_ != nullptr) tail_->next = nullptr;

    size_--;
  }

  void erase(Iterator it) {
    assert(!empty());
    it;
    // TODO
  }

  std::optional<const T&> front() const noexcept {
    if (head_ != nullptr) return *head_;
    else return std::nullopt;
  }

  std::optional<const T&> back() const noexcept {
    if (tail_ != nullptr) return *tail_;
    else return std::nullopt;
  }

  std::size_t size() const noexcept { return size_; }

  bool empty() const noexcept { return size_ == 0; }
};

} // namespace tpz::dsa
