
#ifndef DOMAIN_H
#define DOMAIN_H

#include <vector>

#include "subDomain.hpp"

class Domain {

	public: 
		Domain(unsigned int domainWidth, unsigned int domainHeight, unsigned int domainLength,
			unsigned int extraBorderSize, 
			unsigned int minSplits);

		~Domain();

		unsigned int domainWidth() const;
		unsigned int domainHeight() const;
		unsigned int domainLength() const;

		unsigned int extraBorderSize() const;

		unsigned int nSplits() const;
		unsigned int splitsX() const;
		unsigned int splitsY() const;
		unsigned int splitsZ() const;

		SubDomain *operator()(unsigned int i, unsigned int j, unsigned int k);
		SubDomain *operator[](unsigned int n);

	private:
		unsigned int _domainWidth, _domainHeight, _domainLength;
		unsigned int _extraBorderSize;

		unsigned int _nSplits;
		unsigned int _splitsX, _splitsY, _splitsZ;

		std::vector<SubDomain*> _subDomains;

		void splitDomain(unsigned int minSplits);
		void linkSubDomains();
};

#endif /* end of include guard: DOMAIN_H */