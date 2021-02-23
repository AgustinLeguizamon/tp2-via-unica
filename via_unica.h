#include <iostream>
#include "Sem-sv/sv_sem.h"
#include "Sem-sv/sv_shm.h"

#ifndef VIAUNICA_H
#define VIAUNICA_H

class trains{
	private:
		int _inside;
		int _queued_north;
		int _queued_south;
	
	public:

		void init(int north=0, int south=0, int inside=0){
			_queued_north=north; _queued_south=south; _inside=inside;
			cout<<"trenes inicializado"<<north<<","<<south<<","<<inside<<endl;
		}

		void queueNorth(){
			_queued_north++;
		}

		void queueSouth(){
			_queued_south++;
		}

		void enterNorth(){
			_queued_north--;
			_inside++;
		}

		void enterSouth(){
			_queued_south--;
			_inside++;
		}

		void out(){
			_inside--;
		}

		int getInside(){
			return _inside;
		}

		int getQueuedNorth(){
			return _queued_north;
		}

		int getQueuedSouth(){
			return _queued_south;
		}

		friend ostream& operator<<(ostream& os, trains& t){
			return os << "(" << t._queued_north << "," << t._queued_south << "," << t._inside;
		}
	
};

class ViaUnica {
	private:
		std::string _key;
		
		sv_sem _mutex;
		sv_sem _north;
		sv_sem _south;
		
		sv_shm _memViaUnica;
		
		trains* _p_trains;
		std::string _direction;
	public:
		ViaUnica(std::string key="via_unica_1", bool init=false);

		void inNorth();

		void outSouth();

		void inSouth();

		void outNorth();

		void changeDirection(std::string s_new_direction);

		void del();

		friend ostream& operator<<(ostream& os, ViaUnica& via){
			return os<<"("<<via._key<<", counter="<<via._p_trains->getInside()<<", mutex= "<<via._mutex<<")"<<endl;
		}

		private:
		void _openNorth();

		void _openSouth();
};

#endif
