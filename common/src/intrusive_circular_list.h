/*
 Copyright (C) 2019 Kristian Duske

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRENCHBROOM_INTRUSIVE_CIRCULAR_LIST_H
#define TRENCHBROOM_INTRUSIVE_CIRCULAR_LIST_H

#include <cstddef>
#include <utility>

/**
 * This structure contains the link information stored in list items.
 *
 * @tparam T the type of the list items
 */
template <typename T>
class intrusive_circular_link {
    static_assert(!std::is_pointer<T>::value, "intrusive lists do not accept pointer arguments");
    template <typename, typename> friend class intrusive_circular_list;
private:
    T* m_next;
    T* m_previous;
public:
    /**
     * Creates a new link for the given list item. Sets both next and previous to the given item so that the
     * link forms a self loop.
     *
     * @param item the list item for this link
     */
    explicit intrusive_circular_link(T* item) :
    m_next(item),
    m_previous(item) {}

    /**
     * Returns this link's predecessor.
     */
    T* previous() const {
        return m_previous;
    }

    /**
     * Returns this link's successor.
     */
    T* next() const {
        return m_next;
    }
private:
    /**
     * Sets this link's predecessor.
     */
    void set_previous(T* previous) {
        m_previous = previous;
    }

    /**
     * Sets this link's successor.
     */
    void set_next(T* next) {
        m_next = next;
    }

    /**
     * Flips this link by exchanging its predecessor and its successor.
     */
    void flip() {
        using std::swap;
        swap(m_next, m_previous);
    }
};

/**
 * A circular list that stores its links inside of the list items.
 *
 * If this list is modified in a way that removes an element that is pointed to by an iterator, this iterator
 * becomes invalid. Any iterator becomes invalid if the head item of the list is removed.
 *
 * @tparam T the type of the list items
 * @tparam GetLink maps a list item to its link info structure
 */
template <typename T, typename GetLink>
class intrusive_circular_list {
    static_assert(!std::is_pointer<T>::value, "intrusive lists do not accept pointer arguments");
public:
    using item = T;
    using get_link_info = GetLink;
    using link_info = intrusive_circular_link<T>;
public: // iterators
    template <typename TT>
    class iterator_base {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = TT;
        using pointer = TT*;
        using reference = TT&;
    private:
        pointer m_first;
        pointer m_item;
    public:
        explicit iterator_base(pointer item) :
        m_first(item),
        m_item(item) {}

        iterator_base() :
        m_first(nullptr),
        m_item(nullptr) {}

        iterator_base& operator++() {
            this->increment();
            return *this;
        }

        iterator_base operator++(int) {
            auto result = iterator_base(*this);
            this->increment();
            return result;
        }

        pointer operator*() const {
            return m_item;
        }

        reference operator->() const {
            return *m_item;
        }

        bool operator==(const iterator_base& other) const {
            return m_item == other.m_item;
        }

        bool operator!=(const iterator_base& other) const {
            return m_item != other.m_item;
        }
    private:
        void increment() {
            if (m_item != nullptr) {
                const auto get_link = get_link_info();
                const auto& link = get_link(m_item);
                T* next = link.next();
                if (next == m_first) {
                    m_item = nullptr;
                } else {
                    m_item = next;
                }
            }
        }
    };

    using iterator = iterator_base<T>;
    using const_iterator = iterator_base<T const>;
private:
    T* m_head;
    std::size_t m_size;
public:
    /**
     * Creates a new empty list.
     */
    intrusive_circular_list() :
    m_head(nullptr),
    m_size(0u) {
        assert(check_invariant());
    }

    /**
     * Creates a new list containing the items in the given initializer list. Each of the given items must be a self
     * loop.
     *
     * @param items the items to insert into this list
     */
    intrusive_circular_list(std::initializer_list<T*> items) :
    m_head(nullptr),
    m_size(0u) {
        for (T* item : items) {
            push_back(item);
        }
        assert(check_invariant());
    }

