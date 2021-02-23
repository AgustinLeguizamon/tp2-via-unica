#include <iostream>
#include <fstream>
#include "Sem-sv/sv_sem.h"
#include "via_unica.cpp"

int main(int argc, char const *argv[]){
	ViaUnica viaUnica("via_unica_7", true);
	std::cout << "Creada " << viaUnica <<std::endl;
	return 0;
}
