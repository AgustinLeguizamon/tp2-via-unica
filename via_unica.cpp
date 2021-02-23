#include "via_unica.h"

ViaUnica::ViaUnica(std::string key, bool init){
	this->_key=key;
	std::string mutex_key=key+"_mutex";
	std::string shm_key=key+"_shm";
	std::string north_mutex_key=key+"_north";
	std::string south_mutex_key=key+"_south";
	
	this->_mutex = sv_sem(mutex_key);
	this->_north = sv_sem(north_mutex_key);
	this->_south = sv_sem(south_mutex_key);
	this->_memViaUnica = sv_shm(shm_key);

	this->_p_counter = reinterpret_cast<counter*>(_memViaUnica.map(sizeof(counter)));
	std::cout << "Tomado " << this->_mutex << std::endl;
	std::cout << "Tomado " << this->_north << std::endl;
	std::cout << "Tomado " << this->_south << std::endl;
	std::cout << "Tomado " << this->_memViaUnica << " con valor " << _p_counter->n << std::endl;

	if(init){
		changeDirection("SN");	
	}
}

void ViaUnica::inNorth(){
	_mutex.wait();
	
	/*Si sentido es SN se me bloquea*/
	if(_p_counter->n == 0) _north.wait();
	
	/*cuando se desbloquea, si lo estaba, entra a la via*/	
	_p_counter->n++;
	cout << "Trenes en via unica: " << _p_counter->n << std::endl;
	
	/*toma el mutex sur, -1 salida sur pero bloquea si es 0*/
	//_south.wait()

	_mutex.post();
}

void ViaUnica::outSouth(){
	_mutex.wait();
	
	_p_counter->n--;
	cout << "Trenes en via unica:" << _p_counter->n << std::endl;
	
	//+1 a la salida sur, libera un proceso bloqueado por _south
	//_south.post();
	
	/*cuando es el utlimo en salir, _north vuelve a 1; libero el mutex*/
	if(_p_counter->n == 0) _north.post();
	
	_mutex.post();
}


void ViaUnica::inSouth(){
	_mutex.wait();
	/*NOTA 1: que pasa si cambio direccion pero todavia tengo trenes de SN??*/
	/*deberia esperar a que se vacie los de ese sentido*/
	/*actualmente funciona solo si cambio de direccion y esta vacio*/

	/*Si sentido es NS se me bloquea*/
	if(_p_counter->n == 0) _south.wait();
	
	_p_counter->n++;
	cout << "Trenes en via unica:" << _p_counter->n << std::endl;
	
	//-1 semaforo norte
	//_north.wait();

	_mutex.post();
}

void ViaUnica::outNorth(){
	_mutex.wait();
	
	_p_counter->n--;
	cout << "Trenes en via unica:" << _p_counter->n << std::endl;
	
	if(_p_counter->n == 0) _south.post();

	_mutex.post();
}

void ViaUnica::changeDirection(std::string s_new_direction){
	_mutex.wait();
	if(s_new_direction == "SN"){
		_south.post();
		_north.wait();
		this->_direction=s_new_direction;
		std::cout << "Sentido SN habilitado" << std::endl;
	} else if(s_new_direction == "NS"){
		std::cout << "Sentido NS habilitado" << std::endl;
		_north.post();
		_south.wait();
		this->_direction=s_new_direction;
	} else {
		std::cout << "Ingresar: SN o NS" << std::endl; 
	}
	
	_mutex.post();
}

void ViaUnica::del(){
	if(this->_direction == "SN"){
		_north.post();
		_south.wait();
	} else if(this->_direction == "NS"){
		_south.post();
		_north.wait();
	}
	std::cout << "Borrando " << this->_mutex << std::endl;
	this->_mutex.del();
	std::cout << "Borrando " << this->_north << std::endl;
	this->_north.del();
	std::cout << "Borrando " << this->_south << std::endl;
	this->_south.del();
	std::cout << "Eliminando " << this->_memViaUnica << std::endl;
	this->_memViaUnica.del();
}
