
#ifndef INC_SINGLETON
#define INC_SINGLETON


#include <cassert>


namespace Cistron {


template<typename T>
class CSingleton {

	public:

		// constructor
		CSingleton() {
			assert(!fSingleton);
			int offset = (int)(T*)1 - (int)(CSingleton<T>*)(T*)1;
			fSingleton = (T*)((int)this + offset);
		};


		// destructor
		virtual ~CSingleton() {
			assert(fSingleton);
			fSingleton = 0;
		}


		// get singleton
		inline static T& getSingleton() {
			assert(fSingleton);
			return *fSingleton;
		}
		inline static T* getSingletonPtr() {
			assert(fSingleton);
			return fSingleton;
		}

	private:

		// the singleton
		static T* fSingleton;

};


template <typename T> T* CSingleton<T>::fSingleton = 0;


};



#endif
