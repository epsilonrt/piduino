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
#include <stdexcept>
#include <system_error>
#include <iostream>

#include <cstring>
#include <cerrno>
#include <cstdint>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <linux/gpio.h>

#include <piduino/global.h>

/**
   @namespace GpioDev2
   @brief Namespace containing classes and types for GPIO device interaction.
   Use the kernel GPIO interface version 2.
   This API is documented in the Linux kernel documentation:
   https://www.kernel.org/doc/html/next/userspace-api/gpio/chardev.html
*/
namespace GpioDev2 {

  class Line;

  #ifdef __DOXYGEN__
  /**
    The maximum size of name and label arrays.

    Must be a multiple of 8 to ensure 32/64-bit alignment of structs.
  */
#define GPIO_MAX_NAME_SIZE 32
  /**
     Maximum number of requested lines.

     Must be no greater than 64, as bitmaps are restricted here to 64-bits
     for simplicity, and a multiple of 2 to ensure 32/64-bit alignment of
     structs.
  */
#define GPIO_V2_LINES_MAX 64

  /**
     The maximum number of configuration attributes associated with a line
     request.
  */
#define GPIO_V2_LINE_NUM_ATTRS_MAX 10
  /**
     LineAttribute.id values

     identifying which field of the attribute union is in use.
  */
  enum gpio_v2_line_attr_id {
    GPIO_V2_LINE_ATTR_ID_FLAGS    = 1, ///< flags field is in use
    GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES  = 2, ///< values field is in use
    GPIO_V2_LINE_ATTR_ID_DEBOUNCE   = 3, ///< debounce_period_us field is in use
  };

  /**
     LineAttribute.flags values
  */
  enum gpio_v2_line_flag {
    GPIO_V2_LINE_FLAG_USED      = _BITULL (0), ///< line is not available for request
    GPIO_V2_LINE_FLAG_ACTIVE_LOW    = _BITULL (1), ///< line active state is physical low
    GPIO_V2_LINE_FLAG_INPUT     = _BITULL (2), ///< line is an input
    GPIO_V2_LINE_FLAG_OUTPUT    = _BITULL (3), ///< line is an output
    GPIO_V2_LINE_FLAG_EDGE_RISING   = _BITULL (4), ///< line detects rising (inactive to active) edges
    GPIO_V2_LINE_FLAG_EDGE_FALLING    = _BITULL (5), ///< line detects falling (active to inactive) edges
    GPIO_V2_LINE_FLAG_OPEN_DRAIN    = _BITULL (6), ///< line is an open drain output
    GPIO_V2_LINE_FLAG_OPEN_SOURCE   = _BITULL (7), ///< line is an open source output
    GPIO_V2_LINE_FLAG_BIAS_PULL_UP    = _BITULL (8), ///< line has pull-up bias enabled
    GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN  = _BITULL (9), ///< line has pull-down bias enabled
    GPIO_V2_LINE_FLAG_BIAS_DISABLED   = _BITULL (10), ///< line has bias disabled
    GPIO_V2_LINE_FLAG_EVENT_CLOCK_REALTIME  = _BITULL (11), ///< line events contain REALTIME timestamps
    GPIO_V2_LINE_FLAG_EVENT_CLOCK_HTE = _BITULL (12), ///< line events contain timestamps from the hardware timestamping engine (HTE) subsystem
  };

  /**
     LineEvent.id values
  */
  enum gpio_v2_line_event_id {
    GPIO_V2_LINE_EVENT_RISING_EDGE  = 1, ///< event triggered by a rising edge
    GPIO_V2_LINE_EVENT_FALLING_EDGE = 2, ///< event triggered by a falling edge
  };
  #endif

  /**
    @brief Information about a certain GPIO chip
  */
  struct ChipInfo : public ::gpiochip_info {
    /**
       @brief Default constructor initializes the ChipInfo structure.
    */
    ChipInfo() {
      memset (this, 0, sizeof (*this));
    }

    #ifdef __DOXYGEN__
    char name[GPIO_MAX_NAME_SIZE]; ///< the Linux kernel name of this GPIO chip, such as "gpiochip0"
    char label[GPIO_MAX_NAME_SIZE]; ///< a functional name for this GPIO chip, such as a product number, may be empty (i.e. label[0] == '\0')
    __u32 lines; ///< number of GPIO lines on this chip
    #endif
  };

  /**
    @brief a configurable attribute of a line
  */
  struct LineAttribute : public ::gpio_v2_line_attribute {
    /**
       @brief Default constructor initializes the LineAttribute structure.
    */
    LineAttribute() {
      memset (this, 0, sizeof (*this));
    }

    /**
       @brief Constructor initializes the LineAttribute structure with a specific id.
       @param id The identifier for the line attribute.
    */
    LineAttribute (uint32_t id) {
      memset (this, 0, sizeof (*this));
      this->id = id;
    }
    #ifdef __DOXYGEN__
    __u32 id; ///< the identifier for the line attribute, such as #GPIO_V2_LINE_ATTR_ID_FLAGS
    __u32 padding; ///< reserved for future use, must be zero filled
    union {
      /**
         @brief the flags for the GPIO line, with values from #gpio_v2_line_flag

        if id is #GPIO_V2_LINE_ATTR_ID_FLAGS, the flags for the GPIO
        line, with values from #gpio_v2_line_flag, such as
        #GPIO_V2_LINE_FLAG_ACTIVE_LOW, #GPIO_V2_LINE_FLAG_OUTPUT etc, added
        together.  This overrides the default flags contained in the &struct
        LineConfig for the associated line.
      */
      __aligned_u64 flags;
      /**
         @brief a bitmap containing the values to which the lines will be set

        if id is #GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES, a bitmap
        containing the values to which the lines will be set, with each bit
        number corresponding to the index into &struct
        LineRequest.offsets
      */
      __aligned_u64 values;
      /**
         @brief the desired debounce period, in microseconds

        if id is #GPIO_V2_LINE_ATTR_ID_DEBOUNCE, the
        desired debounce period, in microseconds
      */
      __u32 debounce_period_us;
    };
    #endif
  };

  /**
    @brief Information about a certain GPIO line

    @note This structure is used to retrieve information about a GPIO line
          and is part of the ABI v2. It replaces the older &struct gpioline_info
          structure from ABI v1. The fields are similar but the structure is
          aligned to 64 bits and includes additional attributes for line
          configuration.
  */
  struct LineInfo : public ::gpio_v2_line_info {
    /**
       @brief Default constructor initializes the LineInfo structure.
    */
    LineInfo() {
      memset (this, 0, sizeof (*this));
    }

    /**
       @brief Constructor initializes the LineInfo structure with a specific offset.
       @param offset The local offset on the GPIO chip.
    */
    LineInfo (uint32_t offset) {
      memset (this, 0, sizeof (*this));
      this->offset = offset;
    }

    /**
       @brief Print the attributes of this GPIO line to the specified output stream.
       @param os The output stream to which the attributes will be printed, defaults to std
       cout.
    */
    void printAttributes (std::ostream &os = std::cout) const;

    /**
       @brief Overloaded output operator to print the LineInfo object.
       @param os The output stream to which the LineInfo will be printed.
       @param info The LineInfo object to be printed.
       @return The output stream after printing the LineInfo object.
    */
    friend std::ostream &operator<< (std::ostream &os, const LineInfo &info);

    #ifdef __DOXYGEN__
    /**
       @brief the Linux kernel name of this GPIO line

      the Linux kernel name of this GPIO line,
      such as the output pin of the line on the chip, a rail or a pin header name on
      a board, as specified by the GPIO chip, may be empty (i.e. name[0] == '\0')
    */
    char name[GPIO_MAX_NAME_SIZE];
    /**
       @brief a functional name for the consumer of this GPIO line

      a functional name for the consumer of this GPIO line as set
      by whatever is using it, will be empty if there is no current user but
      may also be empty if the consumer doesn't set this up
    */
    char consumer[GPIO_MAX_NAME_SIZE];
    /**
       @brief the local offset on this GPIO chip

      the local offset on this GPIO chip, fill this in when
      requesting the line information from the kernel
    */
    __u32 offset;
    /**
      @brief the number of attributes in #attrs
    */
    __u32 num_attrs;
    /**
       @brief flags for this GPIO line

      flags for this GPIO line, with values from #gpio_v2_line_flag,
      such as #GPIO_V2_LINE_FLAG_ACTIVE_LOW, #GPIO_V2_LINE_FLAG_OUTPUT etc, added together
    */
    __aligned_u64 flags;
    /**
      @brief the configuration attributes associated with the line
    */
    struct LineAttribute attrs[GPIO_V2_LINE_NUM_ATTRS_MAX];
    /**
      @brief reserved for future use
    */
    __u32 padding[4];
    #endif
  };

