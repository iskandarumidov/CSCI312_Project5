//This is a class descigner to simulate a Philosopher
//which can store integers like id, client socket descriptor(fd), left adn right chopstick index

#ifndef _PHILOSOPHER_H_
#define _PHILOSOPHER_H_

#include <stdbool.h>

class Philosopher{
	private:
		int _id;
		int _fd;
		int _leftChopStick;
		int _rightChopStick;
		
	public:
		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^constructors^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
		Philosopher(){
			_id = 0;
			_fd = 0;
		}


		Philosopher(int i, int fd){
			_id = i;
			_fd = fd;
		}
		
		Philosopher(int i, int fd, int totalPhilosophers){
			_id = i;
			_fd = fd;
			_leftChopStick = _id;
			_rightChopStick = (_id + 1) % totalPhilosophers;
		}
		
		//destructors
		virtual ~Philosopher(){
			_id = 0;
			_fd = 0;
		}

		//^^^^^^^^^^^^^^^^^^^^^acessors and mutators^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^///
		void setId(int i){
			_id = i;
		}

		int getId(){
			return _id;
		}
		
		void setFd(int fd){
			_fd = fd;
		}

		int getFd(){
			return _fd;
		}
		
		
		int getLeftChopStick(){
			return _leftChopStick;
		}
		
		int getRightChopStick(){
			return _rightChopStick;
		}

/*		
		void setLeftChopStick(ChopStick l){
			
		}
		
		void setRightChopStick(ChopStick r){
			
		}
*/		
		
};
#endif
