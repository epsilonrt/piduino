/* Copyright © 2014  Chris Desjardins, http://blog.chrisd.info cjd@chrisd.info
 * Copyright © 2018 Pascal JEAN, https://github.com/epsilonrt
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
 * You should have received a copy of the GNU Lesser General Public License
 * along with the Piduino Library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PIDUINO_THREADSAFEBUFFER_H
#define PIDUINO_THREADSAFEBUFFER_H

#include <list>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace Piduino {

  /**
   * @class ThreadSafeBuffer
   * @author Copyright © 2014 Chris Desjardins
   * @author Copyright © 2018 Pascal JEAN
   * @brief This provides a thread safe buffer
   */
  template <class T> class ThreadSafeBuffer {
    public:
      ThreadSafeBuffer()
        : _queue(),
          _queueMutex(),
          _msgNotification(),
          _numEnqueued (0) {
      }

      virtual ~ThreadSafeBuffer() {
      }

      virtual void write (const T& data) {
        {
          // create a new scope for the mutex
          std::unique_lock<std::mutex> lock (_queueMutex);
          writeData (data);
          _msgNotification.notify_all();
        }
      }

      virtual void write (const T * buf, size_t len) {
        {
          // create a new scope for the mutex
          std::unique_lock<std::mutex> lock (_queueMutex);
          writeData (buf, len);
          _msgNotification.notify_all();
        }
      }

      virtual void write (const std::vector<T>& dataVec) {
        write (dataVec.data(), dataVec.size());
      }

      virtual bool read (T& data, long msTimeout = 0) {
        bool ret = false;
        {
          // create a new scope for the mutex
          std::unique_lock<std::mutex> lock (_queueMutex);
          waitForData (lock, msTimeout);
          ret = readData (data);
        }
        return ret;
      }

      // Dequeue everything
      virtual size_t read (std::vector<T>& dataVec, long msTimeout = 0) {
        size_t size = 0;
        {
          // create a new scope for the mutex
          std::unique_lock<std::mutex> lock (_queueMutex);
          waitForData (lock, msTimeout);
          size = readData (dataVec);
        }
        return size;
      }

      // Dequeue everything
      virtual size_t read (T * buf, size_t max, long msTimeout = 0) {
        size_t size = 0;
        {
          // create a new scope for the mutex
          std::unique_lock<std::mutex> lock (_queueMutex);
          waitForData (lock, msTimeout);
          size = readData (buf, max);
        }
        return size;
      }

      virtual bool peek (T& data, long msTimeout = 0) {
        bool ret = false;
        {
          // create a new scope for the mutex
          std::unique_lock<std::mutex> lock (_queueMutex);
          waitForData (lock, msTimeout);
          ret = peekData (data);
        }
        return ret;
      }

      // Dequeue everything
      virtual size_t peek (std::vector<T>& dataVec, long msTimeout = 0) {
        size_t size = 0;
        {
          // create a new scope for the mutex
          std::unique_lock<std::mutex> lock (_queueMutex);
          waitForData (lock, msTimeout);
          size = peekData (dataVec);
        }
        return size;
      }

      // Dequeue everything
      virtual size_t peek (T * buf, size_t max, long msTimeout = 0) {
        size_t size = 0;
        {
          // create a new scope for the mutex
          std::unique_lock<std::mutex> lock (_queueMutex);
          waitForData (lock, msTimeout);
          size = peekData (buf, max);
        }
        return size;
      }

      size_t size() const {
        return _numEnqueued;
      }

      /*
      ** This function allows you to perform operations on the
      ** vector in a thread safe way. The functor is a function
      ** with the following signature:
      ** int func(std::list<T> &);
      ** The return value is the number of elements added or removed
      ** from the list, for example if 5 elements were removed
      ** and 3 new elements were added then the return value should
      ** be -2.
      */
      template <typename Functor> void iterate (Functor functor) {
        {
          // create a new scope for the mutex
          std::unique_lock<std::mutex> lock (_queueMutex);
          // the return value of this functor is added to the _numEnqueued
          // so if you add buffers then return the number of buffers added
          // or if you remove buffers then return -number of buffers removed
          int numChanged = functor (_queue);
          _numEnqueued += numChanged;
        }
      }

    protected:

      void waitForData (std::unique_lock<std::mutex>& lock, long msTimeout) {
        if (msTimeout != 0) {
          // This function assumes that _queueMutex is locked already!
          std::chrono::system_clock::time_point timeLimit = std::chrono::system_clock::now() +
              std::chrono::milliseconds (msTimeout);
          while (_queue.empty() == true) {
            // if timeout is specified, then wait until the time is up
            // otherwise wait forever (forever is msTimeout = -1)
            if (msTimeout > 0) {
              _msgNotification.wait_until (lock, timeLimit);
              if (std::chrono::system_clock::now() >= timeLimit) {
                break;
              }
            }
            else {
              _msgNotification.wait (lock);
            }
          }
        }
      }

      void writeData (const T& data) {
        // This function assumes that _queueMutex is locked already!
        _queue.push_back (data);
        _numEnqueued++;
      }

      void writeData (const T * buf, size_t len) {
        // This function assumes that _queueMutex is locked already!
        while (len--) {
          writeData (*buf++);
        }
      }

      bool readData (T& data) {
        // This function assumes that _queueMutex is locked already!
        bool ret = false;
        if (_queue.empty() == false) {
          data = _queue.front();
          _queue.pop_front();
          _numEnqueued--;
          ret = true;
        }
        return ret;
      }

      size_t readData (std::vector<T>& dataVec) {
        // This function assumes that _queueMutex is locked already!
        size_t size = 0;
        T data;
        while (readData (data) == true) {
          size += sizeOfData (data);
          dataVec.push_back (data);
        }
        return size;
      }

      size_t readData (T * buf, size_t max) {
        // This function assumes that _queueMutex is locked already!
        max = std::min (_numEnqueued, max);
        size_t size = 0;
        T data;
        while ( (readData (data) == true) && (size <= max)) {
          *buf++ = data;
          size += sizeOfData (data);
        }
        return size;
      }

      bool peekData (T& data) {
        // This function assumes that _queueMutex is locked already!
        bool ret = false;
        if (_queue.empty() == false) {
          data = _queue.front();
          ret = true;
        }
        return ret;
      }

      size_t peekData (std::vector<T>& dataVec) {
        // This function assumes that _queueMutex is locked already!
        size_t size = 0;
        T data;
        while (peekData (data) == true) {
          size += sizeOfData (data);
          dataVec.push_back (data);
        }
        return size;
      }

      size_t peekData (T * buf, size_t max) {
        // This function assumes that _queueMutex is locked already!
        max = std::min (_numEnqueued, max);
        size_t size = 0;
        T data;
        while ( (peekData (data) == true) && (size <= max)) {
          *buf++ = data;
          size += sizeOfData (data);
        }
        return size;
      }

      virtual size_t sizeOfData (const T&) const {
        return sizeof (T);
      }

      std::list<T> _queue;
      std::mutex _queueMutex;
      std::condition_variable _msgNotification;
      size_t _numEnqueued;
  };
}
/* ========================================================================== */
#endif /* PIDUINO_THREADSAFEBUFFER_H defined */
