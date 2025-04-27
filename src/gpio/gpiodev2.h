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

// based on https://gist.github.com/8c8678c65b884c62aca328a5bf57f4f5.git

#include <string>
#include <chrono>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

/**
   @namespace GpioDev2
   @brief Namespace containing classes and types for GPIO device interaction.
   Use the kernel GPIO interface version 2.
*/
namespace GpioDev2 {
  using ChipInfo = ::gpiochip_info;
  using LineInfo = ::gpio_v2_line_info;
  using LineConfig = ::gpio_v2_line_config;
  using LineRequest = ::gpio_v2_line_request;
  using LineValues = ::gpio_v2_line_values;
  using LineEvent = ::gpio_v2_line_event;

  class Line;

  /**
     @class GpioDev2::Chip
     @brief Represents a GPIO chip and provides methods to interact with it.

     This class encapsulates the functionality to open, close, and query information
     about a GPIO chip, as well as perform IOCTL operations.

     @note The class is not copyable or movable.
  */
  class Chip {
    public:
      /**
         @brief Constructor for the Chip class.
         @param consumer A string representing the consumer label for the chip.
      */
      Chip (const std::string &consumer = std::string()) : m_consumer (consumer), m_fd (-1), m_last_error (0), m_last_result (0) {
        memset (&m_dev_info, 0, sizeof (m_dev_info));
      }

      /**
         @tparam ParamType The type of the parameter for the IOCTL operation.
         @brief Performs an IOCTL operation on the GPIO chip.
         @param cmd The IOCTL command.
         @param param A pointer to the parameter for the IOCTL operation.
         @return True if the operation was successful, false otherwise.
      */
      template<class ParamType>
      bool ioCtl (unsigned long cmd, ParamType *param) {
        m_last_result = ::ioctl (m_fd, cmd, param);
        m_last_error = errno;
        return m_last_result != -1;
      }

      /**
         @brief Gets the last error code from the IOCTL operation.
         @return The error code.
      */
      int errorCode() const {
        return m_last_error;
      }

      /**
         @brief Gets the result of the last IOCTL operation.
         @return The result of the operation.
      */
      int result() const {
        return m_last_result;
      }

      /**
         @brief Gets the error message corresponding to the last error code.
         @return A string describing the error.
      */
      const char *errorMessage() const {
        return strerror (errorCode());
      }

      /**
         @brief Opens the GPIO chip device.
         @param dev The path to the GPIO chip device.
         @return True if the device was successfully opened, false otherwise.
      */
      bool open (const char *dev) {
        m_dev = dev;
        m_fd = ::open (dev, O_RDONLY);
        m_last_error = errno;

        return isOpen();
      }

      /**
         @brief Checks if the GPIO chip device is open.
         @return True if the device is open, false otherwise.
      */
      bool isOpen() const {
        return m_fd >= 0;
      }

      /**
         @brief Closes the GPIO chip device.
         @return True if the device was successfully closed, false otherwise.
      */
      bool close() {
        m_last_result = ::close (m_fd);
        m_last_error = errno;
        m_fd = -1;
        return m_last_result == 0;
      }

      /**
         @brief Fills the chip information structure with data from the device.
         @return True if the operation was successful, false otherwise.
      */
      bool fillInfo() {
        if (isOpen()) {
          return ioCtl (GPIO_GET_CHIPINFO_IOCTL, &m_dev_info);
        }
        return false;
      }

      /**
         @brief Gets the name of the GPIO chip.
         @return A string containing the name of the chip.
      */
      std::string name() const {
        return m_dev_info.name;
      }

      /**
         @brief Gets the label of the GPIO chip.
         @return A string containing the label of the chip.
      */
      std::string label() const {
        return m_dev_info.label;
      }

      /**
         @brief Gets the consumer label of the GPIO chip.
         @return A string containing the consumer label.
      */
      std::string consumer() const {
        return m_consumer;
      }

      /**
         @brief Gets the number of lines available on the GPIO chip.
         @return The number of lines.
      */
      uint32_t lines() const {
        return m_dev_info.lines;
      }

      /**
         @brief Gets the path to the GPIO chip device.
         @return A string containing the device path.
      */
      std::string  path() const {
        return m_dev;
      }

      /**
         @brief Retrieves information about a specific GPIO line.
         @param offset The offset of the GPIO line.
         @param info A pointer to a LineInfo structure to be filled with line information.
         @return True if the operation was successful, false otherwise.
      */
      bool lineInfo (uint32_t offset, LineInfo *info) {
        memset (info, 0, sizeof (*info));
        info->offset = offset;

        return ioCtl (GPIO_V2_GET_LINEINFO_IOCTL, info);
      }

    private:
      std::string m_dev;
      std::string m_consumer;
      ChipInfo m_dev_info;
      int m_fd;
      int m_last_error;
      int m_last_result;
  };


