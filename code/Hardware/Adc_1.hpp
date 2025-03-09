#pragma once
#include <IAdcDmaCircularReader.hpp>

#include "adc.h"
#include "tim.h"

inline const std::function<void(std::span<volatile uint16_t>)>
    *Adc_1_half_conv_cb_ = nullptr;
inline const std::function<void(std::span<volatile uint16_t>)> *Adc_1_conv_cb_ =
    nullptr;

inline std::span<volatile uint16_t> Adc_1_data_;

class Adc_1 final : public m::ifc::mcu::IAdcDmaCircularReader<uint16_t> {
 public:
  Adc_1() {
    Adc_1_half_conv_cb_ = &first_half_cb_;
    Adc_1_conv_cb_ = &second_half_cb_;
    hadc1.ConvHalfCpltCallback = [](ADC_HandleTypeDef *hadc) {
      if (*Adc_1_half_conv_cb_)
        (*Adc_1_half_conv_cb_)(Adc_1_data_.first(Adc_1_data_.size() / 2));
    };
    hadc1.ConvCpltCallback = [](ADC_HandleTypeDef *hadc) {
      if (*Adc_1_conv_cb_)
        (*Adc_1_conv_cb_)(
            Adc_1_data_.last(Adc_1_data_.size() - Adc_1_data_.size() / 2));
    };
  }

  void setHalfConversionCallback(
      std::function<void(std::span<volatile type>)> &&first_half_cb) override {
    first_half_cb_ = std::move(first_half_cb);
  }

  void setFullConversionCallback(
      std::function<void(std::span<volatile type>)> &&second_half_cb) override {
    second_half_cb_ = std::move(second_half_cb);
  }

  bool start(std::span<volatile uint16_t> data) override {
    if (running_) return false;

    Adc_1_data_ = data;

    if (!HAL_ADC_Start_DMA(
            &hadc1,
            reinterpret_cast<uint32_t *>(const_cast<uint16_t *>(data.data())),
            data.size()) == HAL_OK)
      return false;
    if (!HAL_TIM_Base_Start(&htim8) == HAL_OK) return false;

    running_ = true;

    return true;
  }

  bool running() override { return running_; }

  bool stop() override {
    if (!running_) return false;
    if (!HAL_ADC_Stop_DMA(&hadc1) == HAL_OK) return false;
    if (!HAL_TIM_Base_Stop(&htim8) == HAL_OK) return false;

    running_ = false;

    return true;
  }

 private:
  bool running_ = false;

  std::function<void(std::span<volatile type>)> first_half_cb_;
  std::function<void(std::span<volatile type>)> second_half_cb_;
};
