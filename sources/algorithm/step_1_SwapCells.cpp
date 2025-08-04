#include "step_1_SwapCells.hpp"
#include "../def/DEFReader.hpp"
#include <string>
#include "../def/DEF.hpp"
#include <vector>
#include <iostream>
#include "../verilog/Verilog.hpp"

 
int recalculating_links(verilog::Instance inst, def::Position pos) {
	int sum = 0;//В процессе вычисления уменьшина на 2 порядка от переполнения
	for (int i = 0; i < inst.ins.size(); i++) {
		for (int o = 0; o < inst.ins[i]->sourceFor.size(); o++) {
			sum = sum + (abs(inst.ins[i]->sourceFor[o]->placement.component->POS.x - pos.x)) + (abs(inst.ins[i]->sourceFor[o]->placement.component->POS.y - pos.y))/100;
		}
	}
	for (int i = 0; i < inst.outs.size(); i++) {
		for (int o = 0; o < inst.outs[i]->sourceFor.size(); o++) {
			sum = sum + (abs(inst.outs[i]->sourceFor[o]->placement.component->POS.x - pos.x)) + (abs(inst.outs[i]->sourceFor[o]->placement.component->POS.y - pos.y))/100;
		}
	}
	for (int i = 0; i < inst.unknown.size(); i++) {
		for (int o = 0; o < inst.unknown[i]->sourceFor.size(); o++) {
			sum = sum + (abs(inst.unknown[i]->sourceFor[o]->placement.component->POS.x - pos.x)) + (abs(inst.unknown[i]->sourceFor[o]->placement.component->POS.y - pos.y))/100;
		}
	}
	return sum;
}

void step_1_SwapCells_u(verilog::Module* top, def::DEF_File* def ) {
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
	/*//Длинна всех связей
	unsigned  long int summ = 0;
	for (int i = 0; i < Sort_Instance.size(); i++) {//Нужна проверка длинны строки
		for (int j = 0; j < Sort_Instance[i].size(); j++) {
			summ = summ + recalculating_links((*Sort_Instance[i][j]), Sort_Instance[i][j]->placement.component->POS);
		}
	}
	*///Конец подсчета


	int u = 0;
    //Начало алгоритма свапа
	if (Sort_Instance.size() > 0) {
		for (int i = 0; i < Sort_Instance.size(); i++) {//Нужна проверка длинны строки
			if (Sort_Instance[i].size() > 1) {
				for (int j = 0; j < (Sort_Instance[i].size() - 1); j++) {
					for (int k = (j + 1); k < Sort_Instance[i].size(); k++) {
						//блок условий
						if ((Sort_Instance[i][j]->placement.component->FIXED != def::FIXED_class::FIXED) && (Sort_Instance[i][k]->placement.component->FIXED != def::FIXED_class::FIXED)) {
							// Мы не производим пересчет радиусов
							//if (((Sort_Instance[i][j]->placement.component->POS.x - Sort_Instance[i][k]->placement.component->POS.x)+(Sort_Instance[i][j]->placement.component->POS.y - Sort_Instance[i][k]->placement.component->POS.y)) < Sort_Instance[i][j]->placement.radius) { 
								if ((recalculating_links((*Sort_Instance[i][j]), Sort_Instance[i][k]->placement.component->POS) + recalculating_links((*Sort_Instance[i][k]), Sort_Instance[i][j]->placement.component->POS)) < (recalculating_links((*Sort_Instance[i][j]), Sort_Instance[i][j]->placement.component->POS) + recalculating_links((*Sort_Instance[i][k]), Sort_Instance[i][k]->placement.component->POS))) {
									def::Position p = Sort_Instance[i][k]->placement.component->POS;
									Sort_Instance[i][k]->placement.component->POS = Sort_Instance[i][j]->placement.component->POS;
									Sort_Instance[i][j]->placement.component->POS = p;
									
								//}
							}	
						}
					}
				}
			}
		}
	}
	//Конец алгоритма свапа 
	/*
	unsigned  long int summ2 = 0;
	for (int i = 0; i < Sort_Instance.size(); i++) {//Нужна проверка длинны строки
			for (int j = 0; j < Sort_Instance[i].size() ; j++) {	
					summ2 = summ2 + recalculating_links((*Sort_Instance[i][j]), Sort_Instance[i][j]->placement.component->POS);
		}
	}
	std::cout << summ << std::endl;
	std::cout << summ2 << std::endl;
	*/
	//Занесение результатов
	int h = 0;
		for (int i = 0; i < Sort_Instance.size(); i++) {
			for (int j = 0; j < Sort_Instance[i].size(); j++) {
				def->COMPONENTS[h] = Sort_Instance[i][j]->placement.component;
				top->instances[h] = Sort_Instance[i][j];
				h++;
			}
	    }
	
}