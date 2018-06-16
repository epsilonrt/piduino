/**
 * \file circular_buffer-main.cpp
 *
 * \brief   Tests for STL-style circular buffer
 * \author  Martin Moene
 * \date    19 December 2011
 * \since   0.0.0
 */

// VC2010: cl -W4 -EHsc -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -I%BOOST_INCLUDE% circular_buffer-main.cpp
//    VC6: cl -W3 -EHsc -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -I%BOOST_INCLUDE% circular_buffer-main.cpp

#include "ringbuffer.h"

#include <cassert>

#include <algorithm>
#include <iterator>
#include <iostream>

#if defined( _MSC_VER ) && ( _MSC_VER >= 1200 ) && ( _MSC_VER < 1300 )
# define COMPILER_IS_MSVC6
#endif

/*
 * dimension_of( array ):
 */
#ifdef COMPILER_IS_MSVC6

# define dimension_of( a ) (sizeof( a ) / sizeof( 0[a] ) )

#else
template< int n >
struct char_array_wrapper {
  char result[n];
};

template< typename T, int s >
char_array_wrapper<s> the_type_of_the_variable_is_not_an_array (T const (&array) [s]) { }

# define dimension_of(v) sizeof( the_type_of_the_variable_is_not_an_array(v).result )

#endif

/*
 * value_type and test data:
 */
using namespace spm;

typedef int value_type;
typedef circular_buffer<value_type> circular_buffer_int;

const value_type data[] = { 1, 3, 5, 7, 9, 11, 13, 15, };

value_type *       begin (value_type * pos) {
  return pos;
}
const value_type * begin (const value_type * pos) {
  return pos;
}

/*
 * dimension_of( array )
 */
#if defined COMPILER_IS_MSVC6

// VC6 Note: size is fixed to that of data[]:

value_type * end (value_type (&a) [dimension_of (data)]) {
  return &a[0] + dimension_of (data);
}
const value_type * end (const value_type (&a) [dimension_of (data)]) {
  return &a[0] + dimension_of (data);
}

#else

template < int N >
value_type * end (value_type (&array) [N]) {
  return &array[0] + N;
}

template < int N >
const value_type * end (const value_type (&array) [N]) {
  return &array[0] + N;
}

#endif

/*
 * Circular buffer:
 */

void testThatDefaultConstructedBufferCapacityMatches() {
  circular_buffer_int b;

  assert (1 == b.capacity());
  assert (0 == b.size());
  assert (b.empty());
}

void testThatResizedBufferCapacityMatches() {
  circular_buffer_int b;

  b.reserve (5);
  assert (5 == b.capacity());
  assert (0 == b.size());
  assert (b.empty());
}

void testThatPushBackBackContentsMatches() {
  circular_buffer_int b (3);
  b.push_back (9);

  assert (! b.empty());
  assert (1 == b.size());
  assert (9 == b.back());
}

void testThatPushBackFrontContentsMatches() {
  circular_buffer_int b (3);
  b.push_back (9);

  assert (1 == b.size());
  assert (9 == b.front());
}

void testThatPopFrontContentsMatches() {
  circular_buffer_int b (3);
  b.push_back (9);
  b.push_back (7);

  b.pop_front();

  assert (1 == b.size());
  assert (7 == b.front());
}

void testThatClearEmptiesBuffer() {
  circular_buffer_int b (3);
  b.push_back (9);

  assert (! b.empty());
  assert (1 == b.size());
  assert (9 == b.back());

  b.clear();
  assert (b.empty());
  assert (0 == b.size());
}

void testThatUncheckedIndexContentsMatches() {
  circular_buffer_int b (3);
  b.push_back (9);
  b.push_back (7);

  assert (2 == b.size());
  assert (9 == b[0]);
  assert (7 == b[1]);
}

void testThatValidCheckedIndexContentsMatches() {
  circular_buffer_int b (3);
  b.push_back (9);
  b.push_back (7);

  assert (2 == b.size());
  assert (9 == b.at (0));
  assert (7 == b.at (1));
}

void testThatInvalidCheckedIndexThrows() {
  circular_buffer_int b (3);
  b.push_back (9);
  b.push_back (7);

  assert (2 == b.size());

  try {
    assert (9 == b.at (3));
  }
  catch (std::out_of_range const &) {
    return ;
  }
  assert (0 && "expected std::out_of_range exception");
}

void testThatCopyConstructedBufferContentsMatch() {
  const unsigned int N = 3;
  circular_buffer_int b (N);

  b.push_back (1);
  b.push_back (2);
  b.push_back (5);

  circular_buffer_int c (b);

  assert (N == b.size());
  assert (N == c.size());

  assert (std::equal (begin (b), end (b), begin (c)));
  assert (b == c);
}

void testThatIterationConstructedBufferSmallerThanCapacityContentsMatch() {
  const unsigned int N = 3;
  circular_buffer_int b (begin (data), begin (data) + N);

  assert (N == b.size());

  assert (std::equal (begin (b), begin (b) + 3, begin (data)));
}

