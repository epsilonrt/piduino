/**
 * \file circular_buffer.h
 *
 * \brief   STL-style circular buffer
 * \author  Pete Goodliffe, modified and adapted for VC6 by Martin Moene
 * \date    12 September 2012
 * \since   0.0.0
 *
 * Copyright 2002 Pete Goodliffe All rights reserved.
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
 * boost::circular_buffer<>, see
 * http://www.boost.org/doc/libs/1_48_0/libs/circular_buffer/
 *
 * Notable changes with respect to he original implementation are:
 * - There's no policy to select between accept and reject on buffer full
 * - Added non-member begin(cb) and end(cb).
 * - The iterator class is defined inside the circular_buffer class
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

#ifndef SPM_CORE_CIRCULAR_BUFFER_H_INCLUDED
#define SPM_CORE_CIRCULAR_BUFFER_H_INCLUDED

#include <iterator>
#include <memory>
#include <stdexcept>

/**
 * \def SPM_CIRCULAR_BUFFER_COMPILER_IS_MSVC
 * Macro to recognize if the MS Visual C++ compiler is being used.
 */
#if defined( _MSC_VER )
# define SPM_CORE_CIRCULAR_BUFFER_COMPILER_IS_MSVC

/**
 * \def SPM_CORE_CIRCULAR_BUFFER_COMPILER_IS_MSVC6
 * Macro to recognize if the MS Visual C++ compiler 6 is being used.
 */
# if ( _MSC_VER >= 1200 ) && ( _MSC_VER < 1300 )
#  define SPM_CORE_CIRCULAR_BUFFER_COMPILER_IS_MSVC6
# endif
#endif

/**
 * spm core namespace.
 */
namespace spm {

  /**
   * STL-style circular buffer.
   */
  template < typename T, typename A = std::allocator<T> >
  class circular_buffer {
    public:
      typedef circular_buffer<T, A> self_type;

      typedef A allocator_type;

      typedef typename allocator_type::value_type        value_type;
      typedef typename allocator_type::pointer           pointer;
      typedef typename allocator_type::const_pointer     const_pointer;
      typedef typename allocator_type::reference         reference;
      typedef typename allocator_type::const_reference   const_reference;

      typedef typename allocator_type::size_type         size_type;
      typedef typename allocator_type::difference_type   difference_type;

      /*
       * lifetime:
       */

      ~circular_buffer() {
        clear();
        m_allocator.deallocate (m_array, m_capacity);
      }

      explicit circular_buffer (size_type const capacity = 1,
                                allocator_type const & allocator = allocator_type())
        : m_capacity (capacity)
        , m_allocator (allocator)
        // VC6 requires (void *) 0:
        , m_array (m_allocator.allocate (capacity, (void *) 0))
        , m_head (1)
        , m_tail (0)
        , m_contents_size (0) {
      }

      // Note: copy-initialiser constructor must be non-explicit for operator=():

      circular_buffer (self_type const & other)
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

      template < typename II >
      circular_buffer (II const from, II const to)
        : m_capacity (0)
        , m_allocator (allocator_type())
        , m_array (0)
        , m_head (0)
        , m_tail (0)
        , m_contents_size (0) {
        circular_buffer tmp (std::distance (from, to));
        tmp.assign_into (from, to);
        swap (tmp);
      }

      // Note: copy-swap idiom using value parameter
      circular_buffer & operator= (self_type other) {
        other.swap (*this);
        return *this;
      }

      void swap (self_type & other) {
        using std::swap;

        swap (m_capacity,      other.m_capacity);

        // no need to swap the allocator object
        // as circular_buffer types are identical:
        // swap( m_allocator, other.m_allocator );

        swap (m_array,         other.m_array);
        swap (m_head,          other.m_head);
        swap (m_tail,          other.m_tail);
        swap (m_contents_size, other.m_contents_size);
      }

      allocator_type get_allocator() const {
        return m_allocator;
      }

      /*
       * circular_buffer iterator class template:
       */

      template< typename E, typename EN >
      class iterator_ : public std::iterator< std::random_access_iterator_tag, EN > {
        public:
          typedef E elem_type;

          // non-const element type
          typedef EN elem_type_nc;

          typedef iterator_< elem_type, elem_type_nc > self_type;

          // VC6 requires <T,A>:
          typedef circular_buffer<T, A> circular_buffer_type;

#ifdef SPM_CORE_CIRCULAR_BUFFER_COMPILER_IS_MSVC6
          typedef typename circular_buffer_type::difference_type difference_type;
#endif

          /*
           * iterator lifetime:
           */

          iterator_ (circular_buffer_type * const buf, size_type const pos)
            : m_buf (buf)
            , m_pos (pos) {
          }

          iterator_ (circular_buffer_type const * const buf, size_type const pos)
            : m_buf (const_cast<circular_buffer_type * > (buf))
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
            return (*m_buf) [ m_pos ];
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
          circular_buffer_type * m_buf;
          size_type  m_pos;
      };

      /*
       * iterators:
       */

      typedef iterator_<value_type, value_type> iterator;
      typedef iterator_<const value_type, value_type> const_iterator;

#ifdef SPM_CORE_CIRCULAR_BUFFER_COMPILER_IS_MSVC6
      typedef std::reverse_iterator<iterator, T>        reverse_iterator;
      typedef std::reverse_iterator<const_iterator, T> const_reverse_iterator;
#else
      typedef std::reverse_iterator<iterator>          reverse_iterator;
      typedef std::reverse_iterator<const_iterator>    const_reverse_iterator;
#endif

      iterator begin() {
        return iterator (this, 0);
      }

      iterator end() {
        return iterator (this, size());
      }

      const_iterator begin() const {
        return const_iterator (this, 0);
      }

      const_iterator end() const {
        return const_iterator (this, size());
      }

      reverse_iterator rbegin() {
        return reverse_iterator (end());
      }

      reverse_iterator rend() {
        return reverse_iterator (begin());
      }

      const_reverse_iterator rbegin() const {
        return const_reverse_iterator (end());
      }

      const_reverse_iterator rend() const {
        return const_reverse_iterator (begin());
      }

      /*
       * size accessors:
       */

      bool empty() const {
        return 0 == m_contents_size;
      }

      size_type capacity() const {
        return m_capacity;
      }

      size_type size() const {
        return m_contents_size;
      }

      size_type max_size() const {
        return m_allocator.max_size();
      }

      /*
       * size modifiers:
       */

      // reserve shrinks or expands the internal buffer to the size given; if the
      // buffer shrinks, keep at most the last new_size items.
      void reserve (size_type const new_size) {
        if (new_size != capacity()) {
          circular_buffer tmp (new_size);

          // preferred:
          // const size_type offset = std::max( 0, size() - new_size );
          // however, VC6 may have defined max() as macro.
          const size_type offset =
            new_size < size() ? size() - new_size : 0;

          tmp.assign_into (begin() + offset, end());
          swap (tmp);
        }
      }

    private:
      // resize is not implemented
      void resize (size_type const new_size);

    public:
      /*
       * content accessors:
       */

      reference front() {
        return m_array[ m_head ];
      }

      reference back() {
        return m_array[ m_tail ];
      }

      const_reference front() const {
        return m_array[ m_head ];
      }

      const_reference back() const {
        return m_array[ m_tail ];
      }

      const_reference operator[] (size_type const n) const {
        return at_unchecked (n);
      }

      const_reference at (size_type const n) const {
        return at_checked (n);
      }

      /*
       * content modifiers:
       */

      void clear() {
        for (size_type n = 0; n < m_contents_size; ++n) {
          m_allocator.destroy (m_array + index_to_subscript (n));
        }

        m_head = 1;
        m_tail = m_contents_size = 0;
      }

      void push_back (value_type const & item) {
        size_type next = next_tail();

        if (m_contents_size == m_capacity) {
          m_array[ next ] = item;
          increment_head();
        }
        else {
          m_allocator.construct (m_array + next, item);
        }
        increment_tail();
      }

      void pop_front() {
        size_type destroy_pos = m_head;
        increment_head();
        m_allocator.destroy (m_array + destroy_pos);
      }

      reference operator[] (size_type const n) {
        return at_unchecked (n);
      }

      reference at (size_type const n) {
        return at_checked (n);
      }

      pointer getimpl() {
        return m_array;
      }

    private:
      reference at_unchecked (size_type const index) const {
        return m_array[ index_to_subscript (index) ];
      }

      reference at_checked (size_type const index) const {
        if (index >= m_contents_size) {
          throw std::out_of_range ("circular_buffer::at()");
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

      size_type next_tail() {
        return (m_tail + 1 == m_capacity) ? 0 : m_tail + 1;
      }

      void increment_head() {
        // precondition: !empty()
        ++m_head;
        --m_contents_size;

        if (m_head == m_capacity) {
          m_head = 0;
        }
      }

      template < typename I >
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

  /*
   * comparison operators:
   */

  template < typename T, typename A >
  bool operator== (circular_buffer<T, A> const & lhs,
                   circular_buffer<T, A> const & rhs) {
    return lhs.size() == rhs.size()
           && std::equal (lhs.begin(), lhs.end(), rhs.begin());
  }

  template < typename T, typename A >
  bool operator!= (circular_buffer<T, A> const & lhs,
                   circular_buffer<T, A> const & rhs) {
    return ! (lhs == rhs);
  }

  template < typename T, typename A >
  bool operator< (circular_buffer<T, A> const & lhs,
                  circular_buffer<T, A> const & rhs) {
    return std::lexicographical_compare (
             lhs.begin(), lhs.end(),
             rhs.begin(), rhs.end());
  }

  template < typename T, typename A >
  typename circular_buffer<T, A>::iterator
  begin (circular_buffer<T, A> & cb) {
    return cb.begin();
  }

  template < typename T, typename A >
  typename circular_buffer<T, A>::iterator
  end (circular_buffer<T, A> & cb) {
    return cb.end();
  }

  template < typename T, typename A >
  typename circular_buffer<T, A>::const_iterator
  begin (circular_buffer<const T, A> const & cb) {
    return cb.begin();
  }

  template < typename T, typename A >
  typename circular_buffer<const T, A>::const_iterator
  end (circular_buffer<const T, A> const & cb) {
    return cb.end();
  }

} // namespace spm

#undef SPM_CORE_CIRCULAR_BUFFER_COMPILER_IS_MSVC
#undef SPM_CORE_CIRCULAR_BUFFER_COMPILER_IS_MSVC6

#endif // SPM_CORE_CIRCULAR_BUFFER_H_INCLUDED

/*
 * end of file
 */
