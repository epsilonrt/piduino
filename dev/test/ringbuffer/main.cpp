/**
 * \file RingBuffer-main.cpp
 *
 * \brief   Tests for STL-style circular buffer
 * \author  Martin Moene
 * \date    19 December 2011
 * \since   0.0.0
 */

#include <piduino/ringbuffer.h>

#include <cassert>
#include <algorithm>
#include <iterator>
#include <iostream>


/*
 * dimension_of( array ):
 */
template< int n >
struct char_array_wrapper {
  char result[n];
};

template< typename T, int s >
char_array_wrapper<s> the_type_of_the_variable_is_not_an_array (T const (&array) [s]) { }

#define dimension_of(v) sizeof( the_type_of_the_variable_is_not_an_array(v).result )

using namespace Piduino;

typedef RingBuffer<int> RingBufferInt;

const int data[] = { 1, 3, 5, 7, 9, 11, 13, 15, };

// -----------------------------------------------------------------------------
int * begin (int * pos) {

  return pos;
}

// -----------------------------------------------------------------------------
const int * begin (const int * pos) {

  return pos;
}

// -----------------------------------------------------------------------------
template <int N>
int * end (int (&array) [N]) {

  return &array[0] + N;
}

// -----------------------------------------------------------------------------
template <int N>
const int * end (const int (&array) [N]) {

  return &array[0] + N;
}

// -----------------------------------------------------------------------------
void testThatDefaultConstructedBufferCapacityMatches() {
  RingBufferInt b;

  assert (1 == b.capacity());
  assert (0 == b.size());
  assert (b.empty());
}

// -----------------------------------------------------------------------------
void testThatResizedBufferCapacityMatches() {
  RingBufferInt b;

  b.reserve (5);
  assert (5 == b.capacity());
  assert (0 == b.size());
  assert (b.empty());
}

// -----------------------------------------------------------------------------
void testThatPushBackBackContentsMatches() {
  RingBufferInt b (3);
  b.push_back (9);

  assert (! b.empty());
  assert (1 == b.size());
  assert (9 == b.back());
}

// -----------------------------------------------------------------------------
void testThatPushBackFrontContentsMatches() {
  RingBufferInt b (3);
  b.push_back (9);

  assert (1 == b.size());
  assert (9 == b.front());
}

// -----------------------------------------------------------------------------
void testThatPopFrontContentsMatches() {
  RingBufferInt b (3);
  b.push_back (9);
  b.push_back (7);

  b.pop_front();

  assert (1 == b.size());
  assert (7 == b.front());
}

// -----------------------------------------------------------------------------
void testThatClearEmptiesBuffer() {
  RingBufferInt b (3);
  b.push_back (9);

  assert (! b.empty());
  assert (1 == b.size());
  assert (9 == b.back());

  b.clear();
  assert (b.empty());
  assert (0 == b.size());
}

// -----------------------------------------------------------------------------
void testThatUncheckedIndexContentsMatches() {
  RingBufferInt b (3);
  b.push_back (9);
  b.push_back (7);

  assert (2 == b.size());
  assert (9 == b[0]);
  assert (7 == b[1]);
}

// -----------------------------------------------------------------------------
void testThatValidCheckedIndexContentsMatches() {
  RingBufferInt b (3);
  b.push_back (9);
  b.push_back (7);

  assert (2 == b.size());
  assert (9 == b.at (0));
  assert (7 == b.at (1));
}