  /**
    @brief a configuration attribute associated with one or more of the requested lines.
  */
  struct LineConfigAttribute : public ::gpio_v2_line_config_attribute {
    /**
       @brief Default constructor initializes the LineConfigAttribute structure.
    */
    LineConfigAttribute() {
      memset (this, 0, sizeof (*this));
    }

    /**
       @brief Constructor initializes the LineConfigAttribute structure with a specific attribute.
       @param attr The line attribute to be associated with the requested lines.
    */
    LineConfigAttribute (const LineAttribute &attr) {
      memset (this, 0, sizeof (*this));
      this->attr = attr;
    }

    #ifdef __DOXYGEN__
    /**
      @brief the configurable attribute
    */
    struct LineAttribute attr;
    /**
      @brief a bitmap identifying the lines to which the attribute applies

      a bitmap identifying the lines to which the attribute applies,
      with each bit number corresponding to the index into &struct
      LineRequest.offsets
    */
    __aligned_u64 mask;
    #endif
  };


  /**
    @brief Configuration for GPIO lines
  */
  struct LineConfig : public ::gpio_v2_line_config {
    /**
       @brief Default constructor initializes the LineConfig structure.
    */
    LineConfig() {
      clear();
    }

    /**
       @brief Constructor initializes the LineConfig structure with specific flags.
       @param flags The flags for the GPIO lines.
    */
    LineConfig (std::uint64_t flags) {
      clear();
      this->flags = flags;
    }

    /**
       @brief Clear the LineConfig structure by zeroing all fields.
    */
    void clear() {
      memset (this, 0, sizeof (*this));
    }

    #ifdef __DOXYGEN__
    /**
        @brief flags for the GPIO lines

      flags for the GPIO lines, with values from
      #gpio_v2_line_flag, such as #GPIO_V2_LINE_FLAG_ACTIVE_LOW,
      #GPIO_V2_LINE_FLAG_OUTPUT etc, added together.  This is the default for
      all requested lines but may be overridden for particular lines using
      #attrs.
    */
    __aligned_u64 flags;
    /**
      @brief the number of attributes in #attrs
    */
    __u32 num_attrs;
    /**
       @brief padding reserved for future use and must be zero filled
    */
    __u32 padding[5];
    /**
       @brief the configuration attributes associated with the requested lines

      the configuration attributes associated with the requested
      lines.  Any attribute should only be associated with a particular line
      once.  If an attribute is associated with a line multiple times then the
      first occurrence (i.e. lowest index) has precedence.
    */
    struct LineConfigAttribute attrs[GPIO_V2_LINE_NUM_ATTRS_MAX];
    #endif
  };

  /**
    @brief Information about a request for GPIO lines.
  */
  struct LineRequest : public ::gpio_v2_line_request {
    /**
       @brief Default constructor initializes the LineRequest structure.
    */
    LineRequest() {
      memset (this, 0, sizeof (*this));
      num_lines = 0;
      event_buffer_size = 0;
      fd = -1;
    }

    /**
       @brief Constructor initializes the LineRequest structure with specific consumer label.
       @param consumer The consumer label for the GPIO lines.
    */
    LineRequest (const std::string &consumer) {
      memset (this, 0, sizeof (*this));
      strncpy (this->consumer, consumer.c_str(), GPIO_MAX_NAME_SIZE - 1);
      num_lines = 0;
      event_buffer_size = 0;
      fd = -1;
    }

