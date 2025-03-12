#ifndef ISTEP_H
#define ISTEP_H

#include <array>
#include <span>
#include <cstdint>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <IIO_AsyncLog.hpp>
#include <Pin_F4_Wrapper.hpp>
#include "Usart_1.hpp"

PinWrapper usart_1_dir{GPIOB, Pin::PinNum::Pin_5, Pin::Mode::Output_PP};
Usart_1 usart_1{usart_1_dir};
m::IIO_AsyncLog log_step{usart_1};


namespace m::ifc {

    struct Packet {
        uint8_t n;      // Количество импульсов
        uint16_t freq;  // Частота в Гц (100 - 20'000 Гц)
    };
    
    class SignalGenerator {
    public:
        SignalGenerator() : is_generating(false) {}
    
        void add(std::span<Packet> packets) {
            if (packets.size() > max_size) {
                log_step.add("Exceeded maximum size of signal array.");
                return;
            }
            size_t index = 0;
            for (const auto& packet : packets) {
                if (index >= max_size)
                    break;
    
                uint16_t period = static_cast<uint16_t>(SystemCoreClock / packet.freq) - 1;
                uint16_t pulse_duration = period / 2;

                dma_burst_buffer[index * 3 + 0] = period;
                dma_burst_buffer[index * 3 + 1] = pulse_duration;
                dma_burst_buffer[index * 3 + 2] = packet.n - 1;
    
                ++index;
            }
            num_packets = packets.size();
        }
    
        void start() {
            if (is_generating) return;
            is_generating = true;
            setup_dma();
            DMA2_Stream1->CR |= DMA_SxCR_EN;
            TIM1->CR1 |= TIM_CR1_CEN;
        }
    
        void stop() {
            is_generating = false;

            TIM1->CR1 &= ~TIM_CR1_CEN;

            DMA2_Stream1->CR &= ~DMA_SxCR_EN;
        }
    
        bool isRunning() const {
            return is_generating;
        }
    
        size_t maxSignalSize() const {
            return max_size;
        }
    
    private:
        static constexpr size_t max_size = 10;
    
        std::atomic<bool> is_generating;
        size_t num_packets{0};

        uint16_t dma_burst_buffer[max_size * 3]{};
    
        void setup_dma() {
            DMA2_Stream1->NDTR = num_packets * 3;
            DMA2_Stream1->M0AR = reinterpret_cast<uint32_t>(dma_burst_buffer);
            DMA2_Stream1->PAR = reinterpret_cast<uint32_t>(&TIM1->DMAR);
        }
    
    };
    
    }  // namespace m::ifc


#endif  // ISTEP_H