// -----------------------------------------------------------------------------
void testThatInvalidCheckedIndexThrows() {
  RingBufferInt b (3);
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

// -----------------------------------------------------------------------------
void testThatCopyConstructedBufferContentsMatch() {
  const unsigned int N = 3;
  RingBufferInt b (N);

  b.push_back (1);
  b.push_back (2);
  b.push_back (5);

  RingBufferInt c (b);

  assert (N == b.size());
  assert (N == c.size());

  assert (std::equal (begin (b), end (b), begin (c)));
  assert (b == c);
}

// -----------------------------------------------------------------------------
void testThatIterationConstructedBufferSmallerThanCapacityContentsMatch() {
  const unsigned int N = 3;
  RingBufferInt b (begin (data), begin (data) + N);

  assert (N == b.size());

  assert (std::equal (begin (b), begin (b) + 3, begin (data)));
}

// -----------------------------------------------------------------------------
void testThatIterationConstructedBufferLargerThanCapacityContentsMatch() {
  const unsigned int N = dimension_of (data);
  const unsigned int M = 5;

  assert (N > M);

  RingBufferInt b (M);

  std::copy (begin (data), end (data), std::back_inserter (b));

  assert (M == b.size());

  assert (std::equal (begin (b), end (b), begin (data) + N - M));
}

// -----------------------------------------------------------------------------
void testThatAssignedBufferComparesEqual() {
  const unsigned int N = dimension_of (data);
  RingBufferInt b (begin (data), begin (data) + N);
  RingBufferInt c;

  c = b;

  assert (N == b.size());
  assert (N == c.size());

  assert (std::equal (begin (b), end (b), begin (data)));
}

// -----------------------------------------------------------------------------
void testThatEqualityOperatorComparesEqual() {
  const unsigned int N = dimension_of (data);
  RingBufferInt b (begin (data), begin (data) + N);
  RingBufferInt c (begin (data), begin (data) + N);

  assert (N == b.size());
  assert (N == c.size());

  assert (b == c);
  assert (c == b);
}

// -----------------------------------------------------------------------------
void testThatEqualityOperatorComparesNotEqual() {
  const unsigned int N = dimension_of (data) - 3;
  RingBufferInt b (begin (data) + 1, begin (data) + 1 + N);
  RingBufferInt c (begin (data) + 2, begin (data) + 2 + N);

  assert (N == b.size());
  assert (N == c.size());

  assert (! (b == c));
  assert (! (c == b));
}

// -----------------------------------------------------------------------------
void testThatInequalityOperatorComparesUnequal() {
  const unsigned int N = dimension_of (data) - 3;
  RingBufferInt b (begin (data) + 1, begin (data) + 1 + N);
  RingBufferInt c (begin (data) + 2, begin (data) + 2 + N);

  assert (N == b.size());
  assert (N == c.size());

  assert (b != c);
  assert (c != b);
}

// -----------------------------------------------------------------------------
void testThatInequalityOperatorComparesNotUnequal() {
  const unsigned int N = dimension_of (data);
  RingBufferInt b (begin (data), begin (data) + N);
  RingBufferInt c (begin (data), begin (data) + N);

  assert (N == b.size());
  assert (N == c.size());

  assert (! (b != c));
  assert (! (c != b));
}

// -----------------------------------------------------------------------------
void testThatLessThanOperatorComparesLessThan() {
  const unsigned int N = dimension_of (data) - 3;
  RingBufferInt b (begin (data) + 1, begin (data) + 1 + N);
  RingBufferInt c (begin (data) + 2, begin (data) + 2 + N);

  assert (N == b.size());
  assert (N == c.size());

  assert (b < c);
  assert (! (c < b));
}

/*
 * Iterators:
 */

// -----------------------------------------------------------------------------
void testThatIteratorAdvancesCorrectly() {
  const unsigned int N = dimension_of (data);
  RingBufferInt b (begin (data), begin (data) + N);

  RingBufferInt::iterator pos = begin (b);

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

// -----------------------------------------------------------------------------
void testThatIteratorDistanceIsCorrect() {
  const unsigned int N = dimension_of (data);
  RingBufferInt b (begin (data), begin (data) + N);

  RingBufferInt::iterator pos = begin (b);

  assert (static_cast<RingBufferInt::difference_type> (N) == std::distance (begin (b), end (b)));
}

// -----------------------------------------------------------------------------
void testThatNonconstIteratorCanBeConvertedToConstIterator() {
  RingBufferInt b;

  RingBufferInt::const_iterator pos = b.begin();
}

/*
 * check iterator const-ness:
 */

// -----------------------------------------------------------------------------
void testThatWriteToConstBufferIteratorFailsToCompile() {
  const unsigned int N = dimension_of (data);
  const RingBufferInt b (begin (data), begin (data) + N);

//   *( b.begin() ) = 33;
}

// -----------------------------------------------------------------------------
void testThatPopBackContentsMatches() {
  RingBufferInt b (3);
  b.push_back (9);
  b.push_back (7);
  b.push_back (5);
  b.push_back (3);

  b.pop_back();
  assert (2 == b.size());
  assert (5 == b.back());
  assert (7 == b.front());
  
  b.pop_back();
  assert (1 == b.size());
  assert (7 == b.back());
  assert (7 == b.front());
}

// -----------------------------------------------------------------------------
void testThatPushFrontContentsMatches() {
  RingBufferInt b (4);
  b.push_back (9);
  b.push_back (7);

  b.push_front(11);
  assert (3 == b.size());
  assert (7 == b.back());
  assert (11 == b.front());
  
  b.push_front(13);
  assert (4 == b.size());
  assert (7 == b.back());
  assert (13 == b.front());
}

// -----------------------------------------------------------------------------
void  testSkip() {
  RingBufferInt b (4);
  b.push_back (9);
  b.push_back (7);
  b.push_back (5);
  b.push_back (3);
  
  b.skip(2);
  assert (2 == b.size());
  assert (3 == b.back());
  assert (5 == b.front());
  
}

// -----------------------------------------------------------------------------
void  testChop() {
  RingBufferInt b (4);
  b.push_back (9);
  b.push_back (7);
  b.push_back (5);
  b.push_back (3);
  
  b.chop(2);
  assert (2 == b.size());
  assert (7 == b.back());
  assert (9 == b.front());
}

// -----------------------------------------------------------------------------
int main() {

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
  /*
   * New functions:
   * push_front()
   * pop_back()
   * skip(n)
   * chop(n)
   */
  testThatPopBackContentsMatches();
  testThatPushFrontContentsMatches();
  testSkip();
  testChop();

  return 0;
}
