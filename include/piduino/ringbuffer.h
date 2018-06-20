/* Copyright © 2002-2012 Pete Goodliffe, All rights reserved.
 *
 * Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the Piduino Library.
 *
 * The Piduino Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The Piduino Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This circular buffer is based on the STL-style circular buffer by
 * Pete Goodliffe (circular.h). That code was subject of the following articles
 * - Pete Goodliffe. STL-style circular buffers by example.
 *   Overload, 10(50), August 2002.
 *   http://accu.org/index.php/journals/389
 * - Pete Goodliffe. STL-style circular buffers by example, part two.
 *   Overload, 10(51), October 2002.
 *   http://accu.org/index.php/journals/383
 * - Project page  C++ circular buffer library,
 *   http://www.goodliffe.net/circularbuffers
 *
 * In 2008, Pete wrote a series of 11 Blog articles on similar, but not
 * completely identical circular buffer code:
 * - Pete Goodliffe. An STL-like circular buffer (Part {n}),
 *   http://goodliffe.blogspot.com/2008/11/c-stl-like-circular-buffer-part-{n}.html
 *   where {n} is 1..11.
 *
 * The code in this file compiles with VC6, VC8, VC2010, GNUC 4.5.2.
 * If compiling with VC6 is not a requirement, you may also be able to use
 * boost::RingBuffer<>, see
 * http://www.boost.org/doc/libs/1_48_0/libs/RingBuffer/
 *
 * Notable changes with respect to he original implementation are:
 * - There's no policy to select between accept and reject on buffer full
 * - Added non-member begin(cb) and end(cb).
 * - The iterator class is defined inside the RingBuffer class
 *   (this may help to make the code more acceptable to VC6).
 * - The iterator class is derived from std::iterator<> to supply the
 *   iterator traits.
 * - The copy-swap idiom used for operator= uses a value parameter.
 *
 * Note 1: use template parameters names with a maximum length of two
 *   to prevent clashes with names defined via #define.
 *   Mathew Wilson. Imperfect C++ p.xxxxxx.
 *
 *  12 September 2012 - fixed computation of offset in reserve().
 *  17 December  2011 - created.
 */

#ifndef PIDUINO_RING_BUFFER_H
#define PIDUINO_RING_BUFFER_H

#include <piduino/memory.h>
#include <iterator>
#include <stdexcept>

namespace Piduino {

  /**
   * @class RingBuffer
   * @brief STL-style circular buffer.
   */
  template <typename T, typename A = std::allocator<T>>
  class RingBuffer {

    public:
      typedef RingBuffer<T, A> self_type;
      typedef A allocator_type;
      typedef typename allocator_type::value_type        value_type;
      typedef typename allocator_type::pointer           pointer;
      typedef typename allocator_type::const_pointer     const_pointer;
      typedef typename allocator_type::reference         reference;
      typedef typename allocator_type::const_reference   const_reference;
      typedef typename allocator_type::size_type         size_type;
      typedef typename allocator_type::difference_type   difference_type;

      // Lifetime
      // -----------------------------------------------------------------------

      /**
       * @brief Default constructor
       * @param capacity
       * @param allocator
       */
      explicit RingBuffer (size_type const capacity = 1,
                           allocator_type const & allocator = allocator_type())
        : m_capacity (capacity)
        , m_allocator (allocator)
        , m_array (m_allocator.allocate (capacity, (void *) 0))
        , m_head (1)
        , m_tail (0)
        , m_contents_size (0) {}

      /**
       * @brief Copy constructor
       *
       * copy-initialiser constructor must be non-explicit for operator=()
       * @param other
       * @return
       */
      RingBuffer (self_type const & other)
        : m_capacity (other.m_capacity)
        , m_allocator (other.m_allocator)
        // VC6 requires (void *) 0:
        , m_array (m_allocator.allocate (other.m_capacity, (void *) 0))
        , m_head (other.m_head)
        , m_tail (other.m_tail)
        , m_contents_size (other.m_contents_size) {

        try {
          assign_into (other.begin(), other.end());
        }
        catch (...) {
          clear();
          m_allocator.deallocate (m_array, m_capacity);
          throw;
        }
      }

      /**
       * @brief ??
       * @param from
       * @param to
       */
      template <typename II>
      RingBuffer (II const from, II const to)
        : m_capacity (0)
        , m_allocator (allocator_type())
        , m_array (0)
        , m_head (0)
        , m_tail (0)
        , m_contents_size (0) {

        RingBuffer tmp (std::distance (from, to));
        tmp.assign_into (from, to);
        swap (tmp);
      }