    /**
     * Destroys this list and its items.
     */
    ~intrusive_circular_list() {
        clear();
    }

    // since items can belong to at most one list, copy is not allowed
    intrusive_circular_list(intrusive_circular_list&) = delete;
    intrusive_circular_list& operator=(intrusive_circular_list&) = delete;

    // move constructor
    intrusive_circular_list(intrusive_circular_list&& other) noexcept {
        m_head = other.m_head;
        m_size = other.m_size;
        other.release();
    }

    // move assignment, cannot be noexcept because we might delete our items, and delete might throw
    intrusive_circular_list& operator=(intrusive_circular_list&& other) {
        clear();
        m_head = other.m_head;
        m_size = other.m_size;
        other.release();
        return *this;
    }

    /**
     * Returns an iterator pointing to the first element of this list. If this list is empty, then the returned
     * iterator is equivalent to an end iterator.
     */
    iterator begin() {
        return iterator(m_head);
    }

    /**
     * Returns an end iterator for this list.
     */
    iterator end() {
        return iterator();
    }

    /**
     * Returns a const iterator pointing to the first element of this list. If this list is empty, then the returned
     * iterator is equivalent to an end iterator.
     */
    const_iterator begin() const {
        return cbegin();
    }

    /**
     * Returns a const end iterator for this list.
     */
    const_iterator end() const {
        return cend();
    }

    /**
     * Returns a const iterator pointing to the first element of this list. If this list is empty, then the returned
     * iterator is equivalent to an end iterator.
     */
    const_iterator cbegin() const {
        return const_iterator(m_head);
    }

    /**
     * Returns a const end iterator for this list.
     */
    const_iterator cend() const {
        return const_iterator();
    }

    /**
     * Returns true if this list is empty and false otherwise.
     */
    bool empty() const {
        return size() == 0u;
    }

    /**
     * Returns the number of items stored in this list.
     */
    std::size_t size() const {
        return m_size;
    }

    /**
     * Returns the first element in this list or null if this list is empty;
     */
    T* front() const {
        return m_head;
    }

    /**
     * Returns the last element in this list or null if this list is empty;
     */
    T* back() const {
        if (empty()) {
            return nullptr;
        } else {
            return get_previous(m_head);
        }
    }

    /**
     * Returns an iterator for the given item. The returned iterator can be used to iterate all items in the list,
     * starting at the given item.
     *
     * @param item the item for which an iterator should be returned
     * @return the iterator
     */
    static iterator iter(T* item) {
        return iterator(item);
    }

    /**
     * Returns aconst iterator for the given item. The returned iterator can be used to iterate all items in the list,
     * starting at the given item.
     *
     * @param item the item for which an iterator should be returned
     * @return the iterator
     */
    static const_iterator iter(const T* item) {
        return const_iterator(item);
    }

    /**
     * Indicates whether the given item is contained in this list.
     *
     * @param item the item to check, which must not be null
     * @return true if the given item is contained in this list and false otherwise
     */
    bool contains(const T* item) const {
        assert(item != nullptr);

        if (empty()) {
            return false;
        } else {
            auto cur_item = m_head;
            do {
                if (cur_item == item) {
                    return true;
                }
                cur_item = get_next(cur_item);
            } while (cur_item != m_head);
        }

        return false;
    }

    /**
     * Adds the given item to this list. The item's link must be a self loop.
     *
     * @param item the items to add, must not be null
     */
    void push_back(T* item) {
        assert(item != nullptr);
        assert(!contains(item));
        assert(check_invariant());

        if (empty()) {
            m_head = item;
            m_size = 1u;
        } else {
            insert_before(begin(), item, 1u);
        }

        assert(check_invariant());
    }

    /**
     * Returns the given item from this list and returns a list containing the removed item.
     *
     * @param item the item to remove, must not be null
     * @return a list containing the removed item
     */
    intrusive_circular_list remove(T* item) {
        return remove(iter(item), std::next(iter(item)), 1u);
    }

    /**
     * Removes the items in the range [first, last) from this list and returns a list containing the removed items.
     *
     * @param first, last the range of items to remove
     * @param count the number of items to remove
     * @return a list containing the removed nodes
     */
    intrusive_circular_list remove(iterator first, iterator last, const std::size_t count) {
        assert(contains(*first));
        assert(last == end() || contains(*last));
        assert(count > 0u);
        assert(count <= size());
        assert(check_invariant());

        intrusive_circular_list result;
        result.splice_back(*this, first, last, count);

        assert(check_invariant());
        return result;
    }

    /**
     * Removes the in the range [first, last) from this list without deleting them.
     *
     * @param first, last the range of items to release
     * @param count the number of items to release
     */
    void release(iterator first, iterator last, const std::size_t count) {
        assert(contains(*first));
        assert(last == end() || contains(*last));
        assert(count > 0u);
        assert(count <= size());
        assert(check_invariant());

        if (count == size()) {
            m_head = nullptr;
            m_size = 0u;
        } else {
            auto previous = get_previous(*first);
            auto next = last == end() ? front() : *last;

            connect(get_previous(next), *first);
            connect(previous, next);

            m_size -= count;
            m_head = next;
        }

        assert(check_invariant());
    }

    /**
     * Creates a new instance of U and adds it to the back of this list.
     *
     * @tparam U the actual type to instantiate, U* must be convertible to T*
     * @tparam Args the types of the arguments to forward to U's constructor
     * @param args the arguments to forward to U's constructor
     * @return a pointer to the newly created instance of U
     */
    template <typename U=T, typename... Args>
    U* emplace_back(Args&&... args) {
        U* item = new U(std::forward<Args>(args)...);
        push_back(item);
        return item;
    }

    /**
     * Reverse the order of the items in this list.
     */
    void reverse() {
        assert(check_invariant());
        if (!empty()) {
            m_head = get_previous(m_head);
            auto cur = m_head;
            do {
                auto& cur_link = get_link(cur);
                auto next = cur_link.next();
                cur_link.flip();
                cur = next;
            } while (cur != m_head);
            assert(check_invariant());
        }
    }

    /**
     * Moves all items from the given list to the end of this list. Afterwards, the given list will be empty.
     *
     * @tparam L the type of the list to append
     * @param list the list to append to this list
     */
    template <typename L>
    void append(L&& list) {
        insert(end(), std::forward<L>(list));
    }

    /**
     * Moves all items from the given list before the given item of this list. Afterwards, the given list will be
     * empty.
     *
     * @tparam L the type of the list to insert
     * @param position the item before which the items of the given list should be inserted
     * @param list the list to insert into this list
     */
    template <typename L>
    void insert(iterator position, L&& list) {
        splice(position, list, std::begin(list), std::end(list), list.size());
    }

    /**
     * Moves items from the given list into this this list before its head item. If this list is empty, the items will
     * be moved to the front of this list.
     *
     * @tparam L the type of the list to splice from
     * @param list the list which the given items should be moved from
     * @param first, last the range of items to move into this list
     * @param count the number of items to move into this list
     */
    template <typename L>
    void splice_back(L&& list, iterator first, iterator last, const std::size_t count) {
        splice(begin(), std::forward<L>(list), first, last, count);
    }

    /**
     * Moves items from the given list into this list before the given item of this list. The items will be removed
     * from the given list and inserted before the given item of this list.
     *
     * @tparam L the type of the list to splice from
     * @param position the item before which the items of the given list should be inserted, or null if this list is empty
     * @param list the list which the given items should be moved from
     * @param list, last the range of items to move into this list
     * @param count the number of items to move into this list
     */
    template <typename L>
    void splice(iterator position, L&& list, iterator first, iterator last, const std::size_t count) {
        assert(!empty() || position == end());
        assert(position == end() || contains(*position));

        list.release(first, last, count);
        if (empty()) {
            m_head = *first;
            m_size = count;
        } else {
            insert_before(position, *first, count);
        }
    }

