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
#include "avrlib/parallel_io.h"
#include "avrlib/time.h"

using namespace avrlib;

TIMER_0_TICK {
  TickSystemClock();
}

int main(void) {
  Boot(true);

  ParallelPort<PortC, PARALLEL_NIBBLE_HIGH> count;
  ParallelPort<PortC, PARALLEL_NIBBLE_LOW> scroll;
  uint8_t counter = 0;

  count.set_mode(DIGITAL_OUTPUT);
  scroll.set_mode(DIGITAL_OUTPUT);

  while (1) {
    Delay(100);
    count.Write((++counter) & 15);
    scroll.Write(1 << (counter & 3));
  }
}
