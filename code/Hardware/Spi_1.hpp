#pragma once
#include <IIO_Sync.hpp>
#include <IPin.hpp>
#include <Timeout.hpp>

#include "spi.h"

inline volatile bool Spi_1_spi_1_tx_done = true;
inline volatile bool Spi_1_spi_1_rx_done = true;

class Spi_1 final : public m::ifc::IIO_Sync<Ms<uint32_t>> {
 public:
  Spi_1(m::ifc::ITime<Ms<uint32_t>> &time) : timeout_(time) {
    hspi1.TxCpltCallback = [](SPI_HandleTypeDef *hspi) {
      Spi_1_spi_1_tx_done = true;
    };
    hspi1.RxCpltCallback = [](SPI_HandleTypeDef *hspi) {
      Spi_1_spi_1_rx_done = true;
    };
  }

  bool write(std::span<uint8_t const> data, Ms<uint32_t> timeout) override {
    if (!Spi_1_spi_1_tx_done) return false;

    Spi_1_spi_1_tx_done = false;

    if (HAL_SPI_Transmit_DMA(&hspi1, data.data(), data.size()) != HAL_OK) {
      Spi_1_spi_1_tx_done = true;
      return false;
    }

    return timeout_.execWithTimeout([]() { return Spi_1_spi_1_tx_done; },
                                    timeout);
  }

  bool read(std::span<uint8_t> data, Ms<uint32_t> timeout) override {
    if (!Spi_1_spi_1_rx_done) return false;

    Spi_1_spi_1_rx_done = false;

    if (HAL_SPI_Receive_DMA(&hspi1, data.data(), data.size()) != HAL_OK) {
      Spi_1_spi_1_rx_done = true;
      return false;
    }

    return timeout_.execWithTimeout([]() { return Spi_1_spi_1_rx_done; },
                                    timeout);
  }

  uint32_t getBaudrate() override { return 21'000'000 / 8; }
  bool setBaudrate(uint32_t baud) override { return false; }

 private:
  m::Timeout<Ms<uint32_t>> timeout_;
};
