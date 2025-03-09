#pragma once

#include <Fsm.hpp>
#include <Timer.hpp>

#include "Drivers.hpp"
#include "Settings.hpp"

class Startup_S final : public m::IState {
 public:
  Startup_S(std::span<m::IState*> transitions, Drivers& drivers,
            Settings& settings, m::ifc::IErrorTracer<uint16_t>& tracer,
            m::ifc::ILog& log)
      : IState(transitions),
        drivers_(drivers),
        settings_(settings),
        tracer_(tracer),
        log_(log) {}

  void entry() override {
    log_.add("Startup state");
    timer_.restart(1'000_Ms);
  }

  void run() override {}

  enum class Event : uint32_t {
    Remote_Only = 0,
    Remote_And_Ts,
    Panel_And_Remote,
    Error,

    Size
  };
  std::optional<uint32_t> checkEvents() override {
    if (timer_.timeOver()) {
      // TODO: Some project logic
    }
    return std::nullopt;
  }

  void exit() override {}

 private:
  Drivers& drivers_;
  Settings& settings_;
  m::ifc::IErrorTracer<uint16_t>& tracer_;
  m::ifc::ILog& log_;

  m::Timer<Ms<uint32_t>> timer_{drivers_.getTimeMs()};
};