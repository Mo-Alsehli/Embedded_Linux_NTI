#include "led.h"

Led::Led(std::fstream& f) : file(std::move(f)) {}

Led::~Led() { file.close(); }

void Led::turn_capslock_on() { file << 1; }

void Led::turn_capslock_off() { file << 0; }