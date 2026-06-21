#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>

#include "core/Bus.h"

int main() {
  auto logger = spdlog::stdout_color_mt("yangboy");
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::trace);  // set via config/arg in practice

  Bus system;

  system.clock();

  return 0;
}