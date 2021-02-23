#include <iostream>
#include "Sem-sv/sv_sem.h"
#include "Sem-sv/sv_shm.h"

#ifndef VIAUNICA_H
#define VIAUNICA_H

typedef struct {int n;} counter;

class ViaUnica {
	private:
		std::string _key;
		sv_sem _mutex;
		sv_sem _north;
		sv_sem _south;
		sv_shm _memViaUnica;
		counter* _p_counter;
		std::string _direction;
	public:
		ViaUnica(std::string key="via_unica", bool init=false);

		void inNorth();

		void outSouth();

		void inSouth();

		void outNorth();

		void changeDirection(std::string s_new_direction);

		void del();

		friend ostream& operator<<(ostream& os, ViaUnica& via){
			return os<<"("<<via._key<<", counter="<<via._p_counter->n<<", mutex= "<<via._mutex<<")"<<endl;
		}
};

#endif
