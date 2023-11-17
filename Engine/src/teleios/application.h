#ifndef TELEIOS_APPLICATION_H
#define TELEIOS_APPLICATION_H

#include "teleios/engine.h"
#include "teleios/logger.h"

void tl_application_define(TLSpecification* spec);
b8 tl_application_initialize(const TLSpecification* spec);
b8 tl_application_terminate();

int main(int argc, char** argv) {
  TLSpecification spec = { 0 };
  
  spec.name = "Teleios APP";
  spec.window.witdh = 640;
  spec.window.height = 480;

  tl_application_define(&spec);
  if (!tl_engine_pre_initialize(&spec)) return -1;
  if (!tl_engine_initialize(&spec)) return -1;
  TLDEBUG("tl_application_initialize");
  if (!tl_application_initialize(&spec)) return -99;
  if (!tl_engine_run()) return -1;
  TLDEBUG("tl_application_terminate");
  if (!tl_application_terminate()) return -99;
  if (!tl_engine_terminate()) return -1;

  return 0;
}

#endif // TELEIOS_APPLICATION_H
