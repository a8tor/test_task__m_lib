#pragma once
#include <B57861S0103F045.hpp>
#include <optional>

#include "ITempSenseWithError.hpp"

class Ntc final : public ITempSenseWithError {
 public:
  Ntc(volatile const uint16_t &data) : data_(data) {}

  std::optional<Celsius<float>> value() override {
    if (data_ < Short_Threshold || data_ > Break_Threshold) {
      return std::nullopt;
    }

    float res = -(data_ / (data_ - 4095.0f));
    float temp = bc_.resToTemp(res);  // TODO: check NTC type
    return Celsius<float>{temp};
  }

  bool shorted() override { return data_ < Short_Threshold; }

  bool broken() override { return data_ > Break_Threshold; }

  std::optional<Celsius<float>> min() override { return min_; }

  std::optional<Celsius<float>> max() override { return max_; }

 private:
  volatile const uint16_t &data_;
  m::ic::B57861S0103F045 bc_;

  const uint32_t Short_Threshold = 5;  // TODO: check range
  const uint32_t Break_Threshold = 4090;

  Celsius<float> min_ = -20_C;
  Celsius<float> max_ = 130_C;
};