      /**
       * @brief Destructor
       */
      ~RingBuffer() {
        clear();
        m_allocator.deallocate (m_array, m_capacity);
      }

      /**
       * @brief copy-swap idiom using value parameter
       * @param other
       */
      RingBuffer & operator= (self_type other) {
        other.swap (*this);
        return *this;
      }

      /**
       * @brief
       * @param other
       */
      void swap (self_type & other) {
        using std::swap;

        swap (m_capacity,      other.m_capacity);

        // no need to swap the allocator object
        // as RingBuffer types are identical:
        // swap( m_allocator, other.m_allocator );

        swap (m_array,         other.m_array);
        swap (m_head,          other.m_head);
        swap (m_tail,          other.m_tail);
        swap (m_contents_size, other.m_contents_size);
      }

      /**
       * @brief
       * @return
       */
      allocator_type get_allocator() const {

        return m_allocator;
      }

      /**
       * @class iterator_
       * @brief RingBuffer iterator class template
       */
      template<typename E, typename EN>
      class iterator_ : public std::iterator<std::random_access_iterator_tag, EN> {

        public:
          typedef E elem_type;

          // non-const element type
          typedef EN elem_type_nc;

          typedef iterator_<elem_type, elem_type_nc> self_type;

          // VC6 requires <T,A>:
          typedef RingBuffer<T, A> RingBufferType;

          /*
           * iterator lifetime:
           */

          iterator_ (RingBufferType * const buf, size_type const pos)
            : m_buf (buf)
            , m_pos (pos) {
          }

          iterator_ (RingBufferType const * const buf, size_type const pos)
            : m_buf (const_cast<RingBufferType *> (buf))
            , m_pos (pos) {
          }

          // Use compiler generated copy ctor and dtor

          // convert from iterator to const_iterator:

          friend class iterator_<const elem_type, elem_type_nc> ;

          iterator_ (iterator_<elem_type_nc, elem_type_nc> const & other)
            : m_buf (other.m_buf)
            , m_pos (other.m_pos) {
          }

          self_type & operator= (self_type const & other) {
            m_buf = other.m_buf;
            m_pos = other.m_pos;
            return *this;
          }

          /*
           * iterator accessors:
           */

          elem_type & operator*() {
            return (*m_buf) [m_pos];
          }

          elem_type * operator->() {
            return & (operator*());
          }

          /*
           * iterator modifiers:
           */

          self_type & operator++() {
            m_pos += 1;
            return *this;
          }

          self_type operator++ (int) {
            self_type tmp (*this);
            ++ (*this);
            return tmp;
          }

          self_type & operator--() {
            m_pos -= 1;
            return *this;
          }

          self_type operator-- (int) {
            self_type tmp (*this);
            -- (*this);
            return tmp;
          }

          self_type operator+ (difference_type const n) const {
            self_type tmp (*this);
            tmp.m_pos += n;
            return tmp;
          }

          self_type &operator+= (difference_type const n) {
            m_pos += n;
            return *this;
          }

          self_type operator- (difference_type const n) const {
            self_type tmp (*this);
            tmp.m_pos -= n;
            return tmp;
          }

          self_type &operator-= (difference_type const n) {
            m_pos -= n;
            return *this;
          }

          difference_type operator- (self_type const & c) const {
            return m_pos - c.m_pos;
          }

          bool operator== (self_type const & other) const {
            return m_pos == other.m_pos && m_buf == other.m_buf;
          }

          bool operator!= (self_type const & other) const {
            return m_pos != other.m_pos && m_buf == other.m_buf;
          }

          bool operator> (self_type const & other) const {
            return m_pos > other.m_pos;
          }

          bool operator>= (self_type const & other) const {
            return m_pos >= other.m_pos;
          }

          bool operator< (self_type const & other) const {
            return m_pos < other.m_pos;
          }

          bool operator<= (self_type const & other) const {
            return m_pos <= other.m_pos;
          }

          /*
           * iterator free standing operator + and -, defined here as
           * friends for brevity and to prevent probable difficulties with VC6:
           */

          friend self_type operator+ (
            typename self_type::difference_type const & lhs,
            self_type                  const & rhs) {
            return self_type (lhs) + rhs;
          }

          friend self_type operator- (
            typename self_type::difference_type const & lhs,
            self_type                  const & rhs) {
            return self_type (lhs) - rhs;
          }

        private:
          RingBufferType * m_buf;
          size_type  m_pos;
      };

      // Iterators
      // -----------------------------------------------------------------------

      typedef iterator_<value_type, value_type> iterator;
      typedef iterator_<const value_type, value_type> const_iterator;

