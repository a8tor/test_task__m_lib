

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
    uint16_t freq;  // Частота в Гц (100Hz - 20'000 Hz)
};


class SignalGenerator {
    public:
        SignalGenerator() : is_generating(false) {}
    
        void add(std::span<Packet> packets) {
            if (packets.size() > max_size) {
                log_step.add("Exceeded maximum size of signal array.");
                return;
            }
            signal_packets = packets;
        }
    
        void start() {
            if (is_generating) return;
            is_generating = true;
            generate_signal();
        }
    
        void stop() {
            is_generating = false;
        }
    
        bool isRunning() const {
            return is_generating;
        }
    
        size_t maxSignalSize() const {
            return max_size;
        }
    
    private:
        static constexpr size_t max_size = 10;
        std::span<Packet> signal_packets;
        std::atomic<bool> is_generating;
    
        void generate_signal() {
                for (const auto& packet : signal_packets) {
                    uint16_t period = static_cast<uint16_t>(1000000 / packet.freq) - 1; // Период в микросекундах
                    uint16_t pulse_duration = period / 2; // 50% заполнение
        
                    // Установка значений таймера
                    TIM1->ARR = period; // Установка значения автозагрузки счетчика
                    TIM1->CCR1 = pulse_duration; // Установка значения длительности импульса
                    TIM1->RCR = packet.n - 1; // Установка значения счетчика повторений
        
                    // Запуск таймера
                    TIM1->CR1 |= TIM_CR1_CEN;
                }
                is_generating = false; // Завершение генерации
        }

    };

    }  // namespace m::ifc

 
 #endif  // ISTEP_H