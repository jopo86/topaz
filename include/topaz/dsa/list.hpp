#pragma once

#include <cstddef>
#include <iterator>
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
      ptr_ = node_ == nullptr ? nullptr : &node_->val_;
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
      ptr_ = node_ == nullptr ? nullptr : &node_->val_;
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
      return lhs.ptr_ != rhs.ptr_ && lhs.node_ != rhs.node_;
    }
  };

  Iterator begin() { return Iterator{ head_.get(), head_ == nullptr ? nullptr : &head_->val_ }; }
  Iterator end() { return Iterator{ nullptr, nullptr }; }

  void push_front(const T& val) noexcept {
    if (size_ == 0) {
      head_ = std::make_unique<Node<T>>(val);
      tail_ = head_.get();
    }
    else {
      std::unique_ptr<Node<T>> new_head = std::make_unique<Node<T>>(val, nullptr, head_.get());
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
    if (tail_ != nullptr) tail_->next_ = nullptr;

    size_--;
  }

  Iterator erase(Iterator it) {
    assert(!empty());
    return it; // TODO
  }

  const T& front() const noexcept {
    assert(!empty());
    return head_->val_;
  }

  const T& back() const noexcept {
    assert(!empty());
    return tail_->val_;
  }

  std::size_t size() const noexcept { return size_; }

  bool empty() const noexcept { return size_ == 0; }
};

} // namespace tpz::dsa