      typedef std::reverse_iterator<iterator>          reverse_iterator;
      typedef std::reverse_iterator<const_iterator>    const_reverse_iterator;

      /**
       * @brief
       * @return
       */
      iterator begin() {

        return iterator (this, 0);
      }

      /**
       * @brief
       * @return
       */
      iterator end() {

        return iterator (this, size());
      }

      /**
       * @brief
       * @return
       */
      const_iterator begin() const {

        return const_iterator (this, 0);
      }

      /**
       * @brief
       * @return
       */
      const_iterator end() const {

        return const_iterator (this, size());
      }

      /**
       * @brief
       * @return
       */
      reverse_iterator rbegin() {

        return reverse_iterator (end());
      }

      /**
       * @brief
       * @return
       */
      reverse_iterator rend() {

        return reverse_iterator (begin());
      }

      /**
       * @brief
       * @return
       */
      const_reverse_iterator rbegin() const {

        return const_reverse_iterator (end());
      }

      /**
       * @brief
       * @return
       */
      const_reverse_iterator rend() const {

        return const_reverse_iterator (begin());
      }

      // Size accessors
      // -----------------------------------------------------------------------

      /**
       * @brief
       * @return
       */
      bool empty() const {

        return 0 == m_contents_size;
      }

      /**
       * @brief
       * @return
       */
      size_type capacity() const {

        return m_capacity;
      }

      /**
       * @brief
       * @return
       */
      size_type size() const {

        return m_contents_size;
      }

      /**
       * @brief
       * @return
       */
      size_type max_size() const {

        return m_allocator.max_size();
      }

      // Size modifiers
      // -----------------------------------------------------------------------

      /**
       * @brief reserve shrinks or expands the internal buffer to the size given
       *
       * if the buffer shrinks, keep at most the last new_size items.
       * @param new_size
       */
      void reserve (size_type const new_size) {
        if (new_size != capacity()) {
          RingBuffer tmp (new_size);

          // preferred:
          // const size_type offset = std::max( 0, size() - new_size );
          // however, VC6 may have defined max() as macro.
          const size_type offset =
            new_size < size() ? size() - new_size : 0;

          tmp.assign_into (begin() + offset, end());
          swap (tmp);
        }
      }

      // Content accessors
      // -----------------------------------------------------------------------

      /**
       * @brief access the first element
       * @return a reference to the first element in the container.
       */
      reference front() {

        return m_array[m_head];
      }

      /**
       * @brief access the last element
       * @return
       */
      reference back() {

        return m_array[m_tail];
      }

      /**
       * @brief access the first element
       * @return a reference to the first element in the container.
       */
      const_reference front() const {

        return m_array[m_head];
      }

      /**
       * @brief access the last element
       * @return
       */
      const_reference back() const {

        return m_array[m_tail];
      }

      /**
       * @brief
       * @param n
       * @return
       */
      const_reference operator[] (size_type const n) const {

        return at_unchecked (n);
      }

      /**
       * @brief
       * @param n
       * @return
       */
      const_reference at (size_type const n) const {

        return at_checked (n);
      }

      // Content modifiers
      // -----------------------------------------------------------------------

      /**
       * @brief
       */
      void clear() {

        for (size_type n = 0; n < m_contents_size; ++n) {
          m_allocator.destroy (m_array + index_to_subscript (n));
        }

        m_head = 1;
        m_tail = m_contents_size = 0;
      }


      /**
       * @brief Appends the given item to the end of the container
       */
      void push_back (value_type const & item) {

        size_type next = next_tail();

        if (m_contents_size == m_capacity) {
          m_array[next] = item;
          increment_head();
        }
        else {
          m_allocator.construct (m_array + next, item);
        }
        increment_tail();
      }

      /**
       * @brief Insert a new element at the beginning of the container
       */
      void push_front (value_type const & item) {

        size_type previous = previous_head();

        if (m_contents_size == m_capacity) {
          m_array[previous] = item; // buffer full, replace last element
        }
        else {
          m_allocator.construct (m_array + previous, item);
        }
        decrement_head();
      }


      /**
       * @brief Removes the first element of the container.
       */
      void pop_front() {

        if (m_contents_size) {

          size_type destroy_pos = m_head;
          increment_head();
          m_allocator.destroy (m_array + destroy_pos);
        }
      }

      /**
       * @brief Remove the last element from the container.
       */
      void pop_back() {

        if (m_contents_size) {

          size_type destroy_pos = previous_tail();
          decrement_tail();
          m_allocator.destroy (m_array + destroy_pos);
        }
      }

      /**
       * @brief
       * @param n
       */
      void skip (size_type len) {

        if (len >= m_contents_size) {
          clear();
        }
        else {

          while (len--) {
            pop_front();
          }
        }
      }

      /**
       * @brief
       * @param len
       */
      void chop (size_type len) {

        if (len >= m_contents_size) {
          clear();
        }
        else {

          while (len--) {
            pop_back();
          }
        }
      }

      /**
       * @brief
       * @param n
       * @return
       */
      reference operator[] (size_type const n) {

        return at_unchecked (n);
      }

      /**
       * @brief
       * @param n
       * @return
       */
      reference at (size_type const n) {

        return at_checked (n);
      }

      /**
       * @brief
       * @return
       */
      pointer getimpl() {

        return m_array;
      }

    private:
      reference at_unchecked (size_type const index) const {
        return m_array[index_to_subscript (index)];
      }

      reference at_checked (size_type const index) const {
        if (index >= m_contents_size) {
          throw std::out_of_range ("RingBuffer::at()");
        }
        return at_unchecked (index);
      }

      // Round an unbounded to an index into m_array
      size_type normalise (size_type const n) const {
        return n % m_capacity;
      }

      // Convert external index to an array subscript
      size_type index_to_subscript (size_type const index) const {
        return normalise (index + m_head);
      }

      void increment_tail() {
        ++m_contents_size;
        m_tail = next_tail();
      }

      void decrement_tail() {
        --m_contents_size;
        m_tail = previous_tail();
      }

      size_type next_tail() {
        return (m_tail + 1 == m_capacity) ? 0 : m_tail + 1;
      }

      size_type previous_tail() {
        return (m_tail == 0) ? m_capacity - 1 : m_tail - 1;
      }

      size_type previous_head() {
        return (m_head == 0) ? m_capacity - 1 : m_head - 1;
      }

      void increment_head() {
        // precondition: !empty()
        ++m_head;
        --m_contents_size;

        if (m_head == m_capacity) {
          m_head = 0;
        }
      }

      void decrement_head() {
        if (m_head == 0) {
          m_head = m_capacity;
        }

        --m_head;
        m_contents_size++;
      }

      template <typename I>
      void assign_into (I from, I const to) {
        if (m_contents_size > 0) {
          clear();
        }

        while (from != to) {
          push_back (*from);
          ++from;
        }
      }

    private:
      size_type      m_capacity;
      allocator_type m_allocator;
      pointer        m_array;
      size_type      m_head;
      size_type      m_tail;
      size_type      m_contents_size;
  };

  // Comparison operators
  // ---------------------------------------------------------------------------
  /**
   * @brief
   * @param lhs
   * @param rhs
   * @return
   */
  template <typename T, typename A>
  bool operator== (RingBuffer<T, A> const & lhs,
                   RingBuffer<T, A> const & rhs) {
    return lhs.size() == rhs.size()
           && std::equal (lhs.begin(), lhs.end(), rhs.begin());
  }

  /**
   * @brief
   * @param lhs
   * @param rhs
   * @return
   */
  template <typename T, typename A>
  bool operator!= (RingBuffer<T, A> const & lhs,
                   RingBuffer<T, A> const & rhs) {
    return ! (lhs == rhs);
  }

  /**
   * @brief
   * @param lhs
   * @param rhs
   * @return
   */
  template <typename T, typename A>
  bool operator< (RingBuffer<T, A> const & lhs,
                  RingBuffer<T, A> const & rhs) {
    return std::lexicographical_compare (
             lhs.begin(), lhs.end(),
             rhs.begin(), rhs.end());
  }

  /**
   * @brief
   * @param cb
   * @return
   */
  template <typename T, typename A>
  typename RingBuffer<T, A>::iterator
  begin (RingBuffer<T, A> & cb) {
    return cb.begin();
  }

  /**
   * @brief
   * @param cb
   * @return
   */
  template <typename T, typename A>
  typename RingBuffer<T, A>::iterator
  end (RingBuffer<T, A> & cb) {
    return cb.end();
  }

  /**
   * @brief
   * @param cb
   * @return
   */
  template <typename T, typename A>
  typename RingBuffer<T, A>::const_iterator
  begin (RingBuffer<const T, A> const & cb) {
    return cb.begin();
  }

  /**
   * @brief
   * @param cb
   * @return
   */
  template <typename T, typename A>
  typename RingBuffer<const T, A>::const_iterator
  end (RingBuffer<const T, A> const & cb) {
    return cb.end();
  }

}
/* ========================================================================== */
#endif // PIDUINO_RING_BUFFER_H
