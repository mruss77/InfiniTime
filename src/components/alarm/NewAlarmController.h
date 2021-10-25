/*  Copyright (C) 2021 mruss77, Florian

    This file is part of InfiniTime.

    InfiniTime is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    InfiniTime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <cstdint>
#include "app_timer.h"
#include "components/datetime/DateTimeController.h"
#include <vector>
#include <FreeRTOS.h>
#include <timers.h>

namespace Pinetime {
  namespace System {
    class SystemTask;
  }
  namespace Controllers {
    class NewAlarmController {
    public:
      NewAlarmController(Controllers::DateTime& dateTimeController, Controllers::Settings& settingsController);
      void Init(System::SystemTask* systemTask);

      enum class AlarmMode { Off, Once, Repeat };

      struct Alarm {
        AlarmMode Mode;
        uint8_t Hour;
        uint8_t Minute;
        bool RepeatDays[7];
      };

      Alarm GetAlarm(uint8_t index);
      void SetAlarmTime(uint8_t index, uint8_t alarmHr, uint8_t alarmMin);
      void SetAlarmMode(uint8_t index, AlarmMode mode);
      void AddAlarm();
      void DeleteAlarm(uint8_t index);
      uint8_t AlarmCount();
      bool TwelveHourFormat() const;

    private:
      Controllers::DateTime& dateTimeController;
      System::SystemTask* systemTask = nullptr;
      std::vector<Alarm> alarms;
      const uint8_t defaultAlarmHour = 7;
      const uint8_t maxAlarms = 8;
      TimerHandle_t timer;
    };
  }
}
