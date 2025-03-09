#pragma once
#include <cstdint>

enum class Regmap : uint16_t {

  // ######################
  //       Hardware 
  // ######################
  // RW
  Rel_1 = 1,  // Heater
  Rel_2 = 2,
  Rel_3 = 3,
  Rel_4 = 4,
  Rel_5 = 5,
  Rel_6 = 6,
  Rel_7 = 7,
  Rel_8 = 8,
  // RW
  Out_1 = 9,
  Out_2 = 10,
  // RO
  In_1 = 11,  // Remote control
  In_2 = 12,  // Door
  In_3 = 13,
  In_4 = 14,
  // RO
  WaterTopLevel = 15,
  WaterBotLevel = 16,
  // RO
  Ntc_1_lo = 17,  // Tank
  Ntc_1_hi,
  Ntc_2_lo = 19,  // Steam Room
  Ntc_2_hi,
  Ntc_3_lo = 21,
  Ntc_3_hi,
  // RO
  WaterLevelCapSense = 23,

};