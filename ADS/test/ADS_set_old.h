#ifndef ADS_SET_H
#define ADS_SET_H

// ADS_set.h Double Hashing
// 
// VU Algorithmen und Datenstrukturen - SS 2018 Universitaet Wien
// Container - Projekt
// https://cewebs.cs.univie.ac.at/ADS/ss18/
//
// Beispielimplementierung mit "Double Hashing - Linear Probing" 
// Erstellt in den Lehrveranstaltungen vom 13. und 20.4.2018
//
// Nicht optimiert und eventuell fehlerhaft (Fehler bitte melden)

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

template <typename Key, size_t N = 7>
class ADS_set {
public:
  class Iterator;
  using value_type = Key;
  using key_type = Key;
  using reference = key_type&;
  using const_reference = const key_type&;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = Iterator;
  using const_iterator = Iterator;
  using key_compare = std::less<key_type>;   // B+-Tree
  using key_equal = std::equal_to<key_type>; // Hashing
  using hasher = std::hash<key_type>;        // Hashing
private:
  enum class Mode {free, used, freeagain, end};
  struct element {
    key_type key;
    Mode mode{Mode::free};
  };
  element * table{nullptr};
  size_type curr_size{0}, table_size{0};
  float max_lf{0.7};
  
  size_type hash_idx(const key_type &key) const { return hasher{}(key) % table_size; }
  size_type next_idx(const key_type &, size_type idx) const { return (idx+1) % table_size; }
  
  element *insert_unchecked(const key_type &key);
  element *find_pos(const key_type &key) const;
  void rehash(size_type n);
  void reserve(size_type n);
public:
  ADS_set() { rehash(N); }
  ADS_set(std::initializer_list<key_type> ilist): ADS_set{} { insert(ilist); }
  template<typename InputIt> ADS_set(InputIt first, InputIt last): ADS_set{} { insert(first, last); }
  ADS_set(const ADS_set &other): ADS_set{} { 
    reserve(other.curr_size);
    for (const auto &key: other) insert_unchecked(key);
  }
  ~ADS_set() { delete[] table; }

  ADS_set& operator=(const ADS_set &other) {
    if (this == &other) return *this;
    ADS_set tmp{other};
    swap(tmp);
    return *this;
  }
  ADS_set& operator=(std::initializer_list<key_type> ilist) {
    ADS_set tmp{ilist};
    swap(tmp);
    return *this;
  }

  size_type size() const { return curr_size; }
  bool empty() const { return !curr_size; }

  size_type count(const key_type& key) const;
  iterator find(const key_type &key) const {
    if (element *pos {find_pos(key)}) return iterator{pos};
    return end();
  }

  void clear() { 
    ADS_set tmp;
    swap(tmp);
  }
  void swap(ADS_set &other) {
    std::swap(table, other.table);
    std::swap(curr_size, other.curr_size);
    std::swap(table_size, other.table_size);
    std::swap(max_lf, other.max_lf);
  }

  void insert(std::initializer_list<key_type> ilist);
  std::pair<iterator,bool> insert(const key_type &key) {
    element *pos {find_pos(key)};
    if (pos) return {iterator{pos},false};
    reserve(curr_size+1);
    return {iterator{insert_unchecked(key)},true};
  }
  template<typename InputIt> void insert(InputIt first, InputIt last);

  size_type erase(const key_type &key) {
    element *pos {find_pos(key)};
    if (pos) {
      pos->mode = Mode::freeagain;
      --curr_size;
      return 1;
    }
    return 0;
  }

  const_iterator begin() const { return const_iterator{table}; }
  const_iterator end() const { return const_iterator{table+table_size}; }

  void dump(std::ostream& o = std::cerr) const;

  friend bool operator==(const ADS_set &lhs, const ADS_set &rhs) {
    if (lhs.curr_size != rhs.curr_size) return false;
    for (const auto &key: rhs) {
      if (!lhs.find_pos(key)) return false;
    }
    return true;
  }
  friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs) { return !(lhs == rhs); }
};

template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {
  element *pos;
  void skip() {
    while (pos->mode != Mode::used && pos->mode != Mode::end) ++pos;
  }
public:
  using value_type = Key;
  using difference_type = std::ptrdiff_t;
  using reference = const value_type&;
  using pointer = const value_type*;
  using iterator_category = std::forward_iterator_tag;

  explicit Iterator(element *pos = nullptr): pos{pos} { if (pos) skip(); }
  reference operator*() const { return pos->key; }
  pointer operator->() const { return &pos->key; }
  Iterator& operator++() { ++pos; skip(); return *this; }
  Iterator operator++(int) { Iterator rc {*this}; ++*this; return rc; }
  friend bool operator==(const Iterator &lhs, const Iterator &rhs) { return lhs.pos == rhs.pos; }
  friend bool operator!=(const Iterator &lhs, const Iterator &rhs) { return lhs.pos != rhs.pos; }
};

template <typename Key, size_t N> void swap(ADS_set<Key,N>& lhs, ADS_set<Key,N>& rhs) { lhs.swap(rhs); }

template <typename Key, size_t N>
typename ADS_set<Key,N>::element *ADS_set<Key,N>::insert_unchecked(const key_type &key) {
  size_type idx{hash_idx(key)}, start_idx{idx};
  while (table[idx].mode == Mode::used) {
    idx = next_idx(key, idx);
    if (idx == start_idx) throw std::runtime_error{"insert_unchecked failed"};
  }
  table[idx].key = key;
  table[idx].mode = Mode::used;
  ++curr_size;
  return table+idx;
}

template <typename Key, size_t N>
typename ADS_set<Key,N>::element *ADS_set<Key,N>::find_pos(const key_type &key) const {
  size_type idx{hash_idx(key)}, start_idx{idx};
  do {
    switch (table[idx].mode) {
      case Mode::used:
        if (key_equal{}(table[idx].key,key)) return table+idx;
        break;
      case Mode::free:
        return nullptr;
      case Mode::freeagain:
        break;
      case Mode::end:
        throw std::runtime_error{"find_pos reached end!"};
    }
  } while ((idx = next_idx(key, idx)) != start_idx);
  return nullptr;
}

template <typename Key, size_t N>
void ADS_set<Key,N>::rehash(size_type n) {
  size_type new_table_size {std::max(N, std::max(n, size_type(curr_size / max_lf)))};
  element *new_table {new element[new_table_size+1]};
  new_table[new_table_size].mode = Mode::end;
  element *old_table {table};
  size_type old_table_size {table_size};
  table = new_table;
  table_size = new_table_size;
  curr_size = 0;
  for (size_type i{0}; i < old_table_size; ++i) {
    if (old_table[i].mode == Mode::used) {
      insert_unchecked(old_table[i].key);
    }
  }
  delete[] old_table;
}

template <typename Key, size_t N>
void ADS_set<Key,N>::reserve(size_type n) {
  if (n > table_size * max_lf) {
    size_type new_table_size {table_size};
    while (n > new_table_size * max_lf) new_table_size = new_table_size * 2 + 1;
    rehash(new_table_size);
  }
}

template <typename Key, size_t N>
typename ADS_set<Key,N>::size_type ADS_set<Key,N>::count(const key_type& key) const {
  return !!find_pos(key);  
}

template <typename Key, size_t N>
void ADS_set<Key,N>::insert(std::initializer_list<key_type> ilist) {
  reserve(curr_size+ilist.size());
  for (const auto &key: ilist) {
    if (!find_pos(key)) insert_unchecked(key);
  }
}

template <typename Key, size_t N>
template <typename InputIt> void ADS_set<Key,N>::insert(InputIt first, InputIt last) {
  for (auto it = first; it != last; ++it) {
    if (!find_pos(*it)) {
      reserve(curr_size+1);
      insert_unchecked(*it);
    }
  }
}

template <typename Key, size_t N>
void ADS_set<Key,N>::dump(std::ostream& o) const {
  for (size_type i{0}; i < table_size; ++i) {
    o << i << ": ";
    switch (table[i].mode) {
      case Mode::used:
        o << table[i].key;
        break;
      case Mode::free:
        o << '-';
        break;
      case Mode::freeagain:
        o << '+';
        break;
      case Mode::end:
        o << "end";
        break;
    }
    o << '\n';
  }
}

#endif // ADS_SET_H
