/* Copyright © 2018-2025 Pascal JEAN, All rights reserved.
   This file is part of the Piduino Library.

   The Piduino Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   The Piduino Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with the Piduino Library; if not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

// based on https://www.geeksforgeeks.org/dsa/implement-thread-safe-queue-in-c/

namespace Piduino {
  /**
     @brief Thread-safe queue implementation.

     This class provides a thread-safe queue (TSQueue) for storing elements of type T.
     It uses a std::queue as the underlying container and synchronizes access using
     std::mutex and std::condition_variable to ensure safe concurrent access from multiple threads.

     @tparam T Type of elements stored in the queue.
  */
  template <typename T>
  class TSQueue {
    private:
      std::queue<T> m_queue; //< Underlying queue container.
      mutable std::mutex m_mutex; //< Mutex for synchronizing access.
      std::condition_variable m_cond; //< Condition variable for blocking/waking threads.

    public:
      /**
         @brief Pushes an item into the queue.

         Acquires the lock, adds the item to the queue, and notifies one waiting thread.

         @param item The item to be added to the queue.
      */
      void push (T item) {

        // Acquire lock
        std::unique_lock<std::mutex> lock (m_mutex);

        // Add item
        m_queue.push (item);

        // Notify one thread that
        // is waiting
        m_cond.notify_one();
      }

      /**
         @brief Pops an item from the queue.

         Waits until the queue is not empty, then retrieves and removes the front item.

         @return The item removed from the front of the queue.
      */
      T pop() {

        // acquire lock
        std::unique_lock<std::mutex> lock (m_mutex);

        // wait until queue is not empty
        m_cond.wait (lock,
        [this]() {
          return !m_queue.empty();
        });

        // retrieve item
        T item = m_queue.front();
        m_queue.pop();

        // return item
        return item;
      }

      /**
         @brief Checks if the queue is empty.

         Acquires the lock and checks if the queue is empty.

         @return true if the queue is empty, false otherwise.
      */
      bool empty() const {

        // acquire lock
        std::lock_guard<std::mutex> lock (m_mutex);

        // return if queue is empty
        return m_queue.empty();
      }

      /**
         @brief Gets the size of the queue.

         Acquires the lock and returns the number of items in the queue.

         @return The size of the queue.
      */
      size_t size() const {

        // acquire lock
        std::lock_guard<std::mutex> lock (m_mutex);

        // return the size of the queue
        return m_queue.size();
      }

      /**
         @brief Clears the queue.

         Acquires the lock and clears the queue, removing all items.
      */
      void clear() {

        // acquire lock
        std::lock_guard<std::mutex> lock (m_mutex);

        // clear the queue
        m_queue = std::queue<T>();
      }
  };
}