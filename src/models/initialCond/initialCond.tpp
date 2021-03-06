
#include "initialCond.hpp"
		
template <typename T>
std::mutex InitialCond<T>::mutex;

template <typename T>
std::condition_variable InitialCond<T>::cond;

template <typename T>
InitialCond<T>::InitialCond(bool lock):
	_lock(lock)
{
}

template <typename T>
InitialCond<T>::~InitialCond() {
}

template <typename T>
void InitialCond<T>::initializeGrid(Grid<T> *grid) const {
	initializeSubGrid(
			grid->data(), 
			0u,0u,0u, 
			grid->width(), grid->height(), grid->length(),
			grid->width(), grid->height(), grid->length());
}

template <typename T>
void InitialCond<T>::initializeSubGrid(
		T *data, 
		unsigned int offsetX, unsigned int offsetY, unsigned int offsetZ,
		unsigned int subGridWidth, unsigned int subGridHeight, unsigned int subGridLength,
		unsigned int gridWidth, unsigned int gridHeight, unsigned int gridLength) const
{
		
	T x,y,z;

	if(_lock) {
		std::unique_lock<std::mutex> lock(mutex);
		for (unsigned int k = 0; k < subGridLength; k++) {
			for (unsigned int j = 0; j < subGridHeight; j++) {
				for (unsigned int i = 0; i < subGridWidth; i++) {
					unsigned long offset = 0ul
						+ k*subGridHeight*subGridWidth 
						+ j*subGridWidth 
						+ i;

					x = (gridWidth -1u==0u ? T(0.5) : (T)(offsetX + i) / (T)(gridWidth -1));
					y = (gridHeight-1u==0u ? T(0.5) : (T)(offsetY + j) / (T)(gridHeight-1));
					z = (gridLength-1u==0u ? T(0.5) : (T)(offsetZ + k) / (T)(gridLength-1));

					data[offset] = F(x,y,z);
				}
			}
		}	
		cond.notify_one();
	}
	else {
		for (unsigned int k = 0; k < subGridLength; k++) {
			for (unsigned int j = 0; j < subGridHeight; j++) {
				for (unsigned int i = 0; i < subGridWidth; i++) {
					unsigned long offset = 0ul
						+ k*subGridHeight*subGridWidth 
						+ j*subGridWidth 
						+ i;

					x = (gridWidth -1u==0u ? T(0.5) : (T)(offsetX + i) / (T)(gridWidth -1));
					y = (gridHeight-1u==0u ? T(0.5) : (T)(offsetY + j) / (T)(gridHeight-1));
					z = (gridLength-1u==0u ? T(0.5) : (T)(offsetZ + k) / (T)(gridLength-1));

					data[offset] = F(x,y,z);
				}
			}
		}	
	}
}
	
	//DEBUG
	//if(i==0 && j==0 && k==0) {
		//log_console->debugStream() << "Initialize grid offset=" << offset << " , grid size=" 
			//<< utils::toStringDimension(gridWidth,gridHeight,gridLength) << " subgrid size="
			//<< utils::toStringDimension(subGridWidth,subGridHeight,subGridLength) << " !";
	//}
	//log_console->infoStream() << offset << "\t" << (data+offset) << "\t" << F(x,y,z);
	//log_console->debugStream() << F(0,0,0.5)   << " " <<  F(0.1,0,0.5) << " " 
							   //<< F(0.2,0,0.5) << " " << F(0.3,0,0.5) << " "
							   //<< F(0.4,0,0.5) << " " << F(0.5,0,0.5) << " "
							   //<< F(0.6,0,0.5) << " " << F(0.7,0,0.5) << " "
							   //<< F(0.8,0,0.5) << " " << F(0.9,0,0.5) << " "
							   //<< F(1.0,0,0.5);
