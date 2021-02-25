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
	
	if(_p_trains->isNorthOpen() && !_p_trains->isOppositeTrainsStillInside()){
		/*abro uno solo, solo funciona si entra de a uno
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

	/*soy el ultimo y hubo un cambio de direccion*/
	if(_p_trains->getInside() == 0 && _p_trains->isSouthOpen()){
		/*Dado el previo cambio de direccion 
		estoy seguro de que no hay mas trenes ni los va a haber del otro lado*/
		_p_trains->isOppositeTrainsStillInside(_FALSE);
				
		const int queued_south = _p_trains->getQueuedSouth();
		for(int i=0 ; i < queued_south; i++){
			_openSouth();
		}		
	}
	
	std::cout << "Salio sur - trenes adentro" << _p_trains->getInside() << std::endl;
	_mutex.post();
}


void ViaUnica::inSouth(){
	_mutex.wait();
	_p_trains->queueSouth();
	std::cout << "En cola sur:" << _p_trains->getQueuedSouth() << std::endl;
		
	if(_p_trains->isSouthOpen() && !_p_trains->isOppositeTrainsStillInside()){
		_openSouth();
	}
	
	_mutex.post();
	_south.wait();
}

void ViaUnica::outNorth(){
	_mutex.wait();
	std::cout << "TOME MUTEX" << std::endl;
	_p_trains->out();

	/*Si es el ultimo y resulta que cambie de direccion es porque hay del otro lado esperando a entrar*/
	if(_p_trains->getInside() == 0 && _p_trains->isNorthOpen()){
		/*habilito la barrera norte*/
		std::cout << "SALIO EL ULTIMO POR EL NORTE" << std::endl;
		_p_trains->isOppositeTrainsStillInside(_FALSE);
		/*Luego hago pasar a todos los trenes encolados*/
		const int queued_north = _p_trains->getQueuedNorth();
		for(int i=0 ; i < queued_north; i++){
			_openNorth();
		}	
	}

	std::cout << "Salio norte - trenes adentro: " << _p_trains->getInside() << std::endl;	
	_mutex.post();
}

void ViaUnica::changeDirection(const std::string& s_new_direction){
	//_mutex.wait();
	if(s_new_direction.compare("SN") == 0){
		std::cout << "Realizando cambio de via" << std::endl;
		
		if(_p_trains->getInside() != 0 && _p_trains->isNorthOpen()){
			std::cout << "Sentido SN habilitado pero a la espera de que salgan los trenes del Norte" << std::endl;
			_p_trains->isOppositeTrainsStillInside(_TRUE);
			_p_trains->setDirection(SN);
			
		} else {
			std::cout << "Sentido SN habilitado" << std::endl;
			/*En el caso de que haya adentro y esten en el mismo sentido, no pasa nada*/
			_p_trains->setDirection(SN);
			/*si hay encolados, los hago entrar*/
			const int queued_south = _p_trains->getQueuedSouth();
			for(int i=0 ; i < queued_south; i++){
				_openSouth();
			}
		}

	} else if(s_new_direction.compare("NS") == 0){
		std::cout << "Realizando cambio de via" << std::endl;
		
		if(_p_trains->getInside() != 0 && _p_trains->isSouthOpen()) {
			std::cout << "Sentido NS habilitado pero a la espera de que salgan los trenes del Sur" << std::endl;
			_p_trains->isOppositeTrainsStillInside(_TRUE);
			_p_trains->setDirection(NS);
			
		} else {
			std::cout << "Sentido NS habilitado" << std::endl;
			_p_trains->setDirection(NS);
			const int queued_north = _p_trains->getQueuedNorth();
			for(int i=0 ; i < queued_north; i++){
				_openNorth();
			}	
		}
		
		
	} else {
		std::cout << "Ingresar: SN o NS" << std::endl; 
	}
	
	//_mutex.post();
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
