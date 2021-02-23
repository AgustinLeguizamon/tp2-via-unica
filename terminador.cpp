#include <iostream>
#include "Sem-sv/sv_sem.h"
#include "via_unica.cpp"

int main(int argc, char const *argv[]){
	std::cout << "Terminador" << std::endl;
	ViaUnica viaUnica("via_unica_7");
	std::cout << "Borrando" << viaUnica << std::endl;
	viaUnica.del();
	return 0;
}