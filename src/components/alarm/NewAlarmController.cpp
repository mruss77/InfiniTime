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
#include "NewAlarmController.h"
#include "systemtask/SystemTask.h"
#include "app_timer.h"
#include "task.h"
#include <chrono>

using namespace Pinetime::Controllers;
using namespace std::chrono_literals;

NewAlarmController::NewAlarmController(Controllers::DateTime& dateTimeController, Controllers::Settings& settingsController) :
    dateTimeController {dateTimeController}, settingsController {settingsController}
}

void AlarmTimerCallback(TimerHandle_t xTimer) {
  auto* controller = static_cast<NewAlarmController*>(pvTimerGetTimerId(xTimer));
  controller->CheckAlarms();
}

void NewAlarmController::Init(System::SystemTask* systemTask) {
  //TODO: Read alarms from settings
  alarms.push_back(Alarm{AlarmMode.Repeat, 7, 30, { true, true, false, false, false, false, false }});
  alarms.push_back(Alarm{AlarmMode.Once, 8, 45});

  //TODO: create callback
  timer = xTimerCreate("AlarmTimer", pdMS_TO_TICKS(60000), pdTRUE, this, AlarmTimerCallback);
  this->systemTask = systemTask;
}

void NewAlarmController::SetAlarmTime(uint8_t index, uint8_t hour, uint8_t minute) {
  if alarms.size() > index {
    alarms[index].Hour = hour;
    alarms[index].Minute = minute;
  }
}

Alarm NewAlarmController::GetAlarm(uint8_t index) {
  if alarms.size() > index {
    return alarms[index];
  }
  return nullptr;
}

void NewAlarmController::SetAlarmMode(uint8_t index, AlarmMode mode) {
  if alarms.size() > index {
    alarms[index].Mode = mode;
  }
}

void NewAlarmController::AddAlarm() {
  if alarms.size() > maxAlarms {
    alarms.push_back(Alarm{AlarmMode.Off, defaultAlarmHour});
  }
}

void NewAlarmController::DeleteAlarm(uint8_t index) {
  if alarms.size() > index {
    alarms.erase(index);
  }
}

uint8_t NewAlarmController::AlarmCount() {
  return alarms.size();
}


uint32_t NewAlarmController::SecondsToAlarm(uint8_t index) {
  if alarms.size() > index {
    return std::chrono::duration_cast<std::chrono::seconds>(alarmTime - dateTimeController.CurrentDateTime()).count();
  }
}

//TODO: handle actually setting off the alarm...

void NewAlarmController::DisableAlarm() {
  app_timer_stop(alarmAppTimer);
  state = AlarmState::Not_Set;
}

void NewAlarmController::SetOffAlarmNow() {
  state = AlarmState::Alerting;
  systemTask->PushMessage(System::Messages::SetOffAlarm);
}

void NewAlarmController::StopAlerting() {
  systemTask->PushMessage(System::Messages::StopRinging);

  // Alarm state is off unless this is a recurring alarm
  if (recurrence == RecurType::None) {
    state = AlarmState::Not_Set;
  } else {
    state = AlarmState::Set;
    // set next instance
    ScheduleAlarm();
  }
}

Settings::ClockType newAlarmController::TwelveHourFormat() {
  return settingsController.GetClockType();
}
