// Copyright 2010 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "avrlib/boot.h"
#include "avrlib/deprecated/devices/output_array.h"
#include "avrlib/deprecated/devices/switch_array.h"
#include "avrlib/serial.h"
#include "avrlib/time.h"
#include "avrlib/output_stream.h"

using namespace avrlib;

typedef Serial<SerialPort0, 9600, DISABLED, POLLED> Debug;
OutputStream<Debug> debug_output;

TIMER_0_TICK {
  TickSystemClock();
}

// Shift registers / muxes.
typedef Gpio<PortB, 7> IOClockLine;
typedef Gpio<PortB, 6> IOInputLine;
typedef Gpio<PortB, 5> IOOutputLine;
typedef Gpio<PortD, 2> IOEnableLine;

int main(void) {
  Boot(true);

  OutputArray<
      IOEnableLine,
      IOClockLine,
      IOOutputLine,
      8,
      4,
      MSB_FIRST,
      false> leds;

  SwitchArray<
      IOEnableLine,
      IOClockLine,
      IOInputLine,
      6> switches;

  uint8_t current_led = 0;
  uint8_t intensity = 15;
  uint8_t divide = 0;

  Debug::Init();
  leds.Init();
  switches.Init();
  uint8_t full = 0;
  while (1) {
    ++divide;
    if ((divide & 3) == 0) {
      switches.Read();
      if (switches.released()) {
        KeyEvent released = switches.key_event();
        debug_output << "id: " << int(released.id);
        debug_output << " hold time:" << int(released.hold_time);
        debug_output << " shifted: " << int(released.shifted) << endl;
        switch (released.id) {
          case 5:
            if (current_led > 0) {
              --current_led;
            }
            break;

          case 4:
            if (current_led < 7) {
              ++current_led;
            }
            break;

          case 3:
            if (intensity > 0) {
              --intensity;
            }
            break;

          case 2:
            if (intensity < 15) {
              ++intensity;
            }
            break;

          case 1:
            intensity = 15;
            current_led = 0;
            full = 0;
            break;

          case 0:
            full = ~full;
            break;
        }
      }
    }
    leds.Clear();
    for (uint8_t i = 0; i < 8; ++i) {
      leds.set_value(i, (i == current_led || full) ? intensity : 0);
    }
    leds.Write();
  }
}
