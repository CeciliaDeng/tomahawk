#ifndef TOMAHAWKOUTPUTENTRY_H_
#define TOMAHAWKOUTPUTENTRY_H_

namespace Tomahawk{
namespace IO{

#define OUTPUT_ENTRY_SIZE	(sizeof(U16) + sizeof(double) + 4*sizeof(U32) + 7*sizeof(float) + 3*sizeof(double))

#pragma pack(1)
struct TomahawkOutputEntry{
	typedef TomahawkOutputEntry self_type;

	TomahawkOutputEntry(); 	// has no ctor
	~TomahawkOutputEntry();	// has no dtor

	// Comparator function
	bool operator<(const self_type& other) const{
		if (this->AcontigID < other.AcontigID) return true;
		if (other.AcontigID < this->AcontigID) return false;

		if (this->Aposition < other.Aposition) return true;
		if (other.Aposition < this->Aposition) return false;

		if (this->BcontigID < other.BcontigID) return true;
		if (other.BcontigID < this->BcontigID) return false;

		if (this->Bposition < other.Bposition) return true;
		if (other.Bposition < this->Bposition) return false;

		return false;
	}

	// Comparator function: inverse of lesser comparator
	bool operator>(const self_type& other){ return(!((*this) < other)); }

	friend std::ostream& operator<<(std::ostream& os, const self_type& entry){
		os << (int)entry.FLAGS << '\t' << entry.MAFMix << '\t' << entry.AcontigID << '\t' << entry.Aposition << '\t' << entry.BcontigID << '\t' << entry.Bposition
				<< '\t' << entry.p1 << '\t' << entry.p2 << '\t' << entry.q1 << '\t' << entry.q2 << '\t' << entry.D << '\t' << entry.Dprime
				<< '\t' << entry.R2 << '\t' << entry.P << '\t' << entry.chiSqFisher << '\t' << entry.chiSqModel;

		return(os);
	}

	U16 FLAGS;
	double MAFMix;
	U32 AcontigID;
	U32 Amissing: 1, Aphased: 1, Aposition: 30;
	U32 BcontigID;
	U32 Bmissing: 1, Bphased: 1, Bposition: 30;
	float p1, p2, q1, q2;
	float D, Dprime;
	float R2;
	double P;
	double chiSqFisher;
	double chiSqModel;
};

// comparator functions for output entry
namespace Support{

static bool TomahawkOutputEntryCompFunc(TomahawkOutputEntry* self, TomahawkOutputEntry* other){
	if (self->AcontigID < other->AcontigID) return true;
	if (other->AcontigID < self->AcontigID) return false;

	if (self->Aposition < other->Aposition) return true;
	if (other->Aposition < self->Aposition) return false;

	if (self->BcontigID < other->BcontigID) return true;
	if (other->BcontigID < self->BcontigID) return false;

	if (self->Bposition < other->Bposition) return true;
	if (other->Bposition < self->Bposition) return false;

	return false;
}

static bool TomahawkOutputEntryCompFuncConst(const TomahawkOutputEntry* self, const TomahawkOutputEntry* other){
	if (self->AcontigID < other->AcontigID) return true;
	if (other->AcontigID < self->AcontigID) return false;

	if (self->Aposition < other->Aposition) return true;
	if (other->Aposition < self->Aposition) return false;

	if (self->BcontigID < other->BcontigID) return true;
	if (other->BcontigID < self->BcontigID) return false;

	if (self->Bposition < other->Bposition) return true;
	if (other->Bposition < self->Bposition) return false;

	return false;
}

}
}
}

#endif /* TOMAHAWKOUTPUTENTRY_H_ */