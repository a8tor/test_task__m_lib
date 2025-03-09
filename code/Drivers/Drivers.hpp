#pragma once

#include <Celsius.hpp>
#include <IPin.hpp>
#include <ITime.hpp>

#include "ITempSenseWithError.hpp"

class Drivers {
 public:
  Drivers(m::ifc::ITime<Us<uint32_t>>& time_us,
          m::ifc::ITime<Ms<uint32_t>>& time_ms, ITempSenseWithError& ts_tank,
          ITempSenseWithError& ts_steam_room, ITempSenseWithError& ts_ntc_3,
          m::ifc::mcu::IPin& in_1, m::ifc::mcu::IPin& in_2,
          m::ifc::mcu::IPin& in_3, m::ifc::mcu::IPin& in_4,
          m::ifc::mcu::IPin& out_1, m::ifc::mcu::IPin& out_2,
          m::ifc::mcu::IPin& rel_1, m::ifc::mcu::IPin& rel_2,
          m::ifc::mcu::IPin& rel_3, m::ifc::mcu::IPin& rel_4,
          m::ifc::mcu::IPin& rel_5, m::ifc::mcu::IPin& rel_6,
          m::ifc::mcu::IPin& rel_7, m::ifc::mcu::IPin& rel_8,
          m::ifc::mcu::IPin& el_hi, m::ifc::mcu::IPin& el_low)

      : time_us_(time_us),
        time_ms_(time_ms),
        ts_tank_(ts_tank),
        ts_steam_room_(ts_steam_room),
        ts_ntc_3_(ts_ntc_3),
        in_1_(in_1),
        in_2_(in_2),
        in_3_(in_3),
        in_4_(in_4),
        out_1_(out_1),
        out_2_(out_2),
        rel_1_(rel_1),
        rel_2_(rel_2),
        rel_3_(rel_3),
        rel_4_(rel_4),
        rel_5_(rel_5),
        rel_6_(rel_6),
        rel_7_(rel_7),
        rel_8_(rel_8),
        el_hi_(el_hi),
        el_low_(el_low) {}

  m::ifc::ITime<Us<uint32_t>>& getTimeUs() { return time_us_; }
  m::ifc::ITime<Ms<uint32_t>>& getTimeMs() { return time_ms_; }

  ITempSenseWithError& getNtcTank() { return ts_tank_; }
  ITempSenseWithError& getNtcSteamRoom() { return ts_steam_room_; }
  ITempSenseWithError& getNtc_3() { return ts_ntc_3_; }

  m::ifc::mcu::IPin& getRemoteControlInput() { return in_1_; }
  m::ifc::mcu::IPin& getIn2() { return in_2_; }
  m::ifc::mcu::IPin& getIn3() { return in_3_; }
  m::ifc::mcu::IPin& getIn4() { return in_4_; }

  m::ifc::mcu::IPin& getOut_1() { return out_1_; }
  m::ifc::mcu::IPin& getOut_2() { return out_2_; }

  m::ifc::mcu::IPin& getHeaterRel() { return rel_1_; }
  m::ifc::mcu::IPin& getRel_2() { return rel_2_; }
  m::ifc::mcu::IPin& getRel_3() { return rel_3_; }
  m::ifc::mcu::IPin& getRel_4() { return rel_4_; }
  m::ifc::mcu::IPin& getRel_5() { return rel_5_; }
  m::ifc::mcu::IPin& getRel_6() { return rel_6_; }
  m::ifc::mcu::IPin& getRel_7() { return rel_7_; }
  m::ifc::mcu::IPin& getRel_8() { return rel_8_; }

  m::ifc::mcu::IPin& getWaterHi() { return el_hi_; }
  m::ifc::mcu::IPin& getWaterLo() { return el_low_; }

 private:
  m::ifc::ITime<Us<uint32_t>>& time_us_;
  m::ifc::ITime<Ms<uint32_t>>& time_ms_;
  ITempSenseWithError& ts_tank_;
  ITempSenseWithError& ts_steam_room_;
  ITempSenseWithError& ts_ntc_3_;
  m::ifc::mcu::IPin& in_1_;
  m::ifc::mcu::IPin& in_2_;
  m::ifc::mcu::IPin& in_3_;
  m::ifc::mcu::IPin& in_4_;
  m::ifc::mcu::IPin& out_1_;
  m::ifc::mcu::IPin& out_2_;
  m::ifc::mcu::IPin& rel_1_;
  m::ifc::mcu::IPin& rel_2_;
  m::ifc::mcu::IPin& rel_3_;
  m::ifc::mcu::IPin& rel_4_;
  m::ifc::mcu::IPin& rel_5_;
  m::ifc::mcu::IPin& rel_6_;
  m::ifc::mcu::IPin& rel_7_;
  m::ifc::mcu::IPin& rel_8_;
  m::ifc::mcu::IPin& el_hi_;
  m::ifc::mcu::IPin& el_low_;
};