void testThatIterationConstructedBufferLargerThanCapacityContentsMatch() {
  const unsigned int N = dimension_of (data);
  const unsigned int M = 5;

  assert (N > M);

  circular_buffer_int b (M);

  std::copy (begin (data), end (data), std::back_inserter (b));

  assert (M == b.size());

  assert (std::equal (begin (b), end (b), begin (data) + N - M));
}

void testThatAssignedBufferComparesEqual() {
  const unsigned int N = dimension_of (data);
  circular_buffer_int b (begin (data), begin (data) + N);
  circular_buffer_int c;

  c = b;

  assert (N == b.size());
  assert (N == c.size());

  assert (std::equal (begin (b), end (b), begin (data)));
}

void testThatEqualityOperatorComparesEqual() {
  const unsigned int N = dimension_of (data);
  circular_buffer_int b (begin (data), begin (data) + N);
  circular_buffer_int c (begin (data), begin (data) + N);

  assert (N == b.size());
  assert (N == c.size());

  assert (b == c);
  assert (c == b);
}

void testThatEqualityOperatorComparesNotEqual() {
  const unsigned int N = dimension_of (data) - 3;
  circular_buffer_int b (begin (data) + 1, begin (data) + 1 + N);
  circular_buffer_int c (begin (data) + 2, begin (data) + 2 + N);

  assert (N == b.size());
  assert (N == c.size());

  assert (! (b == c));
  assert (! (c == b));
}

void testThatInequalityOperatorComparesUnequal() {
  const unsigned int N = dimension_of (data) - 3;
  circular_buffer_int b (begin (data) + 1, begin (data) + 1 + N);
  circular_buffer_int c (begin (data) + 2, begin (data) + 2 + N);

  assert (N == b.size());
  assert (N == c.size());

  assert (b != c);
  assert (c != b);
}

void testThatInequalityOperatorComparesNotUnequal() {
  const unsigned int N = dimension_of (data);
  circular_buffer_int b (begin (data), begin (data) + N);
  circular_buffer_int c (begin (data), begin (data) + N);

  assert (N == b.size());
  assert (N == c.size());

  assert (! (b != c));
  assert (! (c != b));
}

void testThatLessThanOperatorComparesLessThan() {
  const unsigned int N = dimension_of (data) - 3;
  circular_buffer_int b (begin (data) + 1, begin (data) + 1 + N);
  circular_buffer_int c (begin (data) + 2, begin (data) + 2 + N);

  assert (N == b.size());
  assert (N == c.size());

  assert (b < c);
  assert (! (c < b));
}

/*
 * Iterators:
 */

void testThatIteratorAdvancesCorrectly() {
  const unsigned int N = dimension_of (data);
  circular_buffer_int b (begin (data), begin (data) + N);

  circular_buffer_int::iterator pos = begin (b);

  assert (data[0] == *pos);
  assert (data[1] == *++pos);
  assert (data[1] == *pos++);
  assert (data[2] == *pos);
  assert (data[3] == * (pos +  1));
  assert (data[3] == * (pos += 1));
  assert (data[2] == * (pos -  1));
  assert (data[2] == * (pos -= 1));
  assert (data[1] == *--pos);
  assert (data[1] == *pos--);
  assert (data[0] == *pos);
}

void testThatIteratorDistanceIsCorrect() {
  const unsigned int N = dimension_of (data);
  circular_buffer_int b (begin (data), begin (data) + N);

  circular_buffer_int::iterator pos = begin (b);

  assert (static_cast<circular_buffer_int::difference_type> (N) == std::distance (begin (b), end (b)));
}

void testThatNonconstIteratorCanBeConvertedToConstIterator() {
  circular_buffer_int b;

  circular_buffer_int::const_iterator pos = b.begin();
}

/*
 * check iterator const-ness:
 */

void testThatWriteToConstBufferIteratorFailsToCompile() {
  const unsigned int N = dimension_of (data);
  const circular_buffer_int b (begin (data), begin (data) + N);

//   *( b.begin() ) = 33;
}

/**
 * test program.
 */
int main() {
  /*
   * Circular buffer:
   */

  testThatDefaultConstructedBufferCapacityMatches();
  testThatResizedBufferCapacityMatches();

  testThatPushBackBackContentsMatches();
  testThatPushBackFrontContentsMatches();
  testThatPopFrontContentsMatches();
  testThatClearEmptiesBuffer();

  testThatUncheckedIndexContentsMatches();
  testThatValidCheckedIndexContentsMatches();
  testThatInvalidCheckedIndexThrows();

  testThatCopyConstructedBufferContentsMatch();
  testThatIterationConstructedBufferSmallerThanCapacityContentsMatch();
  testThatIterationConstructedBufferLargerThanCapacityContentsMatch();

  testThatAssignedBufferComparesEqual();
  testThatEqualityOperatorComparesEqual();
  testThatEqualityOperatorComparesNotEqual();
  testThatInequalityOperatorComparesUnequal();
  testThatInequalityOperatorComparesNotUnequal();

  testThatLessThanOperatorComparesLessThan();

  /*
   * Iterators:
   */

  testThatIteratorAdvancesCorrectly();
  testThatIteratorDistanceIsCorrect();

  testThatNonconstIteratorCanBeConvertedToConstIterator();

  // failure commented out:
  testThatWriteToConstBufferIteratorFailsToCompile();

  return 0;
}
