
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "step.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <IIO_AsyncLog.hpp>
#include <MemoryPart.hpp>
#include <MemoryTest.hpp>
#include <ModbusRtuProtocol.hpp>
#include <PY25Q128HA.hpp>
#include <Pin_F4_Wrapper.hpp>
#include <SimpleErrorTracer.hpp>
#include <Time_F4_Tim5.hpp>
#include <Units.hpp>
#include <algorithm>

#include "Adc_1.hpp"
#include "Drivers.hpp"
#include "Error.hpp"
#include "I2c_1.hpp"
#include "Machine.hpp"
#include "Regmap.hpp"
#include "Settings.hpp"
#include "Spi_1.hpp"
#include "TempSense.hpp"
#include "Tim_6_10Hz.hpp"
#include "Tim_7_1KHz.hpp"
#include "Usart_1.hpp"
#include "Usart_4.hpp"
#include "Usart_5.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#pragma GCC poison operator new
#pragma GCC poison operator delete
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  __HAL_RCC_TIM1_CLK_ENABLE();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM8_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  TimeUs time_us;
  TimeMs time_ms;

  time_ms.delay(10_Ms);

  // ##################################################
  //              Signal generator STEP
  // ##################################################

  //#include "step.h"
  m::ifc::SignalGenerator generator;
  std::array<m::ifc::Packet,3> signal{{{2, 1000},{3, 500},{2, 2000}}};
  generator.add(signal);
  generator.start();


  // ##################################################
  //              USART 1 Error Log&Tracer
  // ##################################################
  PinWrapper usart_1_dir{GPIOB, Pin::PinNum::Pin_5, Pin::Mode::Output_PP};
  Usart_1 usart_1{usart_1_dir};

  m::IIO_AsyncLog log{usart_1};
  m::SimpleErrorTracer<uint16_t, 100> tracer;

  log.add("Startup");

  // ##################################################
  //                      LED
  // ##################################################
  PinWrapper led_g{GPIOC, Pin::PinNum::Pin_6, Pin::Mode::Output_PP};
  PinWrapper led_r{GPIOC, Pin::PinNum::Pin_7, Pin::Mode::Output_PP};

  // ##################################################
  //              Temperature sensors
  // ##################################################
  static std::array<volatile uint16_t, 3 * 20> adc_data{0};
  static std::array<volatile uint16_t, 3> adc_filtered{0};
  std::function<void(std::span<volatile uint16_t>)> adc_cb =
      [&](std::span<volatile uint16_t> part) {
        for (size_t i = 0; i < 3; ++i) {
          std::array<uint16_t, 10> elements;
          for (size_t j = 0; j < elements.size(); ++j) {
            elements[j] = part[i + j * 3];
          }
          std::sort(elements.begin(), elements.end());
          adc_filtered[i] = elements[elements.size() / 2];
        }
      };

  Adc_1 adc_1;
  adc_1.setHalfConversionCallback(std::move(adc_cb));
  adc_1.setFullConversionCallback(std::move(adc_cb));

  Ntc ts_tank{adc_filtered[0]};
  Ntc ts_steam_room{adc_filtered[1]};
  Ntc ts_ntc_3{adc_filtered[2]};

  // ##################################################
  //                      MEM
  // ##################################################
  PinWrapper mem_wp{GPIOC, Pin::PinNum::Pin_4, Pin::Mode::Output_PP,
                    Pin::Inversion::Inverted};
  PinWrapper mem_hold{GPIOC, Pin::PinNum::Pin_5, Pin::Mode::Output_PP,
                      Pin::Inversion::Inverted};
  PinWrapper spi_1_cs{GPIOA, Pin::PinNum::Pin_4, Pin::Mode::Output_PP,
                      Pin::Inversion::Inverted};

  Spi_1 spi_1{time_ms};

  m::ic::PY25Q128HA pq{spi_1, spi_1_cs, time_ms};

  m::MemoryPart rs485_mem{pq, 0, 256};
  m::MemoryPart settings_mem{pq, 256, 10 * 4 * 1024};

  Settings settings{settings_mem, time_ms, tracer, log};

  // ##################################################
  //                      SW
  // ##################################################

  PinWrapper in_1{GPIOA, Pin::PinNum::Pin_11, Pin::Mode::Input,
                  Pin::Inversion::Inverted};
  PinWrapper in_2{GPIOA, Pin::PinNum::Pin_10, Pin::Mode::Input,
                  Pin::Inversion::Inverted};
  PinWrapper in_3{GPIOA, Pin::PinNum::Pin_9, Pin::Mode::Input,
                  Pin::Inversion::Inverted};
  PinWrapper in_4{GPIOA, Pin::PinNum::Pin_8, Pin::Mode::Input,
                  Pin::Inversion::Inverted};

  // ##################################################
  //                      OUT
  // ##################################################
  PinWrapper out_1{GPIOC, Pin::PinNum::Pin_0, Pin::Mode::Output_PP};
  PinWrapper out_2{GPIOC, Pin::PinNum::Pin_1, Pin::Mode::Output_PP};

  // ##################################################
  //                      ELECTRODES
  // ##################################################
  PinWrapper el_hi{GPIOC, Pin::PinNum::Pin_13, Pin::Mode::Input,
                   Pin::Inversion::Inverted};
  PinWrapper el_low{GPIOC, Pin::PinNum::Pin_14, Pin::Mode::Input,
                    Pin::Inversion::Inverted};

  // ##################################################
  //                      RELAY
  // ##################################################
  PinWrapper rel_1{GPIOB, Pin::PinNum::Pin_1, Pin::Mode::Output_PP};
  PinWrapper rel_2{GPIOB, Pin::PinNum::Pin_2, Pin::Mode::Output_PP};
  PinWrapper rel_3{GPIOB, Pin::PinNum::Pin_10, Pin::Mode::Output_PP};
  PinWrapper rel_4{GPIOB, Pin::PinNum::Pin_11, Pin::Mode::Output_PP};
  PinWrapper rel_5{GPIOB, Pin::PinNum::Pin_12, Pin::Mode::Output_PP};
  PinWrapper rel_6{GPIOB, Pin::PinNum::Pin_13, Pin::Mode::Output_PP};
  PinWrapper rel_7{GPIOB, Pin::PinNum::Pin_14, Pin::Mode::Output_PP};
  PinWrapper rel_8{GPIOB, Pin::PinNum::Pin_15, Pin::Mode::Output_PP};

  // I2c_1 i2c_1;
  // m::ic::FDC1004 fdc{i2c_1};

  // ##################################################
  //                      PROTOCOL
  // ##################################################
  PinWrapper usart_4_dir{GPIOA, Pin::PinNum::Pin_15, Pin::Mode::Output_PP};
  Usart_4 usart_4{usart_4_dir};

  PinWrapper usart_5_dir{GPIOB, Pin::PinNum::Pin_3, Pin::Mode::Output_PP};
  Usart_5 usart_5{usart_5_dir};

  // ################################
  //           Smart home
  // ################################
  alignas(4) static std::array<uint8_t, 256> tx_buf_sh{0};
  alignas(4) static std::array<uint8_t, 256> rx_buf_sh{0};

  m::DataLinkAsync<Us<uint32_t>>::Timings d_l_timings_sh{
      .packet_rx_time_between_bytes =
          Us<uint32_t>{6 * 1'000'000 / usart_4.getBaudrate() + 50}};
  m::DataLinkAsync<Us<uint32_t>> data_link_sh{time_us, usart_4, d_l_timings_sh};

  m::ModbusRtuProtocol<Us<uint32_t>>::Timings timing_sh{.tx_response_delay =
                                                            500_Us};
  m::ModbusRtuProtocol<Us<uint32_t>> mdbs_sh{data_link_sh, time_us, timing_sh,
                                             rx_buf_sh, tx_buf_sh};

  // ################################
  //             Panel
  // ################################
  alignas(4) static std::array<uint8_t, 256> tx_buf_p{0};
  alignas(4) static std::array<uint8_t, 256> rx_buf_p{0};

  m::DataLinkAsync<Us<uint32_t>>::Timings d_l_timings_p{
      .packet_rx_time_between_bytes =
          Us<uint32_t>{6 * 1'000'000 / usart_5.getBaudrate() + 50}};
  m::DataLinkAsync<Us<uint32_t>> data_link_p{time_us, usart_5, d_l_timings_p};

  m::ModbusRtuProtocol<Us<uint32_t>>::Timings timing_p{.tx_response_delay =
                                                           500_Us};
  m::ModbusRtuProtocol<Us<uint32_t>> mdbs_p{data_link_p, time_us, timing_p,
                                            rx_buf_p, tx_buf_p};

  // ################################
  //           Modbus read
  // ################################
  m::ModbusRtuProtocol<Us<uint32_t>>::RMHR_Cb rmhr_cb =
      [&](uint16_t start_addr, uint16_t regs_num, std::span<uint16_t> regs)
      -> std::optional<m::ModbusRtuProtocol<Us<uint32_t>>::Error> {
    float temp_reg = 0.0f;

    settings.setPanelOnline(true);

    for (auto& reg : regs) {
      switch (start_addr) {
        case static_cast<uint16_t>(Regmap::Rel_1): {
          reg = rel_1.read();
        } break;
        case static_cast<uint16_t>(Regmap::Rel_2): {
          reg = rel_2.read();
        } break;
        case static_cast<uint16_t>(Regmap::Rel_3): {
          reg = rel_3.read();
        } break;
        case static_cast<uint16_t>(Regmap::Rel_4): {
          reg = rel_4.read();
        } break;
        case static_cast<uint16_t>(Regmap::Rel_5): {
          reg = rel_5.read();
        } break;
        case static_cast<uint16_t>(Regmap::Rel_6): {
          reg = rel_6.read();
        } break;
        case static_cast<uint16_t>(Regmap::Rel_7): {
          reg = rel_7.read();
        } break;
        case static_cast<uint16_t>(Regmap::Rel_8): {
          reg = rel_8.read();
        } break;

        case static_cast<uint16_t>(Regmap::Out_1): {
          reg = out_1.read();
        } break;
        case static_cast<uint16_t>(Regmap::Out_2): {
          reg = out_2.read();
        } break;

        case static_cast<uint16_t>(Regmap::In_1): {
          reg = in_1.read();
        } break;
        case static_cast<uint16_t>(Regmap::In_2): {
          reg = in_2.read();
        } break;
        case static_cast<uint16_t>(Regmap::In_3): {
          reg = in_3.read();
        } break;
        case static_cast<uint16_t>(Regmap::In_4): {
          reg = in_4.read();
        } break;

        case static_cast<uint16_t>(Regmap::WaterTopLevel): {
          reg = el_hi.read();
        } break;
        case static_cast<uint16_t>(Regmap::WaterBotLevel): {
          reg = el_low.read();
        } break;

        case static_cast<uint16_t>(Regmap::Ntc_1_lo): {
          if (auto value = ts_tank.value(); value) {
            temp_reg = value.value().value();
            auto temp = std::bit_cast<uint32_t>(temp_reg);
            reg = temp;
          } else {
            temp_reg = std::numeric_limits<float>::quiet_NaN();
          }

        } break;
        case static_cast<uint16_t>(Regmap::Ntc_1_hi): {
          auto value = std::bit_cast<uint32_t>(temp_reg);
          reg = value >> 16;
        } break;

        case static_cast<uint16_t>(Regmap::Ntc_2_lo): {
          if (auto value = ts_steam_room.value(); value) {
            temp_reg = value.value().value();
            auto temp = std::bit_cast<uint32_t>(temp_reg);
            reg = temp;
          } else {
            temp_reg = std::numeric_limits<float>::quiet_NaN();
          }
        } break;
        case static_cast<uint16_t>(Regmap::Ntc_2_hi): {
          auto value = std::bit_cast<uint32_t>(temp_reg);
          reg = value >> 16;
        } break;

        case static_cast<uint16_t>(Regmap::Ntc_3_lo): {
          if (auto value = ts_ntc_3.value(); value) {
            temp_reg = value.value().value();
            auto temp = std::bit_cast<uint32_t>(temp_reg);
            reg = temp;
          } else {
            temp_reg = std::numeric_limits<float>::quiet_NaN();
          }
        } break;
        case static_cast<uint16_t>(Regmap::Ntc_3_hi): {
          auto value = std::bit_cast<uint32_t>(temp_reg);
          reg = value >> 16;
        } break;

        case static_cast<uint16_t>(Regmap::WaterLevelCapSense): {
          reg = 0;
        } break;

        default:
          return m::ModbusRtuProtocol<Us<uint32_t>>::Error::IllegalDataAddress;
          break;
      }
      ++start_addr;
    }

    return std::nullopt;
  };

  // ################################
  //        Modbus write multi
  // ################################

  m::ModbusRtuProtocol<Us<uint32_t>>::WMHR_Cb wmhr_cb =
      [&](uint16_t start_addr, uint16_t regs_num, std::span<uint16_t> regs)
      -> std::optional<m::ModbusRtuProtocol<Us<uint32_t>>::Error> {
    for (auto& reg : regs) {
      switch (start_addr) {
        case static_cast<uint16_t>(Regmap::Rel_1): {
          if (el_low.read()) {
            rel_1.write(reg);
          } else {
            if (reg == 0) {
              rel_1.write(reg);
            }
          }
        } break;
        case static_cast<uint16_t>(Regmap::Rel_2): {
          rel_2.write(reg);
        } break;
        case static_cast<uint16_t>(Regmap::Rel_3): {
          rel_3.write(reg);
        } break;
        case static_cast<uint16_t>(Regmap::Rel_4): {
          rel_4.write(reg);
        } break;
        case static_cast<uint16_t>(Regmap::Rel_5): {
          rel_5.write(reg);
        } break;
        case static_cast<uint16_t>(Regmap::Rel_6): {
          rel_6.write(reg);
        } break;
        case static_cast<uint16_t>(Regmap::Rel_7): {
          rel_7.write(reg);
        } break;
        case static_cast<uint16_t>(Regmap::Rel_8): {
          rel_8.write(reg);
        } break;

        case static_cast<uint16_t>(Regmap::Out_1): {
          out_1.write(reg);
        } break;
        case static_cast<uint16_t>(Regmap::Out_2): {
          out_2.write(reg);
        } break;

        default:
          return m::ModbusRtuProtocol<Us<uint32_t>>::Error::IllegalDataAddress;
          break;
      }
      ++start_addr;
    }

    return std::nullopt;
  };

  mdbs_sh.addReadMultipleHoldingRegistersCallback(std::move(rmhr_cb));
  mdbs_sh.addWriteMultipleHoldingRegistersCallback(std::move(wmhr_cb));

  mdbs_p.addReadMultipleHoldingRegistersCallback(std::move(rmhr_cb));
  mdbs_p.addWriteMultipleHoldingRegistersCallback(std::move(wmhr_cb));

  // ##################################################
  //                      RUN
  // ##################################################

  led_g.write(1);
  time_ms.delay(500_Ms);
  led_g.write(0);

  if (!settings.load()) {
    log.add("Loading default settings");
    // ...
  }

  {
    auto addr = settings.getAddress();
    mdbs_sh.setAddress(addr);
    mdbs_p.setAddress(addr);
  }

  if (!adc_1.start(adc_data)) {
    log.add("Adc 1 start failed");
    tracer.add(static_cast<uint16_t>(HardwareError::Adc_1_Start));
  }
  time_ms.delay(100_Ms);  // Delay after ADC start

  std::function<void()> tim_7_it_cb = [&]() {
    mdbs_sh.handle();
    mdbs_p.handle();
  };
  Tim_7_1kHz tim_7_it;
  tim_7_it.setCallback(std::move(tim_7_it_cb));
  tim_7_it.start();

  Drivers drivers{time_us, time_ms, ts_tank, ts_steam_room, ts_ntc_3, in_1,
                  in_2,    in_3,    in_4,    out_1,         out_2,    rel_1,
                  rel_2,   rel_3,   rel_4,   rel_5,         rel_6,    rel_7,
                  rel_8,   el_hi,   el_low};

  Machine fsm{drivers, settings, tracer, log};

  // volatile bool memTest = m::tsts::memoryTest<4678>(settings_mem);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    fsm.handle();
    log.handle();
    settings.handle();
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType =
      RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}
/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state
   */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
     file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
