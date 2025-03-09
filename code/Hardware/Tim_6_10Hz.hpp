#pragma once
#include <IIt.hpp>

#include "tim.h"

inline const std::function<void()>* Tim_6_10Hz_cb_ = nullptr;

class Tim_6_10Hz final : public m::ifc::mcu::IIt {
 public:
  Tim_6_10Hz() {
    Tim_6_10Hz_cb_ = &cb_;
    htim6.PeriodElapsedCallback = [](TIM_HandleTypeDef* htim) {
      if (*Tim_6_10Hz_cb_) (*Tim_6_10Hz_cb_)();
    };
  }

  void setCallback(std::function<void()>&& cb) override { cb_ = std::move(cb); }

  bool start() override {
    if (running_) return false;
    if (HAL_TIM_Base_Start_IT(&htim6) == HAL_OK) {
      running_ = true;
      return true;
    } else {
      return false;
    }
  }

  bool running() override { return running_; }

  bool stop() override {
    if (!running_) return false;
    if (HAL_TIM_Base_Stop_IT(&htim6) == HAL_OK) {
      running_ = false;
      return true;
    } else {
      return false;
    }
  }

 private:
  bool running_ = false;
  std::function<void()> cb_;
};
