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
#include "AlarmList.h"
#include "Screen.h"
#include "Symbols.h"

using namespace Pinetime::Applications::Screens;
using Pinetime::Controllers::AlarmController;

static void AlarmListHandler(lv_obj_t* obj, lv_event_t event) {
  AlarmList* screen = static_cast<AlarmList*>(obj->user_data);
  screen->HandleEvent(obj, event);
}

AlarmList::AlarmList(DisplayApp* app, Controllers::AlarmController& alarmController)
  : Screen(app), running {true}, alarmController {alarmController} {

   // Container for alarm list, which takes up the entire screen below the status bar
   alarmCont = lv_cont_create(lv_scr_act(), nullptr);
   lv_obj_set_size(alarmCont, 240, 210);
   lv_obj_align(alarmCont, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
   lv_obj_set_style_local_pad_left(alarmCont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
   lv_obj_set_style_local_pad_right(alarmCont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
   lv_obj_set_style_local_pad_top(alarmCont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
   lv_obj_set_style_local_pad_bottom(alarmCont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
   lv_obj_set_style_local_pad_inner(alarmCont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
   lv_obj_set_auto_realign(alarmCont, true);
   lv_cont_set_layout(alarmCont, LV_LAYOUT_PRETTY_TOP);

   // 4 rows of buttons to show 4 alarms per page
   for (uint8_t i=0; i<4; i++) {
      rows[i] = lv_cont_create(alarmCont, nullptr);
      lv_obj_set_size(rows[i], 230, 45);
      lv_obj_set_style_local_pad_left(rows[i], LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
      lv_obj_set_style_local_pad_right(rows[i], LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
      lv_obj_set_style_local_pad_inner(rows[i], LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
      lv_obj_set_style_local_pad_top(rows[i], LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
      lv_obj_set_style_local_pad_bottom(rows[i], LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
      lv_cont_set_layout(rows[i], LV_LAYOUT_ROW_TOP);
      lv_obj_set_auto_realign(rows[i], true);

      timeButtons[i] = lv_btn_create(rows[i], nullptr);
      timbButtons[i]->user_data = this;
      lv_obj_set_event_cb(timeButtons[i], AlarmListHandler);
      lv_obj_set_size(timeButtons[i], 100, 45);
      timeLabels[i] = lv_label_create(timeButtons[i], nullptr);
      
      repButtons[i] = lv_btn_create(rows[i], nullptr);
      repButtons[i]->user_data = this;
      lv_obj_set_event_cb(repButtons[i], AlarmListHandler);
      lv_obj_set_size(repButtons[i], 125, 45);
      repLabels[i] = lv_label_create(repButtons[i], nullptr);
   }
}

void AlarmList::HandleEvent(lv_obj_t * obj, lv_event_t event) {
   if (obj == slider && event == LV_EVENT_PRESSING) {
      if (lastSliderVal == -1) {
         lastSliderVal = lv_slider_get_value(slider);
         return;
      }
      int16_t curVal = lv_slider_get_value(slider);
      int16_t diff = curVal - lastSliderVal;
      lastSliderVal = curVal;
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
   } else if (obj == btnHours) {
      setHour();
   } else if (obj == btnMin) {
      setMin();
   } else if (obj == btnAmPm && event == LV_EVENT_PRESSED) {
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

