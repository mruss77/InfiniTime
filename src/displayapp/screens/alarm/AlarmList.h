/*  Copyright (C) 2021 mruss77

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

#include "../Screen.h"
#include "systemtask/SystemTask.h"
#include "../LittleVgl.h"
#include "components/alarm/NewAlarmController.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class AlarmList : public Screen {
      public:
        AlarmList(DisplayApp* app, Controllers::NewAlarmController& alarmController);

      private:
        void HandleEvent(lv_obj_t * obj, lv_event_t event);
        bool running;
        Controllers::NewAlarmController& alarmController;

        lv_obj_t * rows[4];
        lv_obj_t * timeButtons[4];
        lv_obj_t * timeLabels[4];
        lv_obj_t * repButtons[4];
        lv_obj_t * repLabels[4];

        uint8_t page;
      };
    };
  };
}
