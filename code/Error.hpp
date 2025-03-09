#pragma once

#include <cstdint>

enum class HardwareError : uint16_t {
  Adc_1_Start = 1,
  Write_Mem,
  Read_Mem,
  Mem_Bad_Hash,
  Tank_Ntc_Shorted,
  SR_Ntc_Shorted,
  
  Size
};

enum class LogicError : uint16_t {
  Adc_1_Start = static_cast<uint16_t>(HardwareError::Size) + 1,

  Size
};
