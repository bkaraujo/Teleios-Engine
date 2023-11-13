#ifndef TELEIOS_ENTRY_H
#define TELEIOS_ENTRY_H

#include "teleios/engine.h"

int main(int argc, char** argv) {
  TLSpecification spec = { 0 };
  
  spec.name = "Teleios APP";
  spec.window.witdh = 640;
  spec.window.height = 480;

  if (!tl_engine_pre_initialize(&spec)) return -1;
  if (!tl_engine_initialize(&spec)) return -1;
  if (!tl_engine_run()) return -1;
  if (!tl_engine_terminate()) return -1;

  return 0;
}

#endif // TELEIOS_ENTRY_H
