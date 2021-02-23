#include <iostream>
#include "via_unica.cpp"

int main(int argc, char const *argv[]){
	/*checkear si fue inicializado*/
	ViaUnica via("via_unica_1");

	std::string c;
	std::cout << "TrenNS ";
	std::cout << "Entrando a la Via Unica"<< std::endl;
	via.inNorth();
	std::cout << "Dentro de la via unica..." << std::endl;
	std::cin >> c;
	via.outSouth();
	std::cout << "TrenNS ";
	std::cout << "Saliendo de la Via Unica" << std::endl;
	
	return 0;
}