#include "step_1_SwapCells.hpp"
#include "def/DEFReader.hpp"
#include <string>
#include "def/DEF.hpp"
#include <vector>
#include <iostream>
#include "verilog/Verilog.hpp"

/*
Вопросы:зачем в instances поле модуль и надо ли раскладыватьь его на внутреннюю структуру

*/
void step_1_SwapCells(verilog::Module* top ) {
	std::vector<std::vector<verilog::Instance *> > Sort_Instance; 

	//Сортировка начата
	int flag = 0;
	for (int i = 0; i < top->instances.size(); i++) {
		if (Sort_Instance.size() != 0) {
			for (int o = 0; o < Sort_Instance.size();o++) {
				if (top->instances[i]->placement.component->modelName == Sort_Instance[o][0]->placement.component->modelName) {
					flag = 1;
					Sort_Instance[o].push_back(top->instances[i]);
				}
			}
			if (flag == 0) {
				Sort_Instance.push_back(std::vector<verilog::Instance*>());
				Sort_Instance.back().push_back(top->instances[i]);
			}
			flag = 0;
		}
		else {
			Sort_Instance.push_back(std::vector<verilog::Instance*>());
			Sort_Instance.back().push_back(top->instances[i]);
		}
	}
	//Сортировка закончена
	
    //Начало алгоритма свапа
	if (Sort_Instance.size() > 0) {
		for (int i = 0; i < Sort_Instance.size(); i++) {//Нужна проверка длинны строки
			if (Sort_Instance[i].size() > 1) {
				for (int j = 0; j < (Sort_Instance[i].size() - 1); j++) {
					for (int k = (j + 1); k < Sort_Instance[i].size(); k++) {
						//блок условий
						if ((Sort_Instance[i][j]->placement.component->FIXED != def::FIXED_class::FIXED) && (Sort_Instance[i][k]->placement.component->FIXED != def::FIXED_class::FIXED)) {
							if (((Sort_Instance[i][j]->placement.component->POS.x - Sort_Instance[i][k]->placement.component->POS.x)+(Sort_Instance[i][j]->placement.component->POS.y - Sort_Instance[i][k]->placement.component->POS.y)) < Sort_Instance[i][j]->placement.radius) {
							//Тут должно быть условие проверки сокращения длинны связей
							}	
						}
					}
				}
			}
		}
	}
	//Конец алгоритма свапа 
}