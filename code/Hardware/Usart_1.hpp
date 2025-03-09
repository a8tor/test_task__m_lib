#pragma once
#include <IIO_Async.hpp>
#include <IPin.hpp>

#include "usart.h"

class Usart_1 final : public m::ifc::IIO_Async {
 public:
  Usart_1(m::ifc::mcu::IPin& dr_en) : dr_en_(dr_en) {}

  uint32_t bytesToWrite() override { return huart1.hdmatx->Instance->NDTR; }

  bool writeAsync(std::span<uint8_t const> data) override {
    dr_en_.write(1);
    auto res = (HAL_UART_Transmit_DMA(&huart1, (uint8_t*)data.data(),
                                      data.size()) == HAL_OK);
    if (res) {
      dma_tx_started_ = true;
    }
    return res;
  }

  bool abortWrite() override {
    if (!dma_tx_started_) return true;
    auto res = (HAL_UART_AbortTransmit(&huart1) == HAL_OK);
    if (res) {
      dma_tx_started_ = false;
    }

    return res;
  }

  bool writeDone() override {
    if (dma_tx_started_) {
      if (bytesToWrite() == 0) {
        if (HAL_UART_GetState(&huart1) == HAL_UART_STATE_READY) {
          dma_tx_started_ = false;
          return true;
        } else {
          return false;
        }
      }
      return false;
    }
    return true;
  }

  uint32_t bytesAvailable() override {
    return rx_size_ - huart1.hdmarx->Instance->NDTR;
  }

  bool readAsync(std::span<uint8_t> data) override {
    dr_en_.write(0);
    bool res = (HAL_UART_Receive_DMA(&huart1, (uint8_t*)data.data(),
                                     data.size()) == HAL_OK);
    if (res) {
      dma_rx_started_ = true;
      rx_size_ = data.size();
    }
    return res;
  }

  bool abortRead() override {
    if (!dma_rx_started_) return true;
    auto res = (HAL_UART_AbortReceive(&huart1) == HAL_OK);
    if (res) {
      dma_rx_started_ = false;
    }

    return res;
  }

  bool readDone() override {
    if (dma_rx_started_) {
      if (bytesAvailable() == rx_size_) {
        if (abortRead()) {
          if (HAL_UART_GetState(&huart1) == HAL_UART_STATE_READY) {
            return true;
          }
        }
      }
      return false;
    }
    return true;
  }

  uint32_t getBaudrate() override { return 921'600 / 10; }

  bool setBaudrate(uint32_t baud) override { return false; }

  bool error() override {
    return HAL_UART_GetError(&huart1) != HAL_UART_ERROR_NONE;
  }

 private:
  m::ifc::mcu::IPin& dr_en_;

  bool dma_tx_started_ = false;
  bool dma_rx_started_ = false;
  uint32_t rx_size_ = 0;
};