    /**
     * Replaces the items in the range [first, last) from this list and inserts all items from the given list before
     * the given last item.
     *
     * @tparam L the type of the list to move items from
     * @param replace_first, replace_last the range of items to replace
     * @param replace_count the number of items to replace
     * @param list the list to move items from
     * @return a list containing the replaced items
     */
    template <typename L>
    intrusive_circular_list splice_replace(iterator replace_first, iterator replace_last, const std::size_t replace_count, L&& list) {
        return splice_replace(replace_first, replace_last, replace_count, list, std::begin(list), std::end(list), list.size());
    }

    /**
     * Moves items in the range [move_first, move_last) from the given list into this list, replacing the items in range
     * [replace_first, replace_last) of this list.
     *
     * @tparam L the type of the list to splice from
     * @param replace_first, replace_last the range of items to replace
     * @param replace_count the number of items of this list to replace
     * @param list the list which the given items should be moved from
     * @param move_first, move_last the range of items to move into this list
     * @param move_count the number of items to move into this list
     */
    template <typename L>
    intrusive_circular_list splice_replace(
        iterator replace_first, iterator replace_last, const std::size_t replace_count,
        L&& list, iterator move_first, iterator move_last, const std::size_t move_count) {

        assert(empty() || contains(*replace_first));
        assert(replace_last == end() || contains(*replace_last));
        assert(replace_count > 0u);
        assert(replace_count <= size());
        assert(move_count > 0u);
        assert(move_count <= list.size());

        auto insert_position = replace_last;
        auto result = remove(replace_first, replace_last, replace_count);
        if (empty()) {
            insert_position = end();
        }

        splice(insert_position, std::forward<L>(list), move_first, move_last, move_count);
        return result;
    }

    /**
     * Clears this list without deleting its items.
     */
    void release() {
        m_head = nullptr;
        m_size = 0u;
    }

    /**
     * Clears this list and deletes all items.
     */
    void clear() {
        if (!empty()) {
            const auto get_link = GetLink();
            auto cur = m_head;
            do {
                auto next = get_link(cur).next();
                delete cur;
                cur = next;
            } while (cur != m_head);
            m_size = 0u;
        }
    }
private: // helpers
    auto& get_link(T* item) const {
        assert(item != nullptr);
        return GetLink()(item);
    }

    T* get_previous(T* item) const {
        assert(item != nullptr);
        return get_link(item).previous();
    }

    T* get_next(T* item) const {
        assert(item != nullptr);
        return get_link(item).next();
    }

    void connect(T* previous, T* next) {
        auto& previous_link = get_link(previous);
        auto& next_link = get_link(next);

        previous_link.set_next(next);
        next_link.set_previous(previous);
    }

    /**
     * Inserts the given items into this list before the given position. If this list is empty, then the items are
     * inserted at the front of this list.
     *
     * @param position the position before which the items are inserted
     * @param items the items to insert
     * @param count the number of items to insert
     */
    void insert_before(iterator position, T* items, std::size_t count) {
        if (empty()) {
            m_head = items;
            m_size = count;
        } else {
            auto previous = position == end() ? back() : get_previous(*position);
            auto next = get_next(previous);

            auto first = items;
            auto last = get_previous(first);

            connect(previous, first);
            connect(last, next);

            m_size += count;
        }
    }
private: // invariants and checks
    bool check_invariant() {
        if (m_head == nullptr) {
            return m_size == 0u;
        } else {
            std::size_t count = 0u;
            auto cur = m_head;
            do {
                auto next = get_next(cur);
                if (get_previous(next) != cur) {
                    return false;
                }
                ++count;
                cur = next;
            } while (cur != m_head);
            return m_size == count;
        }
    }
};



#endif //TRENCHBROOM_INTRUSIVE_CIRCULAR_LIST_H
