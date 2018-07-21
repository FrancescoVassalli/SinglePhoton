#ifndef Track_h
#define Track_h 

#include <TVector3.h>
#include <TLorentzVector.h>

struct Hits
{
	int total;
	int map;
	int intt;
	int tpc;
	Hits(){}
	Hits(int t, int m, int i, int tp) : total(t),map(m), intt(i), tpc(tp){}
};


class Track
{
public:
	Track(){}
	~Track(){}
	inline int getEvent()const {return event;}
	inline int getCharge()const {return charge;}
	inline int getID(bool truth)const {
		if (truth) return tID;
		else return rID;
	}
	inline float getQuality()const {return quality;}
	inline Hits getHits(bool truth)const {
		if(truth) return thits;
		else return rhits;
	}
	inline TVector3 getCartesianVec(bool truth)const {
		if(truth) return tcartesianVec;
		else return rcartesianVec;
	}
	inline TVector3 getPolarVec(bool truth)const {
		if(truth) return tpolarVec;
		else return rpolarVec;
	}
	inline TLorentzVector getVertex()const {return vertex;}
	inline void setEvent(int e){
		event=e;
	}
	inline void setID(int id, bool truth){
		if(truth) tID=id;
		else rID=id;
	}
	inline void setQuality(float q){
		quality=q;
	}
	inline void setHits(Hits h, bool truth){
		if(truth) thits=h;
		else rhits=h;
	}
	inline void setHits(int total, int map, int intt, int tpc, bool truth){
		Hits h(total,map,intt,tpc);
		if(truth) thits=h;
		else rhits=h;
	}
	inline void setCartesianVec(TVector3 v, bool truth){
		if(truth) tcartesianVec=v;
		else rcartesianVec=v;
	}
	inline void setCartesianVec(float x, float y, float z, bool truth){
		TVector3 v(x,y,z);
		if(truth) tcartesianVec=v;
		else rcartesianVec=v;
	}
	inline void setPolarVec(TVector3 v, bool truth){
		if(truth) tpolarVec=v;
		else rpolarVec=v;
	}
	inline void setPolarVec(float x, float y, float z, bool truth){
		TVector3 v(x,y,z);
		if(truth) tpolarVec=v;
		else rpolarVec=v;
	}
	inline void setVertex(TLorentzVector t){
		vertex=t;
	}
	inline void setVertex(float x, float y, float z, float t){
		vertex=TLorentzVector(x,y,z,t);
	}
	inline void setCharge(int t){
		charge=t;
	}

private:
	int event;

	int rID;
	int charge;
	float quality;
	Hits rhits;
	TVector3 rcartesianVec;
	TVector3 rpolarVec;

	int tID;
	int tflavor;
	Hits thits;
	TVector3 tcartesianVec;
	TVector3 tpolarVec;
	TLorentzVector vertex;
	
};
#endif //Track_h