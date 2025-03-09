#pragma once

#include <HashFAQ6.hpp>
#include <IMemory.hpp>
#include <Timer.hpp>

#include "Error.hpp"

class Settings {
 public:
  Settings(m::ifc::IMemory &mem, m::ifc::ITime<Ms<uint32_t>> &time,
           m::ifc::IErrorTracer<uint16_t> &tracer, m::ifc::ILog &log)
      : mem_(mem), time_(time), tracer_(tracer), log_(log) {
    timer_.restart(10'000_Ms);
  }

  bool getPanelOnline() { return panel_online_; }
  void setPanelOnline(bool value) { panel_online_ = value; }

  bool getTankNtcState() { return data_.tank_ntc; }
  void setTankNtcState(bool value) {
    if (data_.tank_ntc != value) {
      data_.tank_ntc = value;
      changes_ = true;
      timer_.reset();
    }
  }

  // RS485 modbus address
  bool getAddress() { return data_.addr; }
  void setAddress(uint8_t value) {
    if (data_.addr != value) {
      data_.addr = value;
      changes_ = true;
      timer_.reset();
    }
  }

  Ms<uint32_t> getDrainPeriod() { return data_.drain_period; }
  void setDrainPeriod(Ms<uint32_t> value) {
    if (data_.drain_period != value) {
      data_.drain_period = value;
      changes_ = true;
      timer_.reset();
    }
  }

  bool load() {
    if (!mem_.read(0, std::span<uint8_t>{(uint8_t *)&data_, sizeof(Data)})) {
      tracer_.add(static_cast<uint16_t>(HardwareError::Read_Mem));
      log_.add("Load settings err");
      return false;
    }

    m::HashFAQ6::Hash hash{uint8_t(data_.hash), uint8_t(data_.hash >> 8),
                           uint8_t(data_.hash >> 16),
                           uint8_t(data_.hash >> 24)};
    if (!hash_.check(std::span<uint8_t const>{(uint8_t *)&data_,
                                              sizeof(Data) - hash_.size()},
                     hash)) {
      tracer_.add(static_cast<uint16_t>(HardwareError::Mem_Bad_Hash));
      log_.add("Settings bad hash err");
      return false;
    }

    return true;
  }

  bool save() {
    changes_ = false;

    alignas(4) Data data_copy;
    data_copy = data_;

    auto temp = hash_.calc(std::span<uint8_t const>{
        reinterpret_cast<uint8_t *>(&data_copy), sizeof(Data) - hash_.size()});
    data_copy.hash =
        temp[0] | (temp[1] << 8) | (temp[2] << 16) | (temp[3] << 24);
    return mem_.write(
        0, std::span<uint8_t>{reinterpret_cast<uint8_t *>(&data_copy),
                              sizeof(Data)});
  }

  void handle() {
    if (changes_) {
      if (timer_.timeOver()) {
        if (!save()) {
          tracer_.add(static_cast<uint16_t>(HardwareError::Write_Mem));
          log_.add("Save settings err");
        }
      }
    }
  }

 private:
  m::ifc::IMemory &mem_;
  m::ifc::ITime<Ms<uint32_t>> &time_;
  m::ifc::IErrorTracer<uint16_t> &tracer_;
  m::ifc::ILog &log_;

  m::HashFAQ6 hash_;
  m::Timer<Ms<uint32_t>> timer_{time_};

#pragma pack(push, 1)
  struct Data {
    uint8_t addr;  // RS485 modbus address
    Ms<uint32_t> drain_period;
    bool tank_ntc;
    uint32_t hash;
  };
#pragma pack(pop)
  static_assert(sizeof(Data) == 10, "Wrong sizeof(Data)");

  alignas(4) Data data_{};

  bool changes_ = false;

  bool panel_online_ = false;
};
