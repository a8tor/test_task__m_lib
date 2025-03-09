#pragma once
#include <Celsius.hpp>
#include <ITempSense.hpp>
#include <optional>

class ITempSenseWithError
    : public m::ifc::ITempSense<std::optional<Celsius<float>>>,
      public m::ifc::ITempSenseError {
 public:
  virtual std::optional<Celsius<float>> value() = 0;

  virtual bool shorted() = 0;

  virtual bool broken() = 0;
};