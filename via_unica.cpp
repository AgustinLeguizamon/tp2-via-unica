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
		_p_trains->init();
		changeDirection("SN");
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
void ViaUnica::_openNorth(){
	_p_trains->enterNorth();
	std::cout << "DEBUG N: trenes adentro" << _p_trains->getInside() << std::endl;
	_north.post();
}


void ViaUnica::_openSouth(){
	_p_trains->enterSouth();
	std::cout << "DEBUG S: trenes adentro" << _p_trains->getInside() << std::endl;
	_south.post();
}


void ViaUnica::inNorth(){
	_mutex.wait();
	
	_p_trains->queueNorth();
	if(_direction == "NS"){
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
	
	_mutex.post();
}


void ViaUnica::inSouth(){
	_mutex.wait();
	/*NOTA 1: que pasa si cambio direccion pero todavia tengo trenes de SN??*/
	/*deberia esperar a que se vacie los de ese sentido*/
	/*actualmente funciona solo si cambio de direccion y esta vacio*/
	_p_trains->queueSouth();
	if(_direction == "SN"){
		_openSouth();
	}
	
	_mutex.post();
}

void ViaUnica::outNorth(){
	_mutex.wait();
	
	_p_trains->out();
	
	_mutex.post();
}

void ViaUnica::changeDirection(std::string s_new_direction){
	_mutex.wait();
	if(s_new_direction == "SN"){
		this->_direction=s_new_direction;
		std::cout << "Sentido SN habilitado" << std::endl;
		/*abro para todos los que estan esperando entrar*/
		for(int i=0 ; i < _p_trains->getQueuedSouth(); i++){
			_openSouth();
		}		
	} else if(s_new_direction == "NS"){
		std::cout << "Sentido NS habilitado" << std::endl;
		this->_direction=s_new_direction;
		for(int i=0 ; i < _p_trains->getQueuedNorth(); i++){
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
