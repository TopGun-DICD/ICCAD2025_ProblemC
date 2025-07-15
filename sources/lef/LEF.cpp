#include "LEF.hpp"

#include <iostream>

lef::Site::~Site() {};

lef::Macro::~Macro() {
	for (int i = 0; i < pins.size(); ++i)
		delete pins[i];
	pins.clear();
};

lef::Pin* lef::Macro::getPinByName(const std::string& name) {
	for (auto* pin : pins) {
		if (pin->name == name) {
			return pin;
		}
	}
	return nullptr;
}

lef::LEFData::~LEFData() {
	for (int i = 0; i < macroes.size(); ++i)
		delete macroes[i];
	macroes.clear();
}

void lef::LEFData::addMacro(Macro *_macro) {
	if (!_macro) {
		std::cout << "__err__ : LEFData::addMacro with null pointer was called.\n";
		return;
	}
	for (auto *macro : macroes)
		if (macro->name == _macro->name) {
			std::cout << "__err__ : LEFData::addMacro: MACRO with the name '" << _macro->name 
					  << "' already exists in macro collection.\n";
			return;
		}
	macroes.push_back(_macro);
}

void lef::LEFData::addSite(Site *_site) {
	if (!_site) {
		std::cout << "__err__ : LEFData::addSite with null pointer was called.\n";
		return;
	}
	for (auto *site : sites)
		if (site->name == _site->name) {
			std::cout << "__err__ : LEFData::addSite: SITE with the name '" << _site->name 
					  << "' already exists in macro collection.\n";
			return;
		}
	sites.push_back(_site);
}

lef::Macro* lef::LEFData::getMacroByName(const std::string &_name) {
	for (auto *macro : macroes)
		if (macro->name == _name)
			return macro;
	return nullptr;
}

lef::Site* lef::LEFData::getSiteByName(const std::string &_name) {
	for (auto *site: sites)
		if (site->name == _name)
			return site;
	return nullptr;
}

std::pair<double, double> lef::LEFData::getMacroSize(const std::string &_name) const {
	for (auto* macro : macroes) {
		if (macro->name == _name) {
			return { macro->sizeX, macro->sizeY };
		}
	}
	return { 0.0, 0.0 };
}

std::pair<double, double> lef::LEFData::getScaledMacroSize(const std::string &_name) const {
	auto size = LEFData::getMacroSize(_name);
	return { size.first * LEF_TO_DEF_SCALE, size.second * LEF_TO_DEF_SCALE };
}