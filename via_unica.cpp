#include "via_unica.h"

ViaUnica::ViaUnica(std::string key, bool init){
	this->_key=key;
	std::string mutex_key=key+"_mutex";
	std::string shm_key=key+"_shm";
	std::string north_mutex_key=key+"_north";
	std::string south_mutex_key=key+"_south";
	
	this->_mutex = sv_sem(mutex_key,1);
	/*Pongo en 0 para que al prime wait se cierre*/
	this->_north = sv_sem(north_mutex_key,0);
	this->_south = sv_sem(south_mutex_key,0);
	this->_memViaUnica = sv_shm(shm_key);

	this->_p_trains = reinterpret_cast<trains*>(_memViaUnica.map(sizeof(trains)));
	
	if(init){
		_p_trains->init(0,0,0,SN);
	}

	std::cout << "Tomado " << this->_mutex << std::endl;
	std::cout << "Tomado " << this->_north << std::endl;
	std::cout << "Tomado " << this->_south << std::endl;
	std::cout << "Tomado " << this->_memViaUnica << " con valor " << _p_trains->getInside() << std::endl;
}

/*ver si Race condition? 
cada in ejecuta esto, por lo tanto
pueden haber muchas mas posts al estar todos
iterando*/

/*separar el post del enterNorth para evitar RC cuando hago un for*/
void ViaUnica::_openNorth(){
	_p_trains->enterNorth();
	std::cout << "DEBUG Entro Norte - trenes adentro: " << _p_trains->getInside() << std::endl;
	std::cout << "En cola norte:" << _p_trains->getQueuedNorth() << std::endl;
	_north.post();
}


void ViaUnica::_openSouth(){
	_p_trains->enterSouth();
	std::cout << "DEBUG Entro Sur - trenes adentro:" << _p_trains->getInside() << std::endl;
	std::cout << "En cola sur:" << _p_trains->getQueuedSouth() << std::endl;
	_south.post();
}


void ViaUnica::inNorth(){
	_mutex.wait();
	
	_p_trains->queueNorth();

	std::cout << "En cola norte:" << _p_trains->getQueuedNorth() << std::endl;
	if(_p_trains->isNorthOpen()){
		/*abro uno solo, solo funciona si entra de uno
		acordate que al liberar no siempre es el mismo que lo bloqueo*/
		_openNorth();
	}
	
	_mutex.post();
	/*wait va despues pq si no, no libero el mutex*/
	_north.wait();
}

/*mutex no es necesario*/
void ViaUnica::outSouth(){
	_mutex.wait();
	
	_p_trains->out();
	std::cout << "Salio sur - trenes adentro" << _p_trains->getInside() << std::endl;

	_mutex.post();
}


void ViaUnica::inSouth(){
	_mutex.wait();
	/*NOTA 1: que pasa si cambio direccion pero todavia tengo trenes de SN??*/
	/*deberia esperar a que se vacie los de ese sentido*/
	/*actualmente funciona solo si cambio de direccion y esta vacio*/
	
	_p_trains->queueSouth();
	std::cout << "En cola sur:" << _p_trains->getQueuedSouth() << std::endl;
		
	if(_p_trains->isSouthOpen()){
		_openSouth();
	}
	
	_mutex.post();
	_south.wait();
}

void ViaUnica::outNorth(){
	_mutex.wait();
	
	_p_trains->out();

	std::cout << "Salio norte - trenes adentro: " << _p_trains->getInside() << std::endl;	
	_mutex.post();
}

void ViaUnica::changeDirection(const std::string& s_new_direction){
	_mutex.wait();
	if(s_new_direction.compare("SN") == 0){
		_p_trains->setDirection(SN);
		std::cout << "Sentido SN habilitado" << std::endl;
		/*abro para todos los que estan esperando entrar
		OJO, que pasa si todavia tengo trenes en sentido contrario
		tengo que esperar a que se vacie y eso no lo contempla*/
		const int queued_south = _p_trains->getQueuedSouth();
		for(int i=0 ; i < queued_south; i++){
			_openSouth();
		}		
	} else if(s_new_direction.compare("NS") == 0){
		std::cout << "Sentido NS habilitado" << std::endl;
		_p_trains->setDirection(NS);
		/*Race condition cada openNOrth me reduce el contador, jodiendo eeste for
		por eso lo guardo antes*/
		const int queued_north = _p_trains->getQueuedNorth();
		for(int i=0 ; i < queued_north; i++){
			_openNorth();
		}
	} else {
		std::cout << "Ingresar: SN o NS" << std::endl; 
	}
	
	_mutex.post();
}

void ViaUnica::del(){
	std::cout << "Borrando " << this->_mutex << std::endl;
	this->_mutex.del();
	std::cout << "Borrando " << this->_north << std::endl;
	this->_north.del();
	std::cout << "Borrando " << this->_south << std::endl;
	this->_south.del();
	std::cout << "Eliminando " << this->_memViaUnica << std::endl;
	this->_memViaUnica.del();
}
