#pragma once
#include <IIO_Sync.hpp>

#include "i2c.h"

class I2c_1 final : public m::ifc::IIO_Sync<Ms<uint32_t>> {
 public:
  bool write(std::span<uint8_t const> data, Ms<uint32_t> timeout) override {
    if (data.size() < 2) return false;
    return (HAL_I2C_Master_Transmit(
                &hi2c1, data[0], (uint8_t*)data.subspan(1).data(),
                data.size() - 1, timeout.value()) == HAL_OK);
  }

  bool read(std::span<uint8_t> data, Ms<uint32_t> timeout) override {
    if (data.size() < 2) return false;
    return (HAL_I2C_Master_Receive(&hi2c1, data[0], data.subspan(1).data(),
                                   data.size() - 1, timeout.value()) == HAL_OK);
  }

  uint32_t getBaudrate() override { return 100'000 / 8; }
  bool setBaudrate(uint32_t baud) override { return false; }
};