    #ifdef __DOXYGEN__
    /**
       @brief  an array of desired lines, specified by offset index for the

      an array of desired lines, specified by offset index for the
      associated GPIO chip
    */
    __u32 offsets[GPIO_V2_LINES_MAX];
    /**
       @brief a desired consumer label for the selected GPIO lines

      a desired consumer label for the selected GPIO lines such as
      "my-bitbanged-relay"
    */
    char consumer[GPIO_MAX_NAME_SIZE];
    /**
      @brief requested configuration for the lines
    */
    struct LineConfig config;
    /**
       @brief number of lines requested in this request

      number of lines requested in this request, i.e. the number
      of valid fields in the #GPIO_V2_LINES_MAX sized arrays, set to 1 to
      request a single line
    */
    __u32 num_lines;
    /**
       @brief a suggested minimum number of line events that the kernel should buffer

      a suggested minimum number of line events that the
      kernel should buffer.  This is only relevant if edge detection is
      enabled in the configuration. Note that this is only a suggested value
      and the kernel may allocate a larger buffer or cap the size of the
      buffer. If this field is zero then the buffer size defaults to a minimum
      of @num_lines * 16.
    */
    __u32 event_buffer_size;
    /**
      @brief padding reserved for future use and must be zero filled
    */
    __u32 padding[5];
    /**
       @brief a file descriptor representing the request

      after a successful #GPIO_V2_GET_LINE_IOCTL operation, contains
      a valid anonymous file descriptor representing the request
    */
    __s32 fd;
    #endif
  };

  /**
     @brief Values of GPIO lines
  */
  struct LineValues : public ::gpio_v2_line_values {
    /**
       @brief Default constructor initializes the LineValues structure.
    */
    LineValues() {
      memset (this, 0, sizeof (*this));
    }

    /**
       @brief Constructor initializes the LineValues structure with specific bits and mask.
       @param bits The bitmap containing the value of the lines.
       @param mask The bitmap identifying the lines to get or set.
    */
    LineValues (std::uint64_t bits, std::uint64_t mask) {
      memset (this, 0, sizeof (*this));
      this->bits = bits;
      this->mask = mask;
    }

    #ifdef __DOXYGEN__
    /**
       @brief a bitmap containing the value of the lines

      a bitmap containing the value of the lines, set to 1 for active and 0 for inactive
    */
    __aligned_u64 bits;
    /**
       @brief a bitmap identifying the lines to get or set

      a bitmap identifying the lines to get or set, with each bit
      number corresponding to the index into LineRequest.offsets
    */
    __aligned_u64 mask;
    #endif
  };

  /**
     @brief The actual event being pushed to userspace.

     By default the @timestamp_ns is read from #CLOCK_MONOTONIC and is
     intended to allow the accurate measurement of the time between events.
     It does not provide the wall-clock time.

     If the #GPIO_V2_LINE_FLAG_EVENT_CLOCK_REALTIME flag is set then the
     @timestamp_ns is read from #CLOCK_REALTIME.

     If the #GPIO_V2_LINE_FLAG_EVENT_CLOCK_HTE flag is set then the
     @timestamp_ns is provided by the hardware timestamping engine (HTE)
     subsystem.
  */
  struct LineEvent : public ::gpio_v2_line_event {
    /**
       @brief Default constructor initializes the LineEvent structure.
    */
    LineEvent() {

      memset (this, 0, sizeof (*this));
    }

    #ifdef __DOXYGEN__
    /**
      @brief best estimate of time of event occurrence, in nanoseconds
    */
    __aligned_u64 timestamp_ns;
    /**
      @brief event identifier with value from #gpio_v2_line_event_id
    */
    __u32 id;
    /**
      @brief the offset of the line that triggered the event
    */
    __u32 offset;
    /**
       @brief the sequence number for this event

      the sequence number for this event in the sequence of events for all the lines in this line request
    */
    __u32 seqno;
    /**
       @brief the sequence number for this event on a particular line

      the sequence number for this event in the sequence of events on this particular line
    */
    __u32 line_seqno;
    /**
      @brief reserved for future use
    */
    __u32 padding[6];
    #endif
  };

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
      Chip (const std::string &consumer = std::string()) : m_consumer (consumer), m_fd (-1), m_last_error (0), m_last_result (0) {}

      /**
         @brief Gets the consumer label of the GPIO chip.
         @return A string containing the consumer label.
      */
      std::string consumer() const {
        return m_consumer;
      }

      /**
         @tparam ParamType The type of the parameter for the IOCTL operation.
         @brief Performs an IOCTL operation on the GPIO chip.
         @param cmd The IOCTL command.
         @param param A pointer to the parameter for the IOCTL operation.
         @return true if the operation was successful, false otherwise.
      */
      template<class ParamType>
      bool ioCtl (unsigned long cmd, ParamType *param) {

        m_last_result = ::ioctl (m_fd, cmd, param);
        if (m_last_result == -1) {

          m_last_error = errno;
          return false;
        }
        m_last_error = 0;
        return true;
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
         @return true if the device was successfully opened, false otherwise.
      */
      bool open (const char *dev) {

        m_dev = dev;
        m_fd = ::open (dev, O_RDONLY);
        if (!isOpen()) {

          m_last_error = errno;
          return false;
        }
        m_last_error = 0;
        return true;
      }

      /**
         @brief Opens the GPIO chip device using a string.
         @param dev The path to the GPIO chip device as a string.
         @return true if the device was successfully opened, false otherwise.
      */
      bool open (const std::string &dev) {

        return open (dev.c_str());
      }

      /**
         @brief Checks if the GPIO chip device is open.
         @return true if the device is open, false otherwise.
      */
      bool isOpen() const {

        return m_fd != -1;
      }

      /**
         @brief Gets the path to the GPIO chip device.
         @return A string containing the device path.
      */
      std::string  path() const {

        return m_dev;
      }

      /**
         @brief Closes the GPIO chip device.
         @return true if the device was successfully closed, false otherwise.
      */
      bool close() {

        if (isOpen()) {
          // Close the file descriptor and reset it
          // to -1 to indicate that the device is closed.
          m_last_result = ::close (m_fd);
          m_fd = -1;
          if (m_last_result == -1) {

            m_last_error = errno;
            return false;
          }
          m_last_error = 0;
        }
        return true;
      }

      /**
         @brief Fills the chip information structure with data from the device.
         @return true if the operation was successful, false otherwise.
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
         @brief Gets the number of lines available on the GPIO chip.
         @return The number of lines.
      */
      uint32_t lines() const {

        return m_dev_info.lines;
      }

      /**
         @brief Retrieves information about a specific GPIO line.
         @param offset The offset of the GPIO line.
         @param info A pointer to a LineInfo structure to be filled with line information.
         @return true if the operation was successful, false otherwise.
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
      Line (Chip *dev, uint32_t offset) : Line (dev, 1, &offset) {}

      /**
         @brief Constructor for the Line class with multiple lines.
         @param dev A pointer to the associated Chip object.
         @param num_lines The number of lines.
         @param offsets A pointer to an array of line offsets.
      */
      Line (Chip *dev, uint32_t num_lines, const uint32_t *offsets) : m_chip (dev), m_req (dev->consumer())  {

        m_req.num_lines = num_lines;
        m_req.fd = -1;
        memcpy (m_req.offsets, offsets, num_lines * sizeof (uint32_t));
      }

      /**
         @brief Constructor for the Line class with a LineRequest object.
         @param dev A pointer to the associated Chip object.
         @param request A LineRequest object containing the line configuration.
      */
      Line (Chip *dev, const LineRequest &request) : m_chip (dev), m_req (request) {

        strncpy (m_req.consumer, dev->consumer().c_str(), sizeof (m_req.consumer));
      }

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
         @brief Gets the number of lines in the request.
         @return The number of lines in the request.
      */
      uint32_t numLines() const {

        return m_req.num_lines;
      }

      /**
         @brief Gets the offset of a specific line in the request.
         @param line_no The line number (default is 0).
         @return The offset of the specified line.
         @throws std::out_of_range if the line number is out of range.
      */
      uint32_t offset (uint32_t line_no = 0) const {

        if (line_no >= m_req.num_lines) {

          throw std::out_of_range ("Line number out of range");
        }
        return m_req.offsets[line_no];
      }

      /**
         @brief Gets the current line configuration.
         @return A reference to the LineConfig object.
      */
      const LineConfig &config() const {

        return reinterpret_cast<const LineConfig &> (m_req.config);
      }

      /**
         @brief Retrieves information about GPIO lines.

         This function fetches information about a specified number of GPIO lines
         and stores it in the provided `LineInfo` array. The number of lines to
         retrieve is limited by the number of lines available in the request.

         @param [in] info Pointer to an array of `LineInfo` structures where the line
                     information will be stored.
         @param num_lines The number of lines to retrieve information for. Defaults
                          to 1. If `num_lines` exceeds the number of lines available
                          in the request, it will be clamped to the maximum available.
         @return `true` if the information for all requested lines was successfully
                 retrieved, `false` otherwise.
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
         @return true if the line was successfully opened, false otherwise.
      */
      bool open (uint64_t flags) {

        if (m_chip->isOpen() && !isOpen()) {

          m_req.config.flags = flags;
          return m_chip->ioCtl (GPIO_V2_GET_LINE_IOCTL, &m_req);
        }
        return isOpen();
      }

      /**
         @brief Opens the GPIO line with the specified configuration.
         @param config The configuration for the line.
         @return true if the line was successfully opened, false otherwise.
      */
      bool open (const LineConfig &config) {

        if (m_chip->isOpen() && !isOpen()) {

          m_req.config = config;
          return m_chip->ioCtl (GPIO_V2_GET_LINE_IOCTL, &m_req);
        }
        return isOpen();
      }

      /**
         @brief Sets the configuration for the GPIO line.
         @param config The new configuration for the line.
         @return true if the configuration was successfully set, false otherwise.
      */
      bool setConfig (const LineConfig &config) {

        m_req.config = config;
        if (isOpen()) {

          int ret = ::ioctl (m_req.fd, GPIO_V2_LINE_SET_CONFIG_IOCTL, &config);
          return (ret >= 0);
        }
        return false;
      }

      /**
         @brief Checks if the GPIO line is open.
         @return true if the line is open, false otherwise.
      */
      bool isOpen() const {

        return m_req.fd >= 0;
      }

      /**
         @brief Closes the GPIO line.
         @return true if the line was successfully closed, false otherwise.
      */
      bool close() {

        if (isOpen()) {
          int ret = ::close (m_req.fd);
          m_req.fd = -1;
          return ret == 0;
        }
        return false;
      }

      /**
         @brief Gets the values of the GPIO line(s).

         The values returned are logical, indicating if the line is active or inactive.
         The #GPIO_V2_LINE_FLAG_ACTIVE_LOW flag controls the mapping between physical values (high/low)
         and logical values (active/inactive). If #GPIO_V2_LINE_FLAG_ACTIVE_LOW is not set then high is
         active and low is inactive. If #GPIO_V2_LINE_FLAG_ACTIVE_LOW is set then low is active and high is inactive.

         @note The values of both input and output lines may be read.
                For output lines, the value returned is driver and configuration dependent and may be either
                the output buffer (the last requested value set) or the input buffer (the actual level of the line),
                and depending on the hardware and configuration these may differ.

         @param values A reference to a LineValues object to be filled with line values.
         @return true if the values were successfully retrieved, false otherwise.
      */
      bool getValues (LineValues &values) const {

        if (isOpen()) {

          int ret = ::ioctl (m_req.fd, GPIO_V2_LINE_GET_VALUES_IOCTL, &values); // TODO: use Chip::ioCtl() instead
          return (ret >= 0);
        }
        return false;
      }

      /**
         @brief Gets the value of a specific GPIO line.

         The value returned are logical, indicating if the line is active or inactive.
         The #GPIO_V2_LINE_FLAG_ACTIVE_LOW flag controls the mapping between physical value (high/low)
         and logical value (active/inactive). If #GPIO_V2_LINE_FLAG_ACTIVE_LOW is not set then high is
         active and low is inactive. If #GPIO_V2_LINE_FLAG_ACTIVE_LOW is set then low is active and high is inactive.

         If failed to get the value, an std::system_error exception is thrown.

         @note  The values of both input and output lines may be read.
                For output lines, the value returned is driver and configuration dependent and may be either
                the output buffer (the last requested value set) or the input buffer (the actual level of the line),
                and depending on the hardware and configuration these may differ.

         @param line_no The line number (default is 0).
         @return The value of the line (true for high, false for low).
      */
      bool getValue (int line_no = 0) const {

        if (isOpen()) {
          LineValues values (0, 1ULL << line_no);

          if (getValues (values)) {

            return ( (values.bits & values.mask) != 0);
          }
          else {

            throw std::system_error (errno, std::system_category(), EXCEPTION_MSG ("Failed to get line values"));
          }
        }
        return false;
      }

      /**
         @brief Sets the value of a specific GPIO line.

          The value set are logical, indicating if the line is to be active or inactive.
          The #GPIO_V2_LINE_FLAG_ACTIVE_LOW flag controls the mapping between logical values (active/inactive)
          and physical values (high/low). If #GPIO_V2_LINE_FLAG_ACTIVE_LOW is not set then active is high and inactive is low.
          If #GPIO_V2_LINE_FLAG_ACTIVE_LOW is set then active is low and inactive is high.

          Only the values of output lines may be set. Attempting to set the value of an input line is an error (EPERM).
         @param line_no The line number.
         @param value The value to set (true for high, false for low).
         @return true if the value was successfully set, false otherwise.
      */
      bool setValue (bool value, int line_no = 0) {

        if (isOpen()) {
          uint64_t line_mask = 1ULL << line_no;
          LineValues values (value ? line_mask : 0, line_mask);

          return setValues (values);
        }
        return false;
      }

      /**
         @brief Sets the values of the GPIO line(s).

          The values set are logical, indicating if the line is to be active or inactive.
          The #GPIO_V2_LINE_FLAG_ACTIVE_LOW flag controls the mapping between logical values (active/inactive)
          and physical values (high/low). If #GPIO_V2_LINE_FLAG_ACTIVE_LOW is not set then active is high and inactive is low.
          If #GPIO_V2_LINE_FLAG_ACTIVE_LOW is set then active is low and inactive is high.

          Only the values of output lines may be set. Attempting to set the value of an input line is an error (EPERM).

         @param values A reference to a LineValues object containing the values to set.
         @return true if the values were successfully set, false otherwise.
      */
      bool setValues (const LineValues &values) {

        if (isOpen()) {

          int ret = ::ioctl (m_req.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values); // TODO: use Chip::ioCtl() instead
          // TODO: retrieve errno from the ioctl call
          return (ret >= 0);
        }
        return false;
      }

      /**
         @brief Sets the debounce period for the GPIO line.
         @param debounce_ms The debounce period in milliseconds.
         @return true if the debounce period was successfully set, false otherwise.
      */
      bool setDebounce (uint32_t debounce_ms) {

        if (debounce_ms != debounce()) {
          LineConfig &cfg = reinterpret_cast<LineConfig &> (m_req.config);
          auto &debounce = cfg.attrs[0];

          if (debounce_ms > 0) {

            debounce.attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
            debounce.attr.debounce_period_us = debounce_ms * 1000;
            debounce.mask |= 1ULL << 0;
            cfg.num_attrs = 1;
          }
          else {

            debounce.attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
            debounce.attr.debounce_period_us = 0;
            debounce.mask = 0;
            cfg.num_attrs = 0;
          }
          if (isOpen()) {

            return setConfig (config()); // TODO: Debug this, it may not work as expected
          }
        }
        return false;
      }

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
         @return true if an event was successfully received, false otherwise.
      */
      bool waitForEvent (LineEvent &event, int timeout_ms = -1) {

        if (isOpen()) {

          pollfd pfd[1];
          pfd[0].fd = m_req.fd;
          pfd[0].events = POLLIN | POLLPRI;

          int ret = ::poll (pfd, 1, timeout_ms);
          if (ret > 0) {
            return ::read (m_req.fd, &event, sizeof (event)) == sizeof (event);
          }
        }
        return false;
      }

    private:
      Chip *m_chip;
      LineRequest m_req;
  };

}
