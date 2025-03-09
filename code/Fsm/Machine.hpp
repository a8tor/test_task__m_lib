#pragma once
#include <Fsm.hpp>
#include <IErrorTracer.hpp>
#include <ILog.hpp>

#include "Drivers.hpp"
#include "Error_S.hpp"
#include "Settings.hpp"
#include "Startup_S.hpp"

class Machine {
 public:
  Machine(Drivers& drivers, Settings& settings,
          m::ifc::IErrorTracer<uint16_t>& tracer, m::ifc::ILog& log)
      : drivers_(drivers), settings_(settings), tracer_(tracer), log_(log) {
    startup_tr_[static_cast<uint32_t>(Startup_S::Event::Remote_Only)] =
        &erros_s_;
    startup_tr_[static_cast<uint32_t>(Startup_S::Event::Remote_And_Ts)] =
        &erros_s_;
    startup_tr_[static_cast<uint32_t>(Startup_S::Event::Panel_And_Remote)] =
        &erros_s_;
    startup_tr_[static_cast<uint32_t>(Startup_S::Event::Error)] = &erros_s_;

    error_tr_[static_cast<uint32_t>(Error_S::Event::Ok)] = &erros_s_;
  }

  void handle() { fsm_.dispatch(); }

 private:
  Drivers& drivers_;
  Settings& settings_;
  m::ifc::IErrorTracer<uint16_t>& tracer_;
  m::ifc::ILog& log_;

  std::array<m::IState*, static_cast<uint32_t>(Startup_S::Event::Size)>
      startup_tr_;
  Startup_S startup_s_{startup_tr_, drivers_, settings_, tracer_, log_};

  std::array<m::IState*, static_cast<uint32_t>(Error_S::Event::Size)> error_tr_;
  Error_S erros_s_{error_tr_, drivers_, settings_};

  m::Fsm fsm_{&startup_s_};
};