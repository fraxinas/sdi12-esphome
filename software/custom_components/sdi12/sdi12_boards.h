/**
 * =========================== ESPHome adaptation =============================
 *
 * sdi12_boards.h
 * (CC) 2023 Andreas Frisch <github@fraxinas.dev>
 *
 * ======================== Attribution & License =============================
 *
 * @file SDI12_boards.cpp
 * @copyright (c) 2013-2020 Stroud Water Research Center (SWRC)
 *                          and the EnviroDIY Development Team
 * @author Sara Geleskie Damiano (sdamiano@stroudcenter.org)
 *
 * @brief This file implements the setting and unsetting of the proper prescalers for
 * the timers for SDI-12.
 *
 */

/* ======================== Arduino SDI-12 =================================
An Arduino library for SDI-12 communication with a wide variety of environmental
sensors. This library provides a general software solution, without requiring
   ======================== Arduino SDI-12 =================================*/

#pragma once

#ifdef USE_ARDUINO
#include <Arduino.h>

namespace esphome {
namespace sdi12 {

#define LIB_DEBUG     0

#if LIB_DEBUG > 0
  #define LIB_LOG(...)                    \
    {                                     \
      Serial.printf("<%s>",__FUNCTION__); \
      Serial.printf("<%d>",__LINE__);     \
      Serial.printf(__VA_ARGS__);         \
      Serial.printf("\n");                \
      delay(10);                          \
    }
#else
  #define LIB_LOG(...)
#endif

/** The interger type (size) of the timer return value */
typedef uint32_t sdi12timer_t;

/**
 * @brief The class used to define the processor timer for the SDI-12 serial emulation.
 */
class SDI12Timer {
 public:
  /**
   * @brief Construct a new SDI12Timer
   */
  SDI12Timer() {};
  /**
   * @brief Set the processor timer prescaler such that the 10 bits of an SDI-12
   * character are divided into the rollover time of the timer.
   *
   * @note  The ESP32 and ESP8266 are fast enough processors that they can take the
   * time to read the core 'micros()' function still complete the other processing
   * needed on the serial bits.  All of the other processors using the Arduino core also
   * have the micros function, but the rest are not fast enough to waste the processor
   * cycles to use the micros function and must manually configure the processor timer
   * and use the faster assembly macros to read that processor timer directly.
   */
  void configSDI12TimerPrescale(void) {};
  /**
   * @brief Reset the processor timer prescaler to whatever it was prior to being
   * adjusted for this library.
   *
   * @note The prescaler is *NOT* set back to initial values for SAMD boards!  On those
   * processors, generic clock generator 4 will remain configured for SDI-12 until it is
   * reset outside of this library.
   */
  void resetSDI12TimerPrescale(void) {};

// Espressif ESP32/ESP8266 boards
//
#if defined(USE_ESP32) || defined(USE_ESP8266)


  /**
   * @brief Read the processor micros and right shift 6 bits (ie, divide by 64) to get a
   * 64µs tick.
   *
   * @note  The ESP32 and ESP8266 are fast enough processors that they can take the time
   * to read the core 'micros()' function still complete the other processing needed on
   * the serial bits.  All of the other processors using the Arduino core also have the
   * micros function, but the rest are not fast enough to waste the processor cycles to
   * use the micros function and must use the faster assembly macros to read the
   * processor timer directly.
   *
   * @return **sdi12timer_t** The current processor micros
   */
    sdi12timer_t SDI12TimerRead(void) {
    // Its a one microsecond clock but we want 64uS ticks so divide by 64 i.e. right shift
    // 6
    return ((sdi12timer_t)(micros() >> 6));
    }

/**
 * @brief The number of "ticks" of the timer that occur within the timing of one bit
 * at the SDI-12 baud rate of 1200 bits/second.
 *
 * 48MHz / 3 pre-prescaler = 16MHz
 * 16MHz / 1024 prescaler = 15624 'ticks'/sec = 64 µs / 'tick'
 * (1 sec/1200 bits) * (1 tick/64 µs) = 13.0208 ticks/bit
 */
#define TICKS_PER_BIT 13
/**
 * @brief The number of "ticks" of the timer per SDI-12 bit, shifted by 2^10.
 *
 * 1/(13.0208 ticks/bit) * 2^10 = 78.6432
 */
#define BITS_PER_TICK_Q10 79
/**
 * @brief A "fudge factor" to get the Rx to work well.   It mostly works to ensure that
 * uneven tick increments get rounded up.
 *
 * @see https://github.com/SlashDevin/NeoSWSerial/pull/13
 */
#define RX_WINDOW_FUDGE 2

// Raspberry Pi Pico
//
#elif defined(USE_RP2040)
  /**
   * @brief Read the processor micros and right shift 6 bits (ie, divide by 64) to get a
   * 64µs tick.
   *
   * @note  The ESP32 and ESP8266 are fast enough processors that they can take the time
   * to read the core 'micros()' function still complete the other processing needed on
   * the serial bits.  All of the other processors using the Arduino core also have the
   * micros function, but the rest are not fast enough to waste the processor cycles to
   * use the micros function and must use the faster assembly macros to read the
   * processor timer directly.
   *
   * @return **sdi12timer_t** The current processor micros
   */
    sdi12timer_t SDI12TimerRead(void) {
    // Its a one microsecond clock but we want 64uS ticks so divide by 64 i.e. right shift
    // 6
    return ((sdi12timer_t)(micros() >> 6));
    }

/**
 * @brief The number of "ticks" of the timer that occur within the timing of one bit
 * at the SDI-12 baud rate of 1200 bits/second.
 *
 * 48MHz / 3 pre-prescaler = 16MHz
 * 16MHz / 1024 prescaler = 15624 'ticks'/sec = 64 µs / 'tick'
 * (1 sec/1200 bits) * (1 tick/64 µs) = 13.0208 ticks/bit
 */
#define TICKS_PER_BIT 13
/**
 * @brief The number of "ticks" of the timer per SDI-12 bit, shifted by 2^10.
 *
 * 1/(13.0208 ticks/bit) * 2^10 = 78.6432
 */
#define BITS_PER_TICK_Q10 79
/**
 * @brief A "fudge factor" to get the Rx to work well.   It mostly works to ensure that
 * uneven tick increments get rounded up.
 *
 * @see https://github.com/SlashDevin/NeoSWSerial/pull/13
 */
#define RX_WINDOW_FUDGE 2

#else
#error "Please unsupported board for SDI-12"
#endif
#endif // USE_ARDUINO
};

}  // namespace sdi12
}  // namespace esphome
