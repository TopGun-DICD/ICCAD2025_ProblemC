#include "LEF.hpp"

Site::~Site() {};

Macro::~Macro() {};

Pin* Macro::getPinByName(const std::string& name) {
	for (auto& pin : pins) {
		if (pin.name == name) {
			return &pin;
		}
	}
	return nullptr;
}