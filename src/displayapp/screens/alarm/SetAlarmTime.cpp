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
#include "SetAlarmTime.h"
#include "Screen.h"
#include "Symbols.h"

using namespace Pinetime::Applications::Screens;
using Pinetime::Controllers::AlarmController;

static void alarmEventHandler(lv_obj_t* obj, lv_event_t event) {
  Alarm* screen = static_cast<Alarm*>(obj->user_data);
  screen->handleEvent(obj, event);
}

SetAlarmTime::SetAlarmTime(DisplayApp* app, Controllers::AlarmController& alarmController)
  : Screen(app), running {true}, alarmController {alarmController} {

   lv_obj_t * colon = lv_label_create(lv_scr_act(), nullptr);
   lv_obj_set_style_local_text_font(colon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_76);
   lv_obj_set_style_local_text_color(colon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
   lv_label_set_text_static(colon, ":");
   lv_obj_align(colon, lv_scr_act(), LV_ALIGN_CENTER, 0, -40);

   btnHours = lv_btn_create(lv_scr_act(), nullptr);
   btnHours->user_data = this;
   lv_obj_set_event_cb(btnHours, alarmEventHandler);
   lv_obj_set_size(btnHours, 100, 120);
   txtHours = lv_label_create(btnHours, nullptr);
   lv_obj_set_style_local_text_font(txtHours, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_76);
   lv_obj_set_style_local_text_color(txtHours, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
   lv_label_set_text_fmt(txtHours, "%02d", alarmHours);
   lv_obj_align(btnHours, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 5, -40);

   btnMin = lv_btn_create(lv_scr_act(), btnHours);
   btnMin->user_data = this;
   txtMin = lv_label_create(btnMin, txtHours);
   lv_label_set_text_fmt(txtMin, "%02d", alarmMinutes);
   lv_obj_align(btnMin, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, -5, -40);

   trackpad = lv_slider_create(lv_scr_act(), nullptr);
   trackpad->user_data = this;
   lv_obj_set_size(trackpad, 220, 20);
   lv_obj_align(trackpad, nullptr, LV_ALIGN_CENTER, 0, 80);
   lv_obj_set_event_cb(trackpad, alarmEventHandler);
   lv_slider_set_anim_time(trackpad, -1);
   lv_slider_set_range(trackpad, 0, 19);
   
   lv_obj_set_style_local_bg_color(trackpad, LV_SLIDER_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLUE);
   lv_obj_set_style_local_bg_color(trackpad, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE);
   lv_obj_set_style_local_bg_color(trackpad, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_COLOR_BLUE);;
   lv_obj_set_style_local_pad_all(trackpad, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, 0);
   lv_obj_set_style_local_border_color(trackpad, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_COLOR_BLUE);
   lv_obj_set_style_local_border_width(trackpad, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, 0);
   lv_obj_set_style_local_pad_all(trackpad, LV_SLIDER_PART_KNOB, LV_STATE_PRESSED, 0);


   btnAmPm = lv_btn_create(lv_scr_act(), nullptr);
   btnAmPm->user_data = this;
   lv_obj_set_size(btnAmPm, 115,50);
   lv_obj_set_event_cb(btnAmPm, alarmEventHandler);
   txtAmPm = lv_label_create(btnAmPm, nullptr);
   lv_obj_align(btnAmPm, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
   lv_obj_set_hidden(btnAmPm, true);

   if (twelveHr) {
   t12Hour = twentyFourToTwelve(alarmHours);
   }
   setHour();

}

void SetAlarmTime::setHour() {
   lv_obj_set_style_local_bg_color(btnHours, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLUE);
   lv_obj_set_style_local_bg_color(btnMin, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
   if (twelveHr) {
      if (t12Hour.IsPm) {
         lv_label_set_text(txtAmPm, "PM");
      } else {
         lv_label_set_text(txtAmPm, "AM");
      }
   }
   settingHour = true;
}
void SetAlarmTime::setMin() {
   lv_obj_set_style_local_bg_color(btnHours, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
   lv_obj_set_style_local_bg_color(btnMin, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLUE);
   settingHour = false;
}

int16_t SetAlarmTime::twelveToTwentyFour(TwelveHourHr t12Hour) {
   if (!t12Hour.IsPm && t12Hour.Hour == 12) {
      return 0;
   }
   int16_t t24Hour = t12Hour.Hour;
   if (t12Hour.IsPm) {
      t24Hour += 12;
   }
   return t24Hour;
}

struct SetAlarmTime::TwelveHourHr Alarm::twentyFourToTwelve(int16_t t24Hour) {
   TwelveHourHr t12Hour;
   if (t24Hour < 12) {
      t12Hour.Hour = (t24Hour == 0) ? 12 : t24Hour;
      t12Hour.IsPm = false;
   } else {
      t12Hour.Hour = (t24Hour == 12) ? 12 : t24Hour + 12;
      t12Hour.IsPm = true;
   }
   return t12Hour;
}

SetAlarmTime::~SetAlarmTime() {
  lv_obj_clean(lv_scr_act());
}

void SetAlarmTime::handleEvent(lv_obj_t * obj, lv_event_t event) {
   if (obj == trackpad && event == LV_EVENT_PRESSING) {
      if (lastPadVal == -1) {
         lastPadVal = lv_slider_get_value(trackpad);
         return;
      }
      int16_t curVal = lv_slider_get_value(trackpad);
      int16_t diff = curVal - lastPadVal;
      lastPadVal = curVal;
      if (settingHour) {
         if (twelveHr) {
            t12Hour.Hour += diff;
            if (t12Hour.Hour > 12) {
               t12Hour.Hour -= 12;
            } else if (t12Hour.Hour < 1) {
               t12Hour.Hour += 12;
            }
            alarmHours = twelveToTwentyFour(t12Hour);
            lv_label_set_text_fmt(txtHours, "%02d", t12Hour.Hour);
         } else {
            alarmHours += diff;
            if (alarmHours > 23) {
               alarmHours -= 24;
            } else if (alarmHours < 0) {
               alarmHours += 24;
            }
            lv_label_set_text_fmt(txtHours, "%02d", alarmHours);
         }
      } else {
         alarmMinutes += diff;
         if (alarmMinutes < 0) {
            alarmMinutes += 60;
         } else if (alarmMinutes > 59) {
            alarmMinutes -= 60;
         }
         lv_label_set_text_fmt(txtMin, "%02d", alarmMinutes);
      }
   } else if (obj == trackpad && (event == LV_EVENT_RELEASED || event == LV_EVENT_PRESS_LOST)) {
      lastPadVal = -1;
   } else if (obj == btnHours && event == LV_EVENT_CLICKED) {
      setHour();
   } else if (obj == btnMin && event == LV_EVENT_CLICKED) {
      setMin();
   } else if (obj == btnAmPm && event == LV_EVENT_CLICKED) {
      if (t12Hour.IsPm) {
         lv_label_set_text(txtAmPm, "AM");
         t12Hour.IsPm = false;
      } else {
         lv_label_set_text(txtAmPm, "PM");
         t12Hour.IsPm = true;
      }
      alarmHours = twelveToTwentyFour(t12Hour);
   }
}

void SetAlarmTime::UpdateAlarmTime() {
  lv_label_set_text_fmt(time, "%02d:%02d", alarmHours, alarmMinutes);
  alarmController.SetAlarmTime(alarmHours, alarmMinutes);
}

void SetAlarmTime::SetAlerting() {
  SetEnableButtonState();
}

void SetAlarmTime::SetEnableButtonState() {
  switch (alarmController.State()) {
    case AlarmController::AlarmState::Set:
      lv_label_set_text(txtEnable, "ON");
      lv_obj_set_style_local_bg_color(btnEnable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
      break;
    case AlarmController::AlarmState::Not_Set:
      lv_label_set_text(txtEnable, "OFF");
      lv_obj_set_style_local_bg_color(btnEnable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
      break;
    case AlarmController::AlarmState::Alerting:
      lv_label_set_text(txtEnable, Symbols::stop);
      lv_obj_set_style_local_bg_color(btnEnable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
  }
}

void SetAlarmTime::ShowInfo() {
  btnMessage = lv_btn_create(lv_scr_act(), nullptr);
  btnMessage->user_data = this;
  lv_obj_set_event_cb(btnMessage, alarmEventHandler);
  lv_obj_set_height(btnMessage, 200);
  lv_obj_set_width(btnMessage, 150);
  lv_obj_align(btnMessage, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
  txtMessage = lv_label_create(btnMessage, nullptr);
  lv_obj_set_style_local_bg_color(btnMessage, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLUE);

  if (alarmController.State() == AlarmController::AlarmState::Set) {
    auto timeToAlarm = alarmController.SecondsToAlarm();

    auto daysToAlarm = timeToAlarm / 86400;
    auto hrsToAlarm = (timeToAlarm % 86400) / 3600;
    auto minToAlarm = (timeToAlarm % 3600) / 60;
    auto secToAlarm = timeToAlarm % 60;

    lv_label_set_text_fmt(
      txtMessage, "Time to\nalarm:\n%2d Days\n%2d Hours\n%2d Minutes\n%2d Seconds", daysToAlarm, hrsToAlarm, minToAlarm, secToAlarm);
  } else {
    lv_label_set_text(txtMessage, "Alarm\nis not\nset.");
  }
}

void SetAlarmTime::SetRecurButtonState() {
  using Pinetime::Controllers::AlarmController;
  switch (alarmController.Recurrence()) {
    case AlarmController::RecurType::None:
      lv_label_set_text(txtRecur, "ONCE");
      break;
    case AlarmController::RecurType::Daily:
      lv_label_set_text(txtRecur, "DAILY");
      break;
    case AlarmController::RecurType::Weekdays:
      lv_label_set_text(txtRecur, "MON-FRI");
  }
}

void SetAlarmTime::ToggleRecurrence() {
  using Pinetime::Controllers::AlarmController;
  switch (alarmController.Recurrence()) {
    case AlarmController::RecurType::None:
      alarmController.SetRecurrence(AlarmController::RecurType::Daily);
      break;
    case AlarmController::RecurType::Daily:
      alarmController.SetRecurrence(AlarmController::RecurType::Weekdays);
      break;
    case AlarmController::RecurType::Weekdays:
      alarmController.SetRecurrence(AlarmController::RecurType::None);
  }
  SetRecurButtonState();
}