  /**
     @class GpioDev2::Line
     @brief Represents a GPIO line and provides methods to interact with it.

     This class encapsulates the functionality to open, close, configure, and
     manipulate GPIO lines, as well as handle events.

     @note The class is not copyable or movable.
  */
  class Line {
    public:
      /**
         @brief Constructor for the Line class.
         @param dev A pointer to the associated Chip object.
         @param offset The offset of the GPIO line.
      */
      Line (Chip *dev, uint32_t offset);

      /**
         @brief Constructor for the Line class with multiple lines.
         @param dev A pointer to the associated Chip object.
         @param num_lines The number of lines.
         @param offsets A pointer to an array of line offsets.
      */
      Line (Chip *dev, uint32_t num_lines, const uint32_t *offsets);

      /**
         @brief Constructor for the Line class with a LineRequest object.
         @param dev A pointer to the associated Chip object.
         @param request A LineRequest object containing the line configuration.
      */
      Line (Chip *dev, const LineRequest &request);

      /**
         @brief Destructor for the Line class. Closes the line if it is open.
      */
      ~Line() {
        close();
      }

      Line (const Line &) = delete;
      Line (Line &&) = delete;
      Line &operator= (const Line &) = delete;
      Line &operator= (Line &&) = delete;

      /**
         @brief Gets the current line request configuration.
         @return A reference to the LineRequest object.
      */
      const LineRequest &request() const {
        return m_req;
      }

      /**
         @brief Gets the current line configuration.
         @return A reference to the LineConfig object.
      */
      const LineConfig &config() const {
        return m_req.config;
      }


      /**
       * @brief Retrieves information about GPIO lines.
       *
       * This function fetches information about a specified number of GPIO lines
       * and stores it in the provided `LineInfo` array. The number of lines to
       * retrieve is limited by the number of lines available in the request.
       *
       * @param info Pointer to an array of `LineInfo` structures where the line
       *             information will be stored.
       * @param num_lines The number of lines to retrieve information for. Defaults
       *                  to 1. If `num_lines` exceeds the number of lines available
       *                  in the request, it will be clamped to the maximum available.
       * @return `true` if the information for all requested lines was successfully
       *         retrieved, `false` otherwise.
       */
      bool getInfo (LineInfo *info, uint32_t num_lines = 1) {

        if (num_lines > m_req.num_lines) {
          num_lines = m_req.num_lines;
        }
        for (uint32_t i = 0; i < num_lines; ++i) {
          if (!m_chip->lineInfo (m_req.offsets[i], &info[i])) {
            return false;
          }
        }
        return true;
      }


      /**
         @brief Opens the GPIO line with the specified flags.
         @param flags The flags for opening the line.
         @return True if the line was successfully opened, false otherwise.
      */
      bool open (uint64_t flags);

      /**
         @brief Opens the GPIO line with the specified configuration.
         @param config The configuration for the line.
         @return True if the line was successfully opened, false otherwise.
      */
      bool open (const LineConfig &config);

      /**
         @brief Sets the configuration for the GPIO line.
         @param config The new configuration for the line.
         @return True if the configuration was successfully set, false otherwise.
      */
      bool setConfig (const LineConfig &config);

      /**
         @brief Checks if the GPIO line is open.
         @return True if the line is open, false otherwise.
      */
      bool isOpen() const;

      /**
         @brief Closes the GPIO line.
         @return True if the line was successfully closed, false otherwise.
      */
      bool close();

      /**
         @brief Sets the debounce period for the GPIO line.
         @param debounce_ms The debounce period in milliseconds.
         @return True if the debounce period was successfully set, false otherwise.
      */
      bool setDebounce (uint32_t debounce_ms);

      /**
         @brief Gets the debounce period for the GPIO line.
         @return The debounce period in milliseconds.
      */
      uint32_t debounce() const {
        return m_req.config.attrs[0].attr.debounce_period_us / 1000;
      }

      /**
         @brief Waits for an event on the GPIO line.
         @param event A reference to a LineEvent object to be filled with event data.
         @param timeout_ms The timeout in milliseconds. A negative value indicates no timeout.
         @return True if an event was successfully received, false otherwise.
      */
      bool waitForEvent (LineEvent &event, int timeout_ms = -1);

      /**
         @brief Gets the values of the GPIO line(s).
         @param values A reference to a LineValues object to be filled with line values.
         @return True if the values were successfully retrieved, false otherwise.
      */
      bool getValues (LineValues &values) const;

      /**
         @brief Gets the value of a specific GPIO line.
         @param line_no The line number (default is 0).
         @return The value of the line (true for high, false for low).
      */
      bool getValue (int line_no = 0) const;

      /**
         @brief Sets the value of a specific GPIO line.
         @param line_no The line number.
         @param value The value to set (true for high, false for low).
         @return True if the value was successfully set, false otherwise.
      */
      bool setValue (int line_no, bool value);

      /**
         @brief Sets the values of the GPIO line(s).
         @param values A reference to a LineValues object containing the values to set.
         @return True if the values were successfully set, false otherwise.
      */
      bool setValues (const LineValues &values);

    private:
      LineRequest m_req;
      Chip *m_chip;
  };

}
