#include <libs/lvgl/lvgl.h>
#include "StepCount.h"
#include <components/stepcount/StepCountController.h>

#include "../DisplayApp.h"

using namespace Pinetime::Applications::Screens;
extern lv_font_t jetbrains_mono_extrabold_compressed;
extern lv_font_t jetbrains_mono_bold_20;

namespace {
  const char *ToString(Pinetime::Controllers::StepCountController::States s) {
    switch (s) {
      case Pinetime::Controllers::StepCountController::States::NotEnoughData:
        return "Not enough data,\nplease wait...";
      case Pinetime::Controllers::StepCountController::States::NoTouch:
        return "No touch detected";
      case Pinetime::Controllers::StepCountController::States::Running:
        return "Measuring...";
      case Pinetime::Controllers::StepCountController::States::Stopped:
        return "Stopped";
    }
    return "";
  }

  static void btnStartStopEventHandler(lv_obj_t *obj, lv_event_t event) {
    StepCount *screen = static_cast<StepCount *>(obj->user_data);
    screen->OnStartStopEvent(event);
  }
}

StepCount::StepCount(Pinetime::Applications::DisplayApp *app, Controllers::StepCountController& stepCountController) : Screen(app), stepCountController{stepCountController} {
  label_bpm = lv_label_create(lv_scr_act(), NULL);

  labelStyle = const_cast<lv_style_t *>(lv_label_get_style(label_bpm, LV_LABEL_STYLE_MAIN));
  labelStyle->text.font = &jetbrains_mono_bold_20;

  lv_style_copy(&labelBigStyle, labelStyle);
  labelBigStyle.text.font = &jetbrains_mono_extrabold_compressed;

  lv_label_set_style(label_bpm, LV_LABEL_STYLE_MAIN, labelStyle);

  label_hr = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_style(label_hr, LV_LABEL_STYLE_MAIN, &labelBigStyle);
  lv_obj_align(label_hr, lv_scr_act(), LV_ALIGN_CENTER, -70, -40);
  lv_label_set_text(label_hr, "000");

  lv_label_set_text(label_bpm, "Step count");
  lv_obj_align(label_bpm, label_hr, LV_ALIGN_OUT_TOP_MID, 0, -20);


  label_status = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label_status, ToString(Pinetime::Controllers::StepCountController::States::NotEnoughData));
  lv_label_set_style(label_status, LV_LABEL_STYLE_MAIN, labelStyle);
  lv_obj_align(label_status, label_hr, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  btn_startStop = lv_btn_create(lv_scr_act(), NULL);
  btn_startStop->user_data = this;
  lv_obj_set_height(btn_startStop, 50);
  lv_obj_set_event_cb(btn_startStop, btnStartStopEventHandler);
  lv_obj_align(btn_startStop, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  label_startStop = lv_label_create(btn_startStop, nullptr);
  UpdateStartStopButton(stepCountController.State() != Controllers::StepCountController::States::Stopped);
}

StepCount::~StepCount() {
  lv_obj_clean(lv_scr_act());
}

bool StepCount::Refresh() {
  char hr[4];

  auto state = stepCountController.State();
  switch(state) {
    case Controllers::StepCountController::States::NoTouch:
    case Controllers::StepCountController::States::NotEnoughData:
    case Controllers::StepCountController::States::Stopped:
      lv_label_set_text(label_hr, "000");
      break;
    default:
      sprintf(hr, "%03d", stepCountController.StepCount());
      lv_label_set_text(label_hr, hr);
  }

  lv_label_set_text(label_status, ToString(state));
  lv_obj_align(label_status, label_hr, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  return running;
}

bool StepCount::OnButtonPushed() {
  running = false;
  return true;
}

void StepCount::OnStartStopEvent(lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    if(stepCountController.State() == Controllers::StepCountController::States::Stopped) {
      stepCountController.Start();
      UpdateStartStopButton(stepCountController.State() != Controllers::StepCountController::States::Stopped);
    }
    else {
      stepCountController.Stop();
      UpdateStartStopButton(stepCountController.State() != Controllers::StepCountController::States::Stopped);
    }
  }
}

void StepCount::UpdateStartStopButton(bool isRunning) {
  if(isRunning)
    lv_label_set_text(label_startStop, "Stop");
  else
    lv_label_set_text(label_startStop, "Start");
}
