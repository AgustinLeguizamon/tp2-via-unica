#include <iostream>
#include "via_unica.cpp"

int main(int argc, char const *argv[]){
	std::string ns("NS");
	std::string sn("SN");
	if(argc != 2 || ((ns.compare(argv[1]) != 0) && ( sn.compare(argv[1]) != 0 ) )){
		std::cout << "Uso: ./coordinador <NS o SN>" << std::endl;
		return 1;
	}
	ViaUnica via("via_unica_1");
	via.changeDirection(argv[1]);
	return 0;
}