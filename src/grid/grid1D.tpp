
#include <fstream>
#include <typeinfo>

//ne copie pas les données, pointeur NULL
template <typename T>
Grid1D<T>::Grid1D(const Grid1D<T> &grid, bool copyPointer) :
	Grid<T>(grid, copyPointer)
{
}
		
template <typename T>
Grid1D<T>::Grid1D(const std::string &src) :
	Grid<T>()
{
	std::ifstream file(src, std::ios::in);
	
	if(!file.good()) {
		log_console->errorStream() << "Failed to open file " << src << " !";
		file.close();
		exit(EXIT_FAILURE);
	}
	
	size_t typeHash;
	size_t typeNameLength;
	file.read((char*) &typeHash, sizeof(size_t)); 
	file.read((char*) &typeNameLength, sizeof(size_t)); 
	char *typeName = new char[typeNameLength+1];
	file.read(typeName, typeNameLength*sizeof(char)); 
	typeName[typeNameLength] = '\0';
	
	if(typeHash != typeid(T).hash_code()) {
		log_console->errorStream() << "Loaded a GridXD<" << typeName << "> "
			<< "file but object grid type is GridXD<" << typeid(T).name() << "> "
			<< "(from " << src << ") !";
		file.close();
		exit(EXIT_FAILURE);
	}

	file.read((char*) &this->_realWidth, sizeof(T)); 
	file.read((char*) &this->_realHeight, sizeof(T)); 
	file.read((char*) &this->_realLength, sizeof(T)); 
	file.read((char*) &this->_width, sizeof(unsigned int)); 
	file.read((char*) &this->_height, sizeof(unsigned int)); 
	file.read((char*) &this->_length, sizeof(unsigned int)); 
	file.read((char*) &this->_dh, sizeof(T)); 
	file.read((char*) &this->_dim, sizeof(unsigned int)); 
	file.read((char*) &this->_isAllocated, sizeof(bool)); 
	
	if(!file.good()) {
		log_console->errorStream() << "Failed to parse file " << src << " !";
		file.close();
		exit(EXIT_FAILURE);
	}
	
	if(this->_dim != 1u) {
		log_console->errorStream() << "Loaded a Grid" << this->_dim << "D<" << typeName << "> "
			<< "file but object grid type is Grid1D<" << typeid(T).name() << "> "
			<< "(from " << src << ") !";
		file.close();
		exit(EXIT_FAILURE);
	}
	
	if(this->_isAllocated) {
		this->_data = new T[this->size()];
		file.read((char*) this->_data, this->bytes());
		this->_isOwner = true;
	}
	else {
		this->_isOwner = false;
		this->_data = 0;
	}
	
	file.close();
}


template <typename T>
Grid1D<T>::Grid1D(T realWidth_, unsigned int width_, bool allocate_) :
	Grid<T>(realWidth_, realWidth_/width_, realWidth_/width_,
			width_, 1u, 1u, 1u)
{
	assert(realWidth_ > 0.0);
	assert(width_ > 0u);
	if(allocate_) 
		this->allocate();
}

template <typename T>
Grid1D<T>::Grid1D(T realWidth_, T dh_, bool allocate_) : 
	Grid<T>(realWidth_, dh_, dh_, dh_, 1u)
{
	assert(realWidth_ > 0.0);
	assert(dh_ > 0.0);
	if(allocate_) 
		this->allocate();
}

template <typename T>
Grid1D<T>::Grid1D(unsigned int width_, T dh_, bool allocate_) :
	Grid<T>(width_, 1u, 1u, dh_, 1u)
{
	assert(width_ > 0u);
	assert(dh_ > 0.0);
	if(allocate_) 
		this->allocate();
}

template <typename T>
Grid1D<T>::~Grid1D() {
}

template <typename T>
T Grid1D<T>::operator()(unsigned int i) const {
	return this->_data[i];
} 

template <typename T>
T& Grid1D<T>::operator()(unsigned int i) {
	return this->_data[i];
} 

