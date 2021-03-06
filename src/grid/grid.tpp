
#include <cassert>
#include <typeinfo>
#include <fstream>

#include "log.hpp"
#include "utils.hpp"

using namespace log4cpp;

template <typename T>
Grid<T>::Grid() :
	_realWidth(0), _realHeight(0), _realLength(0),
	_width(0), _height(0), _length(0),
	_dh(0),
	_dim(0),
	_isAllocated(false),
	_isOwner(false),
	_data(0)
{
}
		
template <typename T>
Grid<T>::Grid(const std::string &src)
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
		log_console->errorStream() << "Loaded a Grid<" << typeName << "> "
			<< "file but object grid type is Grid<" << typeid(T).name() << "> "
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
	
	if(this->_isAllocated) {
		this->_data = new T[this->size()];
		this->_isOwner = true;
		file.read((char*) this->_data, this->bytes());
	}
	else {
		_isOwner = false;
		_data = 0;
	}
	
	file.close();
}


template <typename T>
Grid<T>::Grid(const Grid<T> &grid, bool copyPointer) :
	_realWidth(grid.realWidth()), _realHeight(grid.realHeight()), _realLength(grid.realLength()),
	_width(grid.width()), _height(grid.height()), _length(grid.length()),
	_dh(grid.dh()),
	_dim(grid.dim()),
	_isAllocated(copyPointer ? grid.isAllocated() : false),
	_isOwner(false),
	_data(copyPointer ? grid.data() : 0)
{
}

template <typename T>
Grid<T>::Grid(T realWidth_, T realHeight_, T realLength_,
		unsigned int width_, unsigned int height_, unsigned int length_,
		unsigned int dim_) :
	_realWidth(realWidth_), _realHeight(realHeight_), _realLength(realLength_),
	_width(width_), _height(height_), _length(length_),
	_dh(realWidth_/width_),
	_dim(dim_),
	_isAllocated(false),
	_isOwner(false),
	_data(0)
{
	assert(    (_dim == 1 && _width > 1 && _height == 1 && _length == 1)
			|| (_dim == 2 && _width > 1 && _height > 1 && _length == 1)
			|| (_dim == 3 && _width > 1 && _height > 1 && _length > 1));
}

template <typename T>
Grid<T>::Grid(T realWidth_, T realHeight_, T realLength_,
		T dh_, unsigned int dim_) :
	_realWidth(realWidth_), _realHeight(realHeight_), _realLength(realLength_),
	_width(realWidth_/dh_), _height(realHeight_/dh_), _length(realLength_/dh_),
	_dh(dh_),
	_dim(dim_),
	_isAllocated(false),
	_isOwner(false),
	_data(0)
{
	assert(    (_dim == 1 && _width > 1 && _height == 1 && _length == 1)
			|| (_dim == 2 && _width > 1 && _height > 1 && _length == 1)
			|| (_dim == 3 && _width > 1 && _height > 1 && _length > 1));
}

template <typename T>
Grid<T>::Grid(unsigned int width_, unsigned int height_, unsigned int length_,
		T dh_, unsigned int dim_) :
	_realWidth(width_*dh_), _realHeight(height_*dh_), _realLength(length_*dh_),
	_width(width_), _height(height_), _length(length_),
	_dh(dh_),
	_dim(dim_),
	_isAllocated(false),
	_isOwner(false),
	_data(0)
{
	assert(    (_dim == 1 && _width > 1 && _height == 1 && _length == 1)
			|| (_dim == 2 && _width > 1 && _height > 1 && _length == 1)
			|| (_dim == 3 && _width > 1 && _height > 1 && _length > 1));
}

template <typename T>
Grid<T>::~Grid() {
	free();
}

template <typename T>
T Grid<T>::realWidth() const {
	return _realWidth;
}
template <typename T>
T Grid<T>::realHeight() const {
	return _realHeight;
}
template <typename T>
T Grid<T>::realLength() const {
	return _realLength;
}

template <typename T>
unsigned int Grid<T>::width() const {
	return _width;
}
template <typename T>
unsigned int Grid<T>::height() const {
	return _height;
}
template <typename T>
unsigned int Grid<T>::length() const {
	return _length;
}

template <typename T>
unsigned int Grid<T>::dim() const {
	return _dim;
}
template <typename T>
bool Grid<T>::isAllocated() const {
	return _isAllocated;
}
template <typename T>
bool Grid<T>::isOwner() const {
	return _isOwner;
}
template <typename T>
T * Grid<T>::data() const {
	return _data;
}
template <typename T>
T Grid<T>::dh() const {
	return _dh;
}

template <typename T>
unsigned long Grid<T>::size() const {
	return (unsigned long)_width*_height*_length;
}

template <typename T>
unsigned long Grid<T>::bytes() const {
	return (unsigned long)_width*_height*_length*sizeof(T);
}

template <typename T>
T Grid<T>::operator()(unsigned int i, unsigned int j, unsigned int k) const {
	return _data[k*_height*_width+j*_width+i];
} 

template <typename T>
T& Grid<T>::operator()(unsigned int i, unsigned int j, unsigned int k) {
	return _data[k*_height*_width+j*_width+i];
} 

template <typename T>
T Grid<T>::operator[](unsigned long n) const {
	return _data[n];
}

template <typename T>
T& Grid<T>::operator[](unsigned long n) {
	return _data[n];
}

		
template <typename T>
void Grid<T>::free() {
	assert(!(_data == 0 && _isAllocated));
	assert(!(_data == 0 && _isOwner));

	if(_isAllocated) {
		if(_isOwner) 
			delete [] _data;
		_data = 0;
		_isAllocated = false;
		_isOwner = false;
	}
}
		
template <typename T>
void Grid<T>::setData(T* data_, bool isOwner_) {
	if(_isOwner && _isAllocated)
		log_console->warnStream() << "Setting data to a grid that was already allocated and data owner !";

	this->free();

	_data = data_;
	if(_data!=0) {
		_isOwner = isOwner_;
		_isAllocated = true;
	}
	else {
		_isOwner = false;
		_isAllocated = false;
	}
}

template <typename T>
void Grid<T>::allocate() {
	assert(_data == 0);
	assert(!_isAllocated);
	assert(!_isOwner);

	_data = new T[this->size()];
		
	if(this->_data == 0) {
		log_console->errorStream() << "Failed to allocate grid !";
		exit(EXIT_FAILURE);
	}

	_isAllocated = true;
	_isOwner = true;
}
		
template <typename T>
Grid<T> *Grid<T>::clone() const {
	Grid<T> *grid = new Grid<T>(*this, true);	
	return grid;
}

template <typename T>
Grid<T> *Grid<T>::cloneAttribs() const {
	Grid<T> *grid = new Grid<T>(*this, false);	
	return grid;
}

template <typename T>
Grid<T> *Grid<T>::cloneData() const {
	Grid<T> *grid = new Grid<T>(*this, false);	
	grid->allocate();
	memcpy(grid->data(), this->data(), this->bytes());
	return grid;
}
		
template <typename T>
void Grid<T>::exportGrid(const std::string &dst) {
	std::ofstream file(dst, std::ios::out | std::ios::trunc);

	if(!file.good()) {
		log_console->errorStream() << "Failed to open file " << dst << " !";
		file.close();
		exit(EXIT_FAILURE);
	}
	
	size_t typeHash = typeid(T).hash_code();
	size_t typeNameLength = strlen(typeid(T).name());
	file.write((char*) &typeHash, sizeof(size_t)); 
	file.write((char*) &typeNameLength, sizeof(size_t)); 
	file.write(typeid(T).name(), typeNameLength*sizeof(char)); 
	file.write((char*) &this->_realWidth, sizeof(T)); 
	file.write((char*) &this->_realHeight, sizeof(T)); 
	file.write((char*) &this->_realLength, sizeof(T)); 
	file.write((char*) &this->_width, sizeof(unsigned int)); 
	file.write((char*) &this->_height, sizeof(unsigned int)); 
	file.write((char*) &this->_length, sizeof(unsigned int)); 
	file.write((char*) &this->_dh, sizeof(T)); 
	file.write((char*) &this->_dim, sizeof(unsigned int)); 
	file.write((char*) &this->_isAllocated, sizeof(bool)); 
	if(this->_isAllocated)
		file.write((char*) _data, this->bytes());
	
	if(!file.good()) {
		log_console->errorStream() << "Failed to write file " << dst << " !";
		file.close();
		exit(EXIT_FAILURE);
	}

	file.close();
}

template <typename T>
void Grid<T>::exportData(const std::string &dst) {
	std::ofstream file(dst, std::ios::out | std::ios::trunc);

	if(!file.good()) {
		log_console->errorStream() << "Failed to open file " << dst << " !";
		file.close();
		exit(EXIT_FAILURE);
	}
	
	assert(this->_isAllocated);

	for (unsigned int j = 0; j < this->height(); j++) {
		for (unsigned int i = 0; i < this->width(); i++) {
			file << "\t" << (*this)(i,j,0);
		}
		file << "\n";
	}

	if(!file.good()) {
		log_console->errorStream() << "Failed to write file " << dst << " !";
		file.close();
		exit(EXIT_FAILURE);
	}

	file.close();
}

template <typename T>
std::ostream & operator <<(std::ostream &out, const Grid<T> &grid) {
	out << "Grid<T=" << typeid(T).name() << ">"
		<< "\n\t Dim               : " << grid.dim() << "D"
		<< "\n\t Real size (WxHxL) : " << grid.realWidth() << " x " << grid.realHeight() << " x " << grid.realLength()
		<< "\n\t Size      (WxHxL) : " << grid.width() << " x " << grid.height() << " x " << grid.length()
		<< "\n\t DH                : " << grid.dh()
		<< "\n\t Is allocated ?    : " << (grid.isAllocated() ? "true" : "false")
		<< "\n\t Elements          : " << grid.size()
		<< "\n\t Memory size       : " << utils::toStringMemory(grid.bytes());

	return out;
}
		
		
