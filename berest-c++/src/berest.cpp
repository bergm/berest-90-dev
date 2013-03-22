#include <cmath>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <numeric>

#include "boost/foreach.hpp"
//#include "boost/fusion/adapted/struct/adapt_struct.hpp"
//#include "boost/fusion/include/adapt_struct.hpp""
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include "berest.h"
#include "helper.h"

using namespace mongo;
using namespace Tools;
using namespace Berest;
using namespace std;

Bodenvektor Berest::operator*(const Bodenvektor& v1, const Bodenvektor& v2)
{
	Bodenvektor res;
	for(int i=0, size=v1.size(); i<size; i++)
		res[i] = v1.at(i)*v2.at(i);
	return res;
}

double Berest::sum(const Bodenvektor& v)
{
	return accumulate(v.begin(), v.end(), double(0));
}

template<typename T>
int round(T value)
{
	return std::floor(value + 0.5);
}

template<class Container>
vector<typename Container::value_type> sub(const Container& v, int start, int length)
{
	return vector<typename Container::value_type>(v.begin()+start, v.begin()+start+length);
}

DBClientConnection& Berest::dbConnection()
{
	static DBClientConnection c;
	static bool connected = false;
	if(!connected)
	{
		c.connect("localhost");
		connected = true;
	}
	return c;
}

pair<BtType, bool> Berest::btTypLesen(string btkbz)
{
	DBClientConnection& c = dbConnection();

	auto cur = c.query("berest.btrecs", QUERY("btkbz" << btkbz), 1);
	if(!cur->more())
		return error<BtType>();
	auto bt = cur->next();
	BtType btrec;
	btrec.btkbz = bt["btkbz"].str();
	btrec.anzMest = bt["anzMest"].numberInt();
	btrec.bzNr = bt["bzNr"].str();
	btrec.krNr = bt["krNr"].str();
	btrec.raNr = bt["raNr"].str();

	return success(btrec);
}

void Berest::btTypSpeichern(BtType b, bool isNew)
{
	DBClientConnection& c = dbConnection();

	BSONObj obj = BSON("btkbz" << b.btkbz <<
										 "anzMest" << b.anzMest <<
										 "bzNr" << b.bzNr <<
										 "krNr" << b.krNr <<
										 "raNr" << b.raNr);

	if(isNew)
		c.insert("berest.btrecs", obj);
	else
		c.update("berest.btrecs", QUERY("btkbz" << b.btkbz), obj, true);
}

pair<Schlagdaten, bool> Berest::schlagLesen(string schlagnummer)
{
	DBClientConnection& c = dbConnection();

	auto cur = c.query("berest.schlags", QUERY("schlagnummer" << schlagnummer), 1);
	if(!cur->more())
		return error<Schlagdaten>();

	auto s = cur->next();
	Schlagdaten schlag;
	schlag.schlagnummer = s["schlagnummer"].str();
	schlag.faFlaeche = s["faFlaeche"].numberDouble();
	schlag.beFlaeche = s["beFlaeche"].numberDouble(),
	schlag.nrNieMst = s["nrNieMst"].numberInt();
	schlag.faNummer = s["faNummer"].str();
	schlag.faAf = s["faAf"].str();
	schlag.faVz = s["faVz"].str();
	schlag.symbol = s["symbol"].str();
	schlag.stTyp = s["stTyp"].numberInt();
	schlag.hangN = s["hangN"].numberInt();
	copy(s["dicke"], schlag.dicke3);
	copy(s["fk"], schlag.fk3);
	copy(s["pwp"], schlag.pwp3);
	schlag.knownDCDay = s["knownDCDay"].numberInt();
	schlag.knownDC = s["knownDC"].numberInt();
//	schlag.anzGaben = s["anzGaben"].numberInt();
//	fillMap(schlag.transaction2info, s["transaction2info"].Obj());

	//setzt auch lambda, aber noch ohne korrektur
	schlag.setup();
	
	return success(schlag);
}

void Berest::schlagSpeichern(const Schlagdaten& s, bool isNew)
{
	DBClientConnection& c = dbConnection();

	BSONObj obj = BSON("schlagnummer" << s.schlagnummer <<
										 "faFlaeche" << s.faFlaeche <<
										 "beFlaeche" << s.beFlaeche <<
										 "nrNieMst" << s.nrNieMst <<
										 "faNummer" << s.faNummer <<
										 "faAf" << s.faAf <<
										 "faVz" << s.faVz <<
										 "symbol" << s.symbol <<
										 "stTyp" << s.stTyp <<
										 "hangN" << s.hangN <<
										 "dicke" << toBSONArray(s.dicke3) <<
										 "fk" << toBSONArray(s.fk3) <<
										 "pwp" << toBSONArray(s.pwp3) <<
										 "knownDCDay" << s.knownDCDay <<
										 "knownDC" << s.knownDC);// <<
//										 "anzGaben" << s.anzGaben <<
//										 "transaction2info" << toBSONObject(s.transaction2info));// <<

	if(isNew)
		c.insert("berest.schlags", obj);
	else
		c.update("berest.schlags", QUERY("schlagnummer" << s.schlagnummer), obj, true);
}

pair<WiDatTyp, bool> Berest::wiDatLesen(string faNrAfVz)
{
	DBClientConnection& c = dbConnection();

	auto cur = c.query("berest.widats", QUERY("faNrAfVz" << faNrAfVz), 1);
	if(!cur->more())
		return error<WiDatTyp>();
	auto wd = cur->next();
	WiDatTyp wiDat;
	wiDat.faNrAfVz = wd["faNrAfVz"];
	fillMap(wiDat.dc2day, wd["dc2day"].Obj());//, "dc", "day");
	fillMap(wiDat.dc2name, wd["dc2name"].Obj());//, "dc", "name");
	fillMap(wiDat.day2bd, wd["day2bd"].Obj());//, "day", "value");
	fillMap(wiDat.day2n, wd["day2n"].Obj());//, "day", "value");
	fillMap(wiDat.day2t, wd["day2t"].Obj());//, "day", "value");
	fillMap(wiDat.day2qu, wd["day2qu"].Obj());//, "day", "value");
	fillMap(wiDat.day2ef, wd["day2ef"].Obj());//, "day", "value");

	return success(wiDat);
}

void Berest::wiDatSpeichern(const WiDatTyp& wd, bool isNew)
{
	DBClientConnection& c = dbConnection();

	BSONObj obj = BSON("faNrAfVz" << wd.faNrAfVz <<
										 "dc2day" << toBSONObject(wd.dc2day) <<
										 "dc2name" << toBSONObject(wd.dc2name) <<
										 "day2bd" << toBSONObject(wd.day2bd) <<
										 "day2n" << toBSONObject(wd.day2n) <<
										 "day2t" << toBSONObject(wd.day2t) <<
										 "day2qu" << toBSONObject(wd.day2qu) <<
										 "day2ef" << toBSONObject(wd.day2ef));

	if(isNew)
		c.insert("berest.widats", obj);
	else
		c.update("berest.widats", QUERY("faNrAfVz" << wd.faNrAfVz), obj, true);
}

pair<BMTyp, bool> Berest::bodenfeuchteLesen(int snru, int tag)
{
	DBClientConnection& c = dbConnection();

	//BMTyp füllen für
	auto cur = c.query("berest.feuchtes",
										tag ? QUERY("snru" << snru << "tag" << tag)
												: QUERY("snru" << snru),
										1);
	if(!cur->more())
		return error<BMTyp>();
	auto b = cur->next();
	BMTyp bm;
	bm.snru = b["snru"].numberInt();
	bm.snrus = b["snru"].str();
	bm.tag = b["tag"].numberInt();
	bm.dc = b["dc"].numberInt();
	bm.flaeche = b["flaeche"].numberDouble();
	bm.grws = b["grws"].numberDouble();
	bm.einheit = CodeEinheit(b["einheit"].numberInt());
	fillMap(bm.dicke2wert, b["dicke2wert"].Obj());//, "dicke", "wert");

	return success(bm);
}

void Berest::bodenfeuchteSpeichern(BMTyp bm, bool isNew)
{
	DBClientConnection& c = dbConnection();

	BSONObj obj = BSON("snru" << bm.snru <<
										 "snrus" << bm.snrus <<
										 "tag" << bm.tag <<
										 "dc" << bm.dc <<
										 "flaeche" << bm.flaeche <<
										 "grws" << bm.grws <<
										 "einheit" << int(bm.einheit) <<
										 "dicke2wert" << toBSONObject(bm.dicke2wert));

	if(isNew)
		c.insert("berest.feuchtes", obj);
	else
		c.update("berest.feuchtes", QUERY("snru" << bm.snru << "tag" << bm.tag), obj, true);
}

double awcPercent(int fromLayer, int toLayer, const Bodenvektor& soilMoisture,
									const Bodenvektor& fk, const Bodenvektor& pwp);
Bodenfeuchte createBF(int snru, int day,
											const Bodenvektor& soilMoisture,
											const Bodenvektor& fk,
											const Bodenvektor& pwp)
{
	Bodenfeuchte bf;
	bf.snru = snru;
	bf.tag = day;
	bf.pNFK_0_30cm = awcPercent(1, 2, soilMoisture, fk, pwp);
	bf.pNFK_30_60cm = awcPercent(3, 3, soilMoisture, fk, pwp);
	bf.pNFK_0_60cm = awcPercent(1, 3, soilMoisture, fk, pwp);
	return bf;
}

pair<Bodenfeuchte, bool> Berest::aktPrognBodenfeuchteLesen
(int snru, int tag, bool readAktFeuchte)
{
	DBClientConnection& c = dbConnection();

	//BMTyp füllen für
	auto cur = c.query(readAktFeuchte ? "berest.aktFeuchte" : "berest.prognFeuchte",
										tag ? QUERY("snru" << snru << "tag" << tag)
												: QUERY("snru" << snru),
										1);
	if(!cur->more())
		return error<Bodenfeuchte>();
	auto b = cur->next();
	Bodenfeuchte bf;
	bf.snru = b["snru"].numberInt();
	bf.tag = b["tag"].numberInt();
	bf.pNFK_0_30cm = b["pNFK_0_30cm"].numberDouble();
	bf.pNFK_30_60cm = b["pNFK_30_60cm"].numberDouble();
	bf.pNFK_0_60cm = b["pNFK_0_60cm"].numberDouble();

	return success(bf);
}



void aktPrognBFSpeichern(Bodenfeuchte bf,
												 bool writeAktFeuchte, bool isNew)
{
	DBClientConnection& c = dbConnection();

	BSONObj obj = BSON("snru" << bf.snru <<
										 "tag" << bf.tag <<
										 "pNFK_0_30cm" << bf.pNFK_0_30cm <<
										 "pNFK_30_60cm" << bf.pNFK_30_60cm <<
										 "pNFK_0_60cm" << bf.pNFK_0_60cm);

	if(isNew)
		c.insert(writeAktFeuchte ? "berest.aktFeuchte" : "berest.prognFeuchte", obj);
	else
		c.update(writeAktFeuchte ? "berest.aktFeuchte" : "berest.prognFeuchte",
						 QUERY("snru" << bf.snru << "tag" << bf.tag), obj, true);
}

void Berest::prognostischeBFSpeichern(Bodenfeuchte bf, bool isNew)
{
	aktPrognBFSpeichern(bf, false, isNew);
}

void Berest::aktuelleBFSpeichern(Bodenfeuchte bf, bool isNew)
{
	aktPrognBFSpeichern(bf, true, isNew);
}

vector<EMType1> Berest::gabenLesen(int snru)
{
	DBClientConnection& c = dbConnection();

	vector<EMType1> gs;

	//EMType1 füllen für
	auto cur = c.query("berest.gaben", QUERY("snru" << snru));//, 1);
	while(cur->more())
	{
		auto g = cur->next();
		EMType1 em;
		em.snru = g["snru"].numberInt();
//		em.code = g["code"].numberInt();
		em.beginn = g["beginn"].numberInt();
		em.ende = g["ende"].numberInt();
//		em.kenn = g["kenn"].str();
//		em.arbart = g["arbart"].numberInt();
		em.arbart = 351;
		em.beafl = g["beafl"].numberDouble();
//		em.material = g["material"].numberInt();
		em.material = 1;
		em.menge = g["menge"].numberInt();
//		em.qunote = g["qunote"].numberInt();

		gs.push_back(em);
	}

	return gs;
}

void Berest::gabeSpeichern(const EMType1& g, bool isNew)
{
	DBClientConnection& c = dbConnection();

	BSONObj obj = BSON("snru" << g.snru <<
//										 "code" << g.code <<
										 "beginn" << g.beginn <<
										 "ende" << g.ende <<
//										 "kenn" << g.kenn <<
//										 "arbart" << g.arbart <<
										 "beafl" << g.beafl <<
//										 "material" << g.material <<
										 "menge" << g.menge);// <<
//										 "qunote" << g.qunote);

	if(isNew)
		c.insert("berest.gaben", obj);
	else
		c.update("berest.gaben", QUERY("snru" << g.snru <<
																	 "beginn" << g.beginn <<
																	 "ende" << g.ende), obj, true);
}

EMType1 Berest::parseIrrigationDonation(string data)
{
	using boost::spirit::qi::double_;
	using boost::spirit::qi::int_;
	using boost::spirit::qi::_1;
	using boost::spirit::qi::phrase_parse;
	using boost::spirit::qi::lit;
	using boost::spirit::ascii::space;
	using boost::phoenix::ref;

	EMType1 gabe;
	auto b = data.begin();
	auto e = data.end();
	bool r = true;
	while(r && b != e)
	{
		int snru1, snru2;
		int no;
		int bday, bmonth, eday, emonth;
		double& irrArea = gabe.beafl;
		int& amount = gabe.menge;
		r = phrase_parse(b, e, (int_[ref(snru1) = _1] >> lit("-") >> int_[ref(snru2) = _1] >> lit("Ga") >> lit(":") >> int_[ref(no) = _1] >> lit("/") >> int_[ref(bday) = _1] >> lit(".") >> int_[ref(bmonth) = _1] >> lit("./ ") >> double_[ref(irrArea) = _1] >> "/" >> int_[ref(amount) = _1] >> lit("/351/") >> int_[ref(eday) = _1] >> lit(".") >> int_[ref(emonth) = _1] >> lit(".") ),
										 space);
		if(!r)
			return EMType1();

		gabe.snru = 10*snru1 + snru2;
		gabe.beginn = date2doy(bday, bmonth);
		gabe.ende = date2doy(eday, emonth);
	}

	return gabe;

	//example
	/*
	040-0 Ga :  1/17.06./  1.0/20/351/17.06.
*/
}

pair<TechnTyp, bool> Berest::technikLesen(int snru)
{
	DBClientConnection& c = dbConnection();

	auto cur = c.query("berest.technik", QUERY("snru" << snru), 1);
	if(!cur->more())
		return error<TechnTyp>();

	auto t = cur->next();
	TechnTyp tt;
	tt.hhMax = t["hhMax"].numberInt();
	tt.hhMin = t["hhMin"].numberInt();
//	tt.hhn = t["hhn"].numberInt();
	tt.hht = t["hht"].numberInt();
	tt.hhNorm = t["hhNorm"].numberInt();
	tt.hhStufung = t["hhStufung"].numberInt();
	tt.turnus = t["turnus"].numberInt();
//	tt.hhOpt = t["hhOpt"].numberInt();

	return success(tt);
}

void Berest::technikSpeichern(int snru, const TechnTyp& t, bool isNew)
{
	DBClientConnection& c = dbConnection();

	BSONObj obj = BSON("snru" << snru <<
										 "hhMax" << t.hhMax <<
										 "hhMin" << t.hhMin <<
										 //								"hhn" << t.hhn <<
										 "hht" << t.hht <<
										 "hhNorm" << t.hhNorm <<
										 "hhStufung" << t.hhStufung <<
										 "turnus" << t.turnus); //<<
	//								"hhOpt" << t.hhOpt));

	if(isNew)
		c.insert("berest.technik", obj);
	else
		c.update("berest.technik", QUERY("snru" << snru), obj, true);
}

WetterTyp Berest::weatherAt(const vector<WetterTyp>& v, int day)
{
	if(!v.empty())
	{
		int i = day - v.front().tag;
		if(0 <= i && i < v.size())
			return v.at(i);
		else
		{
			WetterTyp w;
			w.tag = day;
			auto p = longtermEvapPrecip(day);
			w.verdunstung = p.first;
			w.niederschlag = p.second;
			return w;
		}
	}
	return WetterTyp();
}

pair<double, double> Berest::longtermEvapPrecip(int day)
{
	static const std::array<double, 223> longtermAverageEvaporationValues = {
		/*01.04.*/ 1.1, 1.2, 1.2, 1.2, 1.3, 1.3, 1.3, 1.4, 1.4, 1.4,
		/*11.04.*/ 1.4, 1.5, 1.5, 1.5, 1.6, 1.6, 1.6, 1.7, 1.7, 1.7,
		/*21.04.*/ 1.8, 1.8, 1.8, 1.9, 1.9, 1.9, 2.0, 2.0, 2.0, 2.1,
		/*01.05.*/ 2.1, 2.1, 2.2, 2.2, 2.2, 2.3, 2.3, 2.3, 2.4, 2.4,
		/*11.05.*/ 2.4, 2.5, 2.5, 2.5, 2.6, 2.6, 2.6, 2.7, 2.7, 2.7,
		/*21.05.*/ 2.7, 2.8, 2.8, 2.8, 2.9, 2.9, 2.9, 2.9, 3.0, 3.0, 3.0,
		/*01.06.*/ 3.0, 3.1, 3.1, 3.1, 3.2, 3.2, 3.2, 3.2, 3.3, 3.3,
		/*11.06.*/ 3.3, 3.4, 3.4, 3.4, 3.4, 3.4, 3.4, 3.4, 3.4, 3.4,
		/*21.06.*/ 3.4, 3.4, 3.4, 3.4, 3.4, 3.4, 3.4, 3.4, 3.4, 3.4,
		/*01.07.*/ 3.4, 3.4, 3.3, 3.3, 3.3, 3.3, 3.3, 3.3, 3.3, 3.3,
		/*11.07.*/ 3.3, 3.3, 3.3, 3.3, 3.3, 3.3, 3.3, 3.2, 3.2, 3.2,
		/*21.07.*/ 3.2, 3.2, 3.2, 3.1, 3.1, 3.1, 3.1, 3.1, 3.1, 3.1, 3.0,
		/*01.08.*/ 3.0, 3.0, 3.0, 3.0, 3.0, 2.9, 2.9, 2.9, 2.9, 2.9,
		/*11.08.*/ 2.9, 2.9, 2.8, 2.8, 2.8, 2.8, 2.7, 2.7, 2.7, 2.7,
		/*21.08.*/ 2.6, 2.6, 2.6, 2.6, 2.5, 2.5, 2.4, 2.4, 2.4, 2.4, 2.4,
		/*01.09.*/ 2.3, 2.3, 2.3, 2.2, 2.2, 2.2, 2.2, 2.1, 2.1, 2.1,
		/*11.09.*/ 2.0, 2.0, 2.0, 2.0, 1.9, 1.9, 1.9, 1.8, 1.8, 1.8,
		/*21.09.*/ 1.7, 1.7, 1.6, 1.6, 1.6, 1.5, 1.5, 1.5, 1.4, 1.4,
		/*01.10.*/ 1.4, 1.3, 1.3, 1.3, 1.2, 1.2, 1.2, 1.1, 1.1, 1.0,
		/*11.10.*/ 1.0, 1.0, 0.9, 0.9, 0.9, 0.7, 0.6, 0.7, 0.5, 0.8,
		/*21.10.*/ 1.0, 1.0, 0.9, 0.9, 0.9, 0.7, 0.6, 0.7, 0.5, 0.8
	};

	static const std::array<double, 223> longtermAveragePrecipitationValues = {
		/*01.04.*/ 1.0, 1.7, 0.6, 0.5, 1.1, 0.9, 0.9, 0.9, 1.9, 1.5,
		/*11.04.*/ 1.1, 0.8, 1.2, 1.5, 2.2, 0.9, 1.4, 1.1, 2.0, 0.9,
		/*21.04.*/ 0.7, 1.3, 0.9, 0.9, 0.4, 0.6, 0.9, 1.0, 2.0, 1.6,
		/*01.05.*/ 1.3, 1.1, 2.0, 1.5, 1.6, 1.8, 1.9, 1.3, 1.0, 1.3,
		/*11.05.*/ 4.2, 0.6, 1.6, 1.5, 1.3, 0.6, 0.9, 1.9, 1.4, 4.6,
		/*21.05.*/ 1.0, 0.9, 0.4, 0.9, 2.7, 1.0, 3.6, 2.8, 0.7, 2.2, 2.3,
		/*01.06.*/ 1.1, 2.2, 0.6, 1.3, 1.0, 0.8, 0.7, 2.7, 4.4, 3.5,
		/*11.06.*/ 2.0, 6.0, 1.3, 1.0, 1.8, 1.9, 1.5, 1.0, 3.3, 1.5,
		/*21.06.*/ 1.9, 2.8, 0.7, 0.6, 3.6, 2.4, 4.1, 3.3, 3.5, 1.9,
		/*01.07.*/ 1.6, 1.5, 1.9, 3.0, 3.4, 1.9, 1.1, 0.9, 2.5, 1.2,
		/*11.07.*/ 1.3, 2.2, 1.5, 1.0, 2.5, 2.0, 1.9, 3.4, 1.1, 4.3,
		/*21.07.*/ 3.6, 3.7, 3.6, 1.5, 0.9, 1.4, 2.1, 1.0, 1.4, 1.2, 0.9,
		/*01.08.*/ 0.8, 0.5, 2.4, 1.7, 1.0, 1.3, 0.8, 1.7, 1.9, 1.3,
		/*11.08.*/ 2.0, 1.7, 1.4, 0.3, 2.3, 1.7, 2.8, 1.1, 1.1, 3.1,
		/*21.08.*/ 1.6, 2.9, 1.2, 1.4, 2.6, 1.4, 2.4, 3.2, 4.0, 1.6, 0.6,
		/*01.09.*/ 2.1, 0.5, 0.3, 1.0, 1.4, 1.6, 3.1, 1.8, 2.6, 2.3,
		/*11.09.*/ 2.9, 1.0, 1.2, 1.9, 0.6, 2.0, 1.8, 1.1, 0.7, 1.2,
		/*21.09.*/ 0.6, 1.5, 0.6, 2.3, 1.2, 0.9, 0.6, 2.2, 2.3, 1.0,
		/*01.10.*/ 0.6, 0.8, 2.5, 0.4, 0.7, 0.5, 2.1, 0.5, 1.1, 2.4,
		/*11.10.*/ 0.8, 0.2, 0.9, 1.6, 1.0, 2.5, 1.7, 1.6, 1.5, 1.0,
		/*21.10.*/ 2.2, 2.9, 1.8, 1.4, 1.2, 0.6, 1.3, 2.0, 0.4, 1.9
	};

	if(90 < day && day <= 90+223)
	{
		return make_pair(longtermAverageEvaporationValues.at(day - 90 - 1),
										 longtermAveragePrecipitationValues.at(day - 90 - 1));
	}

	return make_pair(0.0, 0.0);
}

vector<WetterTyp> Berest::wetterLesen(int station, int tag, SortOrder so,
																			bool includePrognosisData)
{
	DBClientConnection& c = dbConnection();

	vector<WetterTyp> v;

	Query query;
	if(tag == 0)
		query = (includePrognosisData ? QUERY("station" << station)
																	: QUERY("station" << station <<
																					"istPrognose" << false))
						.sort("tag", int(so));
	else
		query = includePrognosisData ? QUERY("station" << station << "tag" << tag)
																 : QUERY("station" << station <<
																				 "istPrognose" << false <<
																				 "tag" << tag);

	auto cur = c.query("berest.wetter", query);
	while(cur->more())
	{
		auto w = cur->next();

		WetterTyp wt;
		wt.tag = w["tag"].numberInt();
		wt.niederschlag = w["niederschlag"].numberDouble();
		wt.verdunstung = w["verdunstung"].numberDouble();
		wt.istPrognose = w["istPrognose"].boolean();

		v.push_back(wt);
	}

	//use longterm average value
	if(v.empty())
	{
		WetterTyp wt;
		wt.tag = tag;
		auto p = longtermEvapPrecip(tag);
		wt.verdunstung = p.first;
		wt.niederschlag = p.second;
		wt.istPrognose = true;

		v.push_back(wt);
	}

	return v.empty() ? vector<WetterTyp>(1) : v;
}

void Berest::wetterSpeichern(int tag, const WetterTyp& w, int station,
														 bool isNew)
{
	DBClientConnection& c = dbConnection();

	BSONObj obj = BSON("tag" << tag <<
										 "station" << station <<
										 "niederschlag" << w.niederschlag <<
										 "verdunstung" << w.verdunstung <<
										 "istPrognose" << w.istPrognose);

	if(isNew)
		c.insert("berest.wetter", obj);
	else
		c.update("berest.wetter", QUERY("tag" << tag << "station" << station), obj, true);
}

vector<WetterTyp> Berest::parseWeatherData(const string& data)
{
	using boost::spirit::qi::double_;
	using boost::spirit::qi::int_;
	using boost::spirit::qi::_1;
	using boost::spirit::qi::phrase_parse;
	using boost::spirit::qi::lit;
	using boost::spirit::ascii::space;
	using boost::phoenix::ref;

	vector<WetterTyp> v;
	auto b = data.begin();
	auto e = data.end();
	bool r = true;
	while(r && b != e)
	{
		WetterTyp wt;
		int& tag = wt.tag;
		double& nied = wt.niederschlag;
		double& verd = wt.verdunstung;
		//r = phrase_parse(b, e, (int_[ref(tag) = _1] >> ".:" >> -(lit(" ")) >> double_[ref(verd) = _1] >> "/" >> -(lit(" ")) >> double_[ref(nied) = _1] >> -(lit(": ")) ),
		r = phrase_parse(b, e, (int_[ref(tag) = _1] >> ".:" >> double_[ref(verd) = _1] >> "/" >> double_[ref(nied) = _1] >> -(lit(":")) ),
										 space);
		if(r)
			v.push_back(wt);
	}

	return v;

	//example
	/*
	01.: 3.0/ 0.0: 02.: 2.1/ 0.3: 03.: 3.4/ 0.0: 04.: 1.6/ 4.3: 05.: 2.6/ 0.2:
	06.: 0.9/ 0.0: 07.: 2.2/ 0.1: 08.: 2.0/ 0.0: 09.: 3.6/ 0.0: 10.: 3.6/ 0.0:
	11.: 2.8/ 0.0: 12.: 1.4/ 1.7: 13.: 0.5/ 4.3: 14.: 1.0/ 0.9: 15.: 3.3/ 0.0:
	16.: 2.5/ 0.0: 17.: 2.5/ 0.0: 18.: 3.1/ 0.0: 19.: 3.5/ 0.0: 20.: 4.2/ 0.0:
	21.: 4.4/ 0.0: 22.: 4.2/ 0.0: 23.: 4.4/ 0.0: 24.: 4.1/ 0.0: 25.: 2.6/ 0.0:
	26.: 2.4/ 1.8: 27.: 4.1/ 0.0: 28.: 3.3/ 0.0: 29.: 4.1/ 0.0:
*/
}

vector<WetterTyp> Berest::loadBersimWeatherData(const string& precipFile, const string& evapFile)
{
	using boost::spirit::qi::unused_type;
	using boost::spirit::qi::double_;
	using boost::spirit::qi::int_;
	using boost::spirit::qi::_1;
	using boost::spirit::qi::phrase_parse;
	using boost::spirit::qi::lit;
	using boost::spirit::ascii::space;
	using boost::phoenix::ref;

	vector<WetterTyp> v;
	map<int, WetterTyp> m;
				
	std::array<string, 2> files = { precipFile, evapFile };
	for(auto it = files.begin(); it != files.end(); it++)
	{
		ifstream ifs(*it);
		
		while(ifs.good())
		{
			string data;
			getline(ifs, data); 

			auto b = data.begin();
			auto e = data.end();

			bool r = true;
			while(r && b != e)
			{
				int day, month, year;
				vector<double> values;
				r = phrase_parse(b, e, int_[ref(day)=_1] >> "." >> int_[ref(month)=_1] >> "." >> int_[ref(year)=_1] >> +(double_[[&values](double v, unused_type, unused_type){values.push_back(v);}]),
					space);
				if(r)
				{
					int doy = date2doy(day, month);
					for(int i = 0, doy = date2doy(day, month), size = values.size(); i < size; i++)
					{
						WetterTyp& w = m[doy+i];
						w.tag = doy+i;
						(it == files.begin() ? w.niederschlag : w.verdunstung) = values.at(i);
					}
				}
			}
		}
	}

	std::transform(m.begin(), m.end(), back_inserter(v), [](const pair<int, WetterTyp>& p){ return p.second; });

	return v;;

	//example
	/*
	01.04.89    1.0  1.0  1.0  0.4  1.7  2.0  1.2  1.9  0.9  0.9
11.04.89    1.7  2.9  3.0  2.8  1.7  1.0  1.5  1.7  0.8  1.8
21.04.89    2.8  0.9  0.7  2.9  2.7  1.5  1.0  0.6  0.8  1.4
01.05.89    3.2  2.3  1.9  3.7  3.5  1.8  3.1  2.2  4.0  1.6
11.05.89    1.2
12.05.89    2.5
13.05.89    3.5
*/
}

double potentialEvaporationTW(double globRad_Jpcm2,
															double tavg, double fk = 1)
{
	return (globRad_Jpcm2+93*fk)*(tavg+22)/(150*(tavg+123));
}

double climaticWaterBalanceTW(double precip_mm, double globRad_Jpcm2,
															double tavg, double fk = 1)
{
	return precip_mm - potentialEvaporationTW(globRad_Jpcm2, tavg, fk);
}

vector<WetterTyp> Berest::loadCSVWeatherData(const string& csvFile)
{
	//using boost::spirit::qi::unused_type;
	using boost::spirit::qi::double_;
	using boost::spirit::qi::int_;
	using boost::spirit::qi::_1;
	using boost::spirit::qi::phrase_parse;
	using boost::spirit::qi::lit;
	using boost::spirit::ascii::space;
	using boost::phoenix::ref;

	vector<WetterTyp> v;
				
	ifstream ifs(csvFile);
	string data;
	//skip first header line
	if(ifs.good()) 
		getline(ifs, data);

	while(ifs.good())
	{
		getline(ifs, data); 

		auto b = data.begin();
		auto e = data.end();

		bool r = true;
		while(r && b != e)
		{
			int day, month, year;
			double tavg, precip, globrad;
			vector<double> values;
			r = phrase_parse(b, e, int_[ref(day)=_1] >> "," >> int_[ref(month)=_1] >> "," >> int_[ref(year)=_1] >> ","
			>> double_[ref(tavg)=_1] >> "," >> double_[ref(precip)=_1] >> "," >> double_[ref(globrad)=_1], space);
			if(r)
			{
				int doy = date2doy(day, month);
				WetterTyp wt;
				wt.tag = date2doy(day, month);
				wt.niederschlag = precip;
				wt.verdunstung = potentialEvaporationTW(globrad, tavg);
				v.push_back(wt);
			}
		}
	}
		
	return v;;

	//example
	/*
	"day","month","year","tavg","precip","globrad"
1,1,2006,1.5,0,196
2,1,2006,1.3,0,190
3,1,2006,0.1,0,95
4,1,2006,-0.1,0.1,113
*/
}


//pair<GlobVars, bool> Berest::globVarsLesen()
//{
//	DBClientConnection& c = dbConnection();

//	auto cur = c.query("berest.globVars", Query(), 1);
//	if(!cur->more())
//		return error<GlobVars>();

//	auto gg = cur->next();
//	GlobVars g;
//	g.rechentag = gg["rechentag"].numberInt();
//	g.vorReTag = gg["vorReTag"].numberInt();
//	g.minWetter = gg["minWetter"].numberInt();

//	return success(g);
//}

//void Berest::globVarsSpeichern(const GlobVars& gvs, bool isNew)
//{
//	DBClientConnection& c = dbConnection();

//	BSONObj obj = BSON("rechentag" << gvs.rechentag <<
//										 "vorReTag" << gvs.vorReTag <<
//										 "minWetter" << gvs.minWetter);

//	if(isNew)
//		c.insert("berest.globVars", obj);
//	else
//		c.update("berest.globVars", Query(), obj, true);
//}

template<typename Key, typename Value>
double interpolatedValue(const map<Key, Value>& map, Key key, Key& key4value) throw (out_of_range)
{
	if(map.empty())
		throw out_of_range("map empty");

	auto it = map.find(key);
	if(it != map.end())
	{
		key4value = it->first;
		return it->second;
	}

	//lowerBound returns first element not less than key
	//or the key (but this case has been caught before)
	//so if possible we have to get at the element before 
	auto lowerBound = map.lower_bound(key);
	if(lowerBound == map.begin())
	{
		key4value = map.begin()->first;
		return map.begin()->second;
	}
	lowerBound--;

	auto upperBound = map.upper_bound(key);
	//after last element
	if(upperBound == map.end())
	{
		key4value = map.rbegin()->first;
		return map.rbegin()->second;
	}

	double lowerKey = double(lowerBound->first);
	double lowerValue = double(lowerBound->second);
	double upperKey = double(upperBound->first);
	double upperValue = double(upperBound->second);

	double res = lowerValue +
							 (key - lowerKey)/(upperKey - lowerKey)*(upperValue - lowerValue);

	key4value = key;
	return res;
}

template<typename Key, typename Value>
double interpolatedValue(const map<Key, Value>& map, Key key) throw (out_of_range)
{
	Key k;
	return interpolatedValue(map, key, k);
}

struct ZusR
{
	ZusR(int t, int h) : tag(t), hh(h) {}
	int tag;
	int hh;
};

int gabenAm(const list<ZusR>& gaben, int tag)
{
	int res = 0;
	BOOST_FOREACH(ZusR gabe, gaben)
	{
		if(gabe.tag == tag)
			res += gabe.hh;
	}
	return res;
}

struct DCWRes
{
	DCWRes() : nextDC(0), absNextDCDay(0) {}
	int nextDC, absNextDCDay;
	bool isValid() const { return absNextDCDay > 0; }
};
DCWRes nextDC(const map<int, int>& dc2day, int knownDC, int absKnownDCDay,
							int absSearchDay)
{
	DCWRes res;

	if(dc2day.empty())
		return res;

	//get the relative dc day for the given dc
	int relKnownDCDay = round(interpolatedValue(dc2day, knownDC)+0.1);
	//calculate what the old relative day is for the given new dc and dc day
	int relSearchDay = relKnownDCDay + (absSearchDay - absKnownDCDay);

	//reverse map to get the dc milestone just before (or exactly) to the new current day
	auto day2dc = swapMap(dc2day);
	auto lowerIt = day2dc.lower_bound(relSearchDay);
	//lower_bound returns iterator to first item not less or equal to key
	//thus if the keys are not the same, we're behind the lower value we're seeking
	//if possible we have to back up one more item
	if(lowerIt == day2dc.end())
	{
		auto rb = day2dc.rbegin();
		res.nextDC = rb->second;
		res.absNextDCDay = absKnownDCDay + (rb->first - relKnownDCDay);
		return res;
	}
	else if(lowerIt->first != relSearchDay && lowerIt != day2dc.begin())
		lowerIt--;

	int relLowerDCDay = lowerIt->first;
	int lowerDC = lowerIt->second;
	res.nextDC = lowerDC;
	//get the new absolute dc day by taking the input new dc day as reference
	res.absNextDCDay = absKnownDCDay + (relLowerDCDay - relKnownDCDay);

	return res;
}

list<ZusR> readIrrigationDonations(int snru, double irrigationArea)
{
	list<ZusR> donations;

	BOOST_FOREACH(EMType1 em, gabenLesen(snru))
	{
		// gabe gilt laut handbuch auch als ausgeführt wenn mehr als
		// 50% der fläche bereits beregent wurde und man davon ausgehen kann
		// dass in den nächsten tagen die beregnungsgabe abgeschlossen wird
		if(em.ende >= em.beginn && em.beafl > 0.5*irrigationArea) 
			donations.push_back(ZusR((em.beginn+em.ende) / 2, em.menge));
	}

	return donations;

//	//res.minDatum = 365;

//	BOOST_FOREACH(EMType1 em, gabenLesen(snru))
//	{
//		//for secondary analysis
////		res.schSrec.gabe_mm  = round(em.menge);

//		//finished donation
//		if(em.ende >= em.beginn) //{ abgeschlossene Gabe }
//		{
//			int tag = min((em.beginn+em.ende) / 2, absPreCalculationDay);
//			//res.minDatum = min(tag, res.minDatum);
			
//			// gabe gilt laut handbuch auch als ausgeführt wenn mehr als
//			// 50% der fläche bereits beregent wurde und man davon ausgehen kann
//			// dass in den nächsten tagen die beregnungsgabe abgeschlossen wird
//			if(em.beafl > 0.5*irrigationArea)
//				gaben.push_back(ZusR(tag,round(em.menge)));

//			//for secondary analysis
//			//			res.schSrec.bha_Ist += em.beafl; // { xxxxx.x }
//			//			res.schSrec.bha_Zug += em.beafl; // { xxxxx.x             (*) }
//			//			res.schSrec.m3_Ist += em.beafl*em.menge*10; //  { xxxxxxxx }
//			//			res.schSrec.m3_Zug += em.beafl*em.menge*10; // { xxxxxxxx }
//			//			res.schSrec.nrGabeTt = res.schSrec.nrGabeTt++; // { xx Zahl der 700-Gruppen}
//			//			res.schSrec.nrGabe = res.schSrec.nrGabe++;
//			//			res.schSrec.bha_I_Ges += em.beafl; // { xxxxx.x }
//			//			res.schSrec.mm_I_Ges += round(em.menge); // { xxx }
//			//			res.schSrec.lfd_Bha = 0; // { xxx.x }
//		}
//		else
//		{
//			//for secondary analysis
//			//			if(!DateStr(em.beginn,0,4,res.schSrec.gabe_Tag))
//			//				res.schSrec.gabe_Tag = "....";
//			//			res.schSrec.lfd_Bha = em.beafl; // { xxx.x }

//			// es scheint, dass es sein kann dass eine nicht abgeschlossene
//			// gabe ganz am ende kommen kann, dann wird das nur in lfd_bha notiert
//			// und geendet
//			//break;
//		}
//	}

//	return gaben;
}

Bodenvektor Schlagdaten::lambda(int aktTag) const
{
	static map<int, double> fku;
	static bool initialized = false;
	if(!initialized)
	{
		//attention there was an error in the berest code where the
		//second digit at keys 90, 175, 240 and 282 was missing
		fku[90] = 82; //was 8
		fku[135] = 135;
		fku[175] = 169; //was 17
		fku[210] = 177;
		fku[240] = 166; //was 17
		fku[282] = 127; //was 13
		fku[306] = 95;

		initialized = true;
	}

	double lambkor = interpolatedValue(fku, aktTag)*0.01;
	Bodenvektor ls = lambdaWithoutCorrection;
	for_each(ls.begin(), ls.end(), [lambkor](double& l){ l *= lambkor; });
	return ls;
}

void Schlagdaten::setup()
{
	static std::array<map<int, int>, 2+1> fkpwpl;
	static bool initialized = false;
	if(!initialized)
	{
		map<int, int> m, m2;
		m[100] = 115; m2[90] = 115;
		m[140] = 68; m2[120] = 68;
		m[200] = 50; m2[180] = 50;
		m[240] = 33; m2[220] = 33;
		m[260] = 33; m2[250] = 33;
		m[300] = 25; m2[280] = 25;
		m[312] = 25; m2[344] = 25;
		m[327] = 17; m2[364] = 17;
		m[356] = 15; m2[373] = 15;
		m[1000] = 15; m2[1000] = 15;
		fkpwpl[1] = m; fkpwpl[2] = m2;

		initialized = true;
	}

	std::array<int,3+1> idick;
	idick[0] = 0;
	for(int isp=1; isp<=3; isp++)
	{
		int sd = dicke3[isp];
		if(1 <= sd && sd <= 9)
			idick[isp] = idick[isp-1] + dicke3[isp]*10;
		else
			idick[isp] = 0;
	}

	if(idick[2]==150)
		;
	else if(idick[3]==150)
		;
	else if(idick[3] > 0)
		idick[3] = 150;
	else
		idick[2] = 150;

	int izaecm = 0;
	int is = 1;
	int isp_ = 0;
	double ladm = 0;

	for(int isc=1; isc<=noOfSoilLayers; isc++)
	{
		lambdaWithoutCorrection[isc] = 10;

		for(int j=1; j<=isc*10; j++)
		{
			izaecm++;
			//beim ersten mal ist isp_ noch 0 und idick[isp] ist auch 0, weil es bei 1 beginnt
			//und damit wird beim ersten mal dieser Teil übersprungen
			if(izaecm<=idick[isp_])
			{
				if(izaecm==31)
					isp_--;
				else
					goto summe;
			}

			//die ersten 30 cm können potentiell andere parameter verwenden
			//als der restliche boden, auch wenn oben alle parameter derzeit gleich sind
			if(izaecm==31)
				is = 2;

			if(1121<=stTyp && stTyp<=1323)
				ladm = 100;
			else if(713<=stTyp && stTyp<=723)
				ladm = 5;
			else
			{
				int fk3search = round(fk3[isp_+1]*10);
				auto it = fkpwpl[is].find(fk3search);
				if(it == fkpwpl[is].end())
				{
					auto ub = fkpwpl[is].upper_bound(round(fk3[isp_+1]*10));
					if(ub == fkpwpl[is].end())
						ladm = fkpwpl[is].rbegin()->second;
					else
						ladm = ub->second;
				}
				else
					ladm = it->second;
			}
			
			if(ivd()*10.0 < izaecm)
				ladm = 1;

			ladm /= 100.0;
			isp_++;
summe:
			//aufsummierung von fk und pwp der einzelnen schichten cm-weise
			fk[isc] += fk3[isp_]*0.1;
			pwp[isc] += pwp3[isp_]*0.1;
			//lambda wird nicht summiert, sondern der kleinere wert in der resultierenden schicht gewinnt
			
			lambdaWithoutCorrection[isc] = min(ladm, lambdaWithoutCorrection[isc]);
		}

		lambdaWithoutCorrection[isc] /= double(isc*isc);
	}

}

inline double nFK(double fk, double pwp){ return fk - pwp; }

double infiltrationBarrier(double fk, double pwp, int absCurrentDay, int layerDepthCM)
{
	static map<int, double> vsGr, vsGr2;
	static bool initialized = false;
	if(!initialized)
	{
		vsGr[80] = 11;
		vsGr[140] = 7;
		vsGr2[100] = 10;
		vsGr2[200] = 8;
		initialized = true;
	}

	double vs = interpolatedValue(layerDepthCM <= 30 ? vsGr : vsGr2, absCurrentDay)*0.1;
	if(vs < 1)
	{
		double pwfk = pwp/fk - 1 + vs;
		if(pwfk > 0)
			vs += (1.0 - vs)*pwfk*0.95/(0.66 - 0.30);
	}

	return nFK(fk, pwp)*vs + pwp;
}

//available water capacity in percent (nFK Prozent)
double awcPercent(int fromLayer, int toLayer, const Bodenvektor& soilMoisture,
									const Bodenvektor& fk, const Bodenvektor& pwp)
{
	double sumWc = 0;
	double sumAwc = 0;

	for(int i=fromLayer; i<=toLayer;i++)
	{
		sumWc += soilMoisture[i] - pwp[i];
		sumAwc += nFK(fk[i], pwp[i]);
	}
	double awcp = 100.0*sumWc/sumAwc;

	return max(0, round(awcp));
}

struct IRes
{
	double effectivePrecipitation;
	double pet;
	double effectiveIrrigation;
	double effectiveIrrigationUncovered;
};
IRes calcInterception(double precipitation, double evaporation, double irrigation,
											double transpirationFactor, IrrigationMode im)
{
//	static const double null = 0;
	static const double null5 = 0.5;
	static const double null2 = 0.2;

	//potential evaporation has at least the evaporation component
	double pet = evaporation;
	double interceptionPrecipitation = 0;

	transpirationFactor = max(1.0, transpirationFactor);

	// Berechnung fuer natuerlichen Regen
	double tin = null5 + (evaporation - 1) * transpirationFactor * null2;

	if(precipitation > 0)
	{
		interceptionPrecipitation = tin;
		pet = max(0.0, evaporation - interceptionPrecipitation*null5);
	}

	// Berechnung fuer Zusatzregen/Spruehverluste
	double interceptionIrrigation = 0, sprinkleLoss = 0;
	if(irrigation > 1)
	{
		switch(im)
		{
		case sprinkleLosses:
			interceptionIrrigation = 0.6*tin*(1.0 + irrigation*0.05);
			sprinkleLoss = (1 + (evaporation - 2.5)*null2)*null2*irrigation;
			break;
		case noSL:
			interceptionIrrigation = 0.0;
			sprinkleLoss = 0.0;
		}

		// correction of potential evaporation
		if(precipitation > 0)
			pet = evaporation - (interceptionIrrigation + interceptionPrecipitation)*null5;
		else
			pet = evaporation - interceptionIrrigation*0.75;
	}

	pet = max(0.0, pet);

	IRes res;
	res.pet = pet;
	res.effectivePrecipitation = precipitation - interceptionPrecipitation;
	res.effectiveIrrigation = irrigation - interceptionIrrigation - sprinkleLoss;
	res.effectiveIrrigationUncovered = irrigation - sprinkleLoss;

	return res;
}

//wia = WiA = Wassergehalt zu Beginn des Zeitabschnittes i (mm) = initialExcessWater
//nist = Ni* = Ni - Vi = netInfiltrationFromAboveLayer
//lai = lambdai
double glugla(double wia, double nist, double lai)
{
	//wie = WiE = Wassergehalt am Ende des Zeitabschnittes i (mm) = finalExcessWater
	double wie = 0;

	if(wia < 0) //more water will fit into the current layer (below infiltration barrier)
	{
		wie = nist + wia;
		if(nist >= 0 && wie > 0)
		{
			double b2 = -exp(-2.0*sqrt(lai*nist)*(1.0 + wia/nist));
			wie = sqrt(nist/lai)*(1.0 + b2)/(1.0 - b2);
		}
	}
	else //current layer already above infiltration barrier 
	{
		if(nist == 0)
			wie = wia/(1.0 + lai*wia);
		else if(nist < 0) // Entzug
		{
			double n7l = sqrt(-nist/lai);
			double nl = sqrt(-nist*lai);
			double at = atan(wia/n7l)/nl;
			if(at <= 1)
				wie = nist*(1.0 - at);
			else
			{
				double beta = sin(nl)/cos(nl);
				wie = n7l*(wia - n7l*beta)/(n7l + wia*beta);
			}
		}
		else //if(nist > 0) // kein Entzug
		{
			double n7l = sqrt(nist/lai);
			double nl = sqrt(lai*nist);
			double alpha = (wia - n7l)/(wia + n7l)*exp(-2.0*nl);
			wie = n7l*(1.0 + alpha)/(1.0 - alpha);
		}
	}

	return wie;
}

int isInLayer(int depthDM)
{
	static const std::array<int, 15+1> l = { 0, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5 };
	return depthDM > 15 ? 6 : l[depthDM];
}

inline double waterAbstractionFraction(int noOfLayers, int layerNo)
{
	return (2.0 - (2.0*double(layerNo) - 1.0)/double(noOfLayers))/double(noOfLayers);
}

Bodenvektor f1Koitzsch(int maxDepthDM, int ivd, bool isCovered)
{
	static const std::array<double,3+1> fnub = {-9999.9,
																							0.625, 0.3, 0.075};

	if(ivd>1)
		ivd = 20;

	Bodenvektor ff;
	ff.fill(0);

	if(isCovered)
	{
		maxDepthDM = min(15, maxDepthDM);

		int depthDM = 0;
		enum UD { up, down };
		UD direction = down;
		for(int l=1; l<=maxDepthDM; l++)
		{
			switch(direction)
			{
			case down:
				depthDM++;
				if(depthDM>ivd)
				{
					depthDM = ivd;
					direction = up;
				}
				break;
			case up:
				depthDM--;
				if(depthDM<1)
				{
					depthDM = 1;
					direction = down;
				}
				break;
			}

			int j = isInLayer(depthDM);
			ff[j] += waterAbstractionFraction(maxDepthDM, l); 
		}
	}
	else
	{
		for(int i=1; i<=3; i++)
			ff[i]=fnub[i];
	}

	return ff;
}

Bodenvektor giKoitzsch(int maxLayer, int extractionDepthDM, bool isCovered,
											 Bodenvektor re, int ivd)
{
	maxLayer = min(5, maxLayer);

	Bodenvektor gi;
	gi.fill(0);

	double rmax = 0;
	for(int i=1, depthDM=0, n=0; i<=maxLayer; i++)
	{
		for(int l=1; l<=i; l++)
		{
			depthDM++;
			if(depthDM > extractionDepthDM)
				return gi;
			else
			{
				if(!isCovered || depthDM==extractionDepthDM || depthDM>=2)
				{
					Bodenvektor ff = f1Koitzsch(depthDM, ivd, isCovered);
					Bodenvektor gj = re*ff;
					double rij = sum(gj);

					if(rmax - 0.002 > rij)
					{
						if(depthDM>=5)
						{
							if(n==1)
								return gi;
							else
								n = 1;
						}
					}
					else
					{
						rmax = rij;
						gi = gj;
						n = 0;
					}
				}
			}
		}
	}
	return gi;
}

double capillaryRiseBarrier(double fk, int layerThicknessDM)
{
	double fk10 = fk/double(layerThicknessDM);
	return (fk10 + 40.667 - fk10*0.408)*layerThicknessDM;
}

struct BF1Res
{
	BF1Res() : aet(0), infiltration15dm(0) { soilMoisture.fill(0.0); }
	double aet;
	Bodenvektor soilMoisture;
	double infiltration15dm;
};
BF1Res bf1Tag(int extractionDepthDM, double coverDegree, double pet,
							int absCurrentDay, const Bodenvektor& fk, const Bodenvektor& pwp,
							const Bodenvektor& lambda, const Bodenvektor& currentSoilMoisture,
							bool isSoilMoisturePrognosis, double evaporation, int ivd,
							int groundwaterlevel, double dailyPrecipitationAndIrrigation)
{

	BF1Res res;

	//remembers the maximum layer reduction parameters for covered soil are obtained from
	//but in the case of uncovered soil, just stays at layer 1 and
	//all covered soil reduction paramters are 1.0
	int maxLayer = 1;

	//calculation of the current reduction parameters
	const int sreb = 3; // { sreb=(pwp-pw7)/(pwp-pw9)=3.0 }
	Bodenvektor reu, reb;
	reu.fill(0);
	reb.fill(1.0);
	for(int j=1, depthDM=0; j<=noOfSoilLayers; j++)
	{
		double fkj = fk[j];
		double pwpj = pwp[j];
		double pwp4pj = pwpj - 0.04*nFK(fkj, pwpj);
		double wmmj = currentSoilMoisture[j] + (j == 1 ? dailyPrecipitationAndIrrigation : 0);

		double r = wmmj > pwp4pj ? min((wmmj - pwp4pj)/(fkj - pwp4pj), 1.0) : 0;
		reu[j] = r*r;

		if(coverDegree > 0.01 && depthDM < extractionDepthDM)
		{
			maxLayer = j;
			depthDM += j;
			if(wmmj < fkj)
			{
				double fkdm = fkj/double(j);
				double nfk = nFK(fkj, pwpj);
				double xsmj = fkdm < 10 || fkdm > 46
											? nfk*0.81 + pwpj
											: (67.77
												 + 3.14*fkdm
												 - 0.2806*fkdm*fkdm
												 + 0.008131*fkdm*fkdm*fkdm
												 - 0.0000735*fkdm*fkdm*fkdm*fkdm
												 )*nfk*0.01 + pwpj;

				double rk = pet > 3.0 ? (fkj - xsmj)*0.0666 : (xsmj - pwpj)*0.3333;

				double wmmcritj = xsmj + (pet - 3.0)*rk;
				if(wmmj < wmmcritj)
				{
					double pwp12pj =  pwpj - 0.12*nFK(fkj, pwpj);
					if(pwpj < wmmj)
						reb[j] = (wmmj - pwp12pj)/(wmmcritj - pwp12pj);
					else if(pwp4pj < wmmj)
						reb[j] = (wmmj - pwp4pj)/(wmmcritj - pwp12pj)*sreb;
					else
						reb[j] = 0;
				}
			}
		}
	}

	//calculation of the water abstraction from the soil layers
	Bodenvektor waterAbstraction;
	waterAbstraction.fill(0);
	
	// at least partly uncovered ground
	if(coverDegree < 0.99) 
	{
		const int maxLayerUncovered = 3, extractionDepthDMUncovered = 6;
		Bodenvektor gi = giKoitzsch(maxLayerUncovered, extractionDepthDMUncovered, false, reu, ivd);
		for(int j=1; j<=maxLayerUncovered; j++)
			waterAbstraction[j] = evaporation*gi[j];
	}

	//at least partly covered ground
	if(coverDegree > 0.01)
	{
		//in prognosis mode restrict extraction depth and thus max layer no
		if(isSoilMoisturePrognosis)
		{
			extractionDepthDM = min(extractionDepthDM, 6);
			maxLayer = min(maxLayer, 3);
		}

		Bodenvektor gi = giKoitzsch(maxLayer, extractionDepthDM, true, reb, ivd);
		for(int j=1; j<=maxLayer; j++)
		{
			res.aet += pet*gi[j];
			waterAbstraction[j] = waterAbstraction[j]*(1.0 - coverDegree) + pet*gi[j]*coverDegree;
		}
	}

	//calculation of soil moisture
	double infiltrationFromPrevLayer = dailyPrecipitationAndIrrigation;
	int lowestCapillaryRiseLayerNo = 0;
	for(int j = 1, depthDM=0; j <= noOfSoilLayers; j++)
	{
		double fkj = fk[j];
		double pwpj = pwp[j];
		double crBarrier = capillaryRiseBarrier(fkj, j);
		depthDM += j;
		lowestCapillaryRiseLayerNo = 0;
		if(double(depthDM) - 0.5*double(j) <= double(groundwaterlevel))
		{
			//above that barrier the water will start to infiltrate to the next layer
			double infBarrier = infiltrationBarrier(fkj, pwpj, absCurrentDay, depthDM*10);

			//in the next steps we basically care just about the excess water
			//not about the layers full water content, so we calculate
			//just the difference (positive or negative) to the infiltration barrier
			//(in case of the first layer, possibly including todays precipitation)
			double initialExcessWater = currentSoilMoisture[j] - infBarrier;

			double netInfiltrationFromAboveLayer = infiltrationFromPrevLayer - waterAbstraction[j];

			//the excess water after calculation of the infiltration to the next layer
			double finalExcessWater = glugla(initialExcessWater, netInfiltrationFromAboveLayer, lambda[j]);

			//calculate the resulting infiltration for the next layer from this layer
			infiltrationFromPrevLayer = initialExcessWater + netInfiltrationFromAboveLayer - finalExcessWater;

			//add the (positive/negative) excess water back to the infiltration barrier
			//to obtain the actual water content now in this layer
			//(after water could infiltrate to the next layer)
			double pw9j = pwpj - 0.04*nFK(fkj, pwpj);
			res.soilMoisture[j] = max(infBarrier + finalExcessWater, pw9j);

			//if the water content now is higher than the capillary rise
			//remember the lowest layer which has to be processed for rising excess water
			if(res.soilMoisture[j] > crBarrier)
				lowestCapillaryRiseLayerNo = j;
		}
		else
		{
			res.soilMoisture[j] = crBarrier;
			infiltrationFromPrevLayer = 0;
		}
	}

	//VERTIKALER AUSGLEICH BEI UEBERFEUCHTUNG
	//calculate the capillary rise if applicable
	for(int j=lowestCapillaryRiseLayerNo; j>=1; j--)
	{
		double excessWater = max(0.0, res.soilMoisture[j] - capillaryRiseBarrier(fk[j], j));
		if(j > 1)
			res.soilMoisture[j-1] += excessWater;
		res.soilMoisture[j] -= excessWater;
	}

	res.infiltration15dm = infiltrationFromPrevLayer;
	return res;
}

struct BFxRes
{
	BFxRes() : quAkt(0), quSoll(0) { soilMoisture.fill(0.0); }
	Bodenvektor soilMoisture;
	double quAkt, quSoll;
};
BFxRes bfXTage(int xDays, int absCurrentDay, int relDCDay, Bodenvektor currentSoilMoisture,
							 const Schlagdaten& schlag, bool bfprognose, const WiDatTyp& wiDat,
							 IrrigationMode im, list<ZusR> gaben, const std::vector<WetterTyp>& weather,
							 ostream& out)
{
	BFxRes res;
	res.soilMoisture = currentSoilMoisture;

	double quSum = 0;
	
	double coverDegree = interpolatedValue(wiDat.day2bd, relDCDay);//-1);

	//just temporary to enable direct comparision with original BEREST90
	//save the day this function has been called with, so the
	//the day won't be advanced for lambda calculations
	int absCurrentPrognosisDay = absCurrentDay;

	if(xDays>0)
	{
		int days = xDays;
		while(xDays>0)
		{
			absCurrentDay++;
			relDCDay++;

			double currentQuSoll = coverDegree < 0.01 ? 0 : interpolatedValue(wiDat.day2qu, relDCDay);

			coverDegree = interpolatedValue(wiDat.day2bd, relDCDay);

			int roundedExtractionDepthDM = coverDegree <= 0.001
				? 0 : int(round(interpolatedValue(wiDat.day2n, relDCDay)+0.1));
			
			WetterTyp wetterBf = weatherAt(weather, absCurrentDay);

			int hhzus = gabenAm(gaben, absCurrentDay);

			double transpirationFactor = interpolatedValue(wiDat.day2t, relDCDay);

			IRes res1 = calcInterception(wetterBf.niederschlag, wetterBf.verdunstung,
																	hhzus, transpirationFactor, im);
			double pet = coverDegree < 0.001 ? 0 : res1.pet * transpirationFactor;
			double effectivePrecipitation = res1.effectivePrecipitation;
			double effectiveIrrigation = res1.effectiveIrrigation;
			double effectiveIrrigationUncovered = res1.effectiveIrrigationUncovered;

			// Wassergehalt der oberen Schicht wird um den Niederschlag
			// und den Zusatzregen zu Beginn des Tages erhoeht
			double dailyPrecipitationAndIrrigation = //0;
			//res.soilMoisture[1] +=
					(effectivePrecipitation + effectiveIrrigation) * coverDegree +
					(wetterBf.niederschlag + effectiveIrrigationUncovered) * (1.0 - coverDegree);

			//compared to the PASCAL code absCurrentDay is already at this point advanced one day
			//because in the PASCAL code the aktDatum is being cut to 7 day steps outside this function
			//but here absCurrentDay has been already incremented at least once 
			//(so incrementing it once more to get use the day after the aktDatum (bfDatum) as calculation base isn't necessary anymore)
			//int tbcd = bfprognose ? absCurrentPrognosisDay : ((int(absCurrentDay / 7.0) + 1) * 7) - 3;
			int tbcd = absCurrentDay;
			BF1Res res2 = bf1Tag(roundedExtractionDepthDM, coverDegree, pet, tbcd, schlag.fk,
													 schlag.pwp, schlag.lambda(tbcd), res.soilMoisture,
													 bfprognose, wetterBf.verdunstung, schlag.ivd(),
													 schlag.groundwaterlevel, dailyPrecipitationAndIrrigation);
			double aet = res2.aet;
			res.soilMoisture = res2.soilMoisture;
						
			double quAet7Pet = 0;
			if(coverDegree < 0.001)
				quAet7Pet = 0;
			else
			{
				if(pet > 0.01)
					quAet7Pet = aet/pet;
				else
					quAet7Pet = 1;
			}
			if(quAet7Pet < currentQuSoll)
				res.quAkt += currentQuSoll - quAet7Pet;
			quSum += currentQuSoll;

			xDays--;

			out << absCurrentDay << ","
					<< wetterBf.niederschlag << ","
					<< -wetterBf.verdunstung << ","
					<< hhzus << ","
					<< pet << ","
					<< aet << ","
					<< quAet7Pet << ","
					<< currentQuSoll << ","
					<< res2.infiltration15dm << ","
					<< res.soilMoisture[1] << ","
					<< res.soilMoisture[2] << ","
					<< res.soilMoisture[3] << ","
					<< res.soilMoisture[4] << ","
					<< res.soilMoisture[5] << ","
					<< (accumulate(res.soilMoisture.begin()+1, res.soilMoisture.begin()+3, 0.0)) << "," 
					<< res.soilMoisture[3] << ","
					<< relDCDay << ","
					<< 0
					<< endl;

//			out << absCurrentDay << "\t"
//					<< res.soilMoisture[1] << "\t"
//					<< res.soilMoisture[2] << "\t"
//					<< res.soilMoisture[3] << "\t"
//					<< res.soilMoisture[4] << "\t"
//					<< res.soilMoisture[5] << "\t"
//					<< (accumulate(res.soilMoisture.begin()+1, res.soilMoisture.begin()+4, 0.0)) << "\t"
//					<< (accumulate(res.soilMoisture.begin()+1, res.soilMoisture.end(), 0.0)) << "\t"
//					<< quAet7Pet << "\t"
//					<< currentQuSoll << "\t"
//					<< res2.infiltration15dm << "\t"
//					<< hhzus << "\t"
//					<< wetterBf.niederschlag << "\t"
//					<< aet << endl;

			int dummy = 1;
		}

		res.quAkt = (quSum - res.quAkt) / double(days);
		res.quSoll = quSum / double(days);
	}

	return res;
}

inline double pFK2mm(double percentValue, double fk)
{ 
	return 0.01*percentValue*fk;
}

inline double pNFK2mm(double percentValue, double fk, double pwp)
{
	return pwp + 0.01*percentValue*(fk - pwp);
}

inline double mm2pNFK(double mmValue, double fk, double pwp)
{
	return (mmValue - pwp)/(fk - pwp)*100.0;
}

// works only if result is in a 1 dm layer
inline double volP2mm(double value) { return value; }

struct  ReBFRes
{
	ReBFRes() { initialSoilMoisture.fill(0); }
	int absInitialSoilMoistureDay;
	Bodenvektor initialSoilMoisture;
	double groundwaterLevel;
};
ReBFRes ReBF(int snru, int minDatum, const Bodenvektor& fk, const Bodenvektor& pwp)
{
	ReBFRes res;

	std::array<int,6+1> calculationThickness = {-9999, 1, 3, 6, 10, 15, 16};

	BMTyp bm = getValue(bodenfeuchteLesen(snru, minDatum));
	res.groundwaterLevel = bm.grws;

	int i = 1, k = 1;
	auto ci = bm.dicke2wert.begin();
	while(i < 16 && ci != bm.dicke2wert.end())
	{
		int thickness = ci->first;
		double value = ci->second;
		switch(bm.einheit)
		{
		case pFK:
			res.initialSoilMoisture[k] += pFK2mm(value, fk[k])/double(k);
			break;
		case pNFK:
			res.initialSoilMoisture[k] += pNFK2mm(value, fk[k], pwp[k])/double(k);
			break;
		case Volp: case mm:
			res.initialSoilMoisture[k] += volP2mm(value);
			break;
		default: ;
		}
		if(i == calculationThickness[k])
			k++;
		i++;
		if(thickness < calculationThickness[k])
			ci++;
	}
	res.absInitialSoilMoistureDay=bm.tag;

	return res;
}

struct HRes
{
	QuType qu;
	TechnTyp schlTechn;
};
HRes hohe(int relDCDay, Bodenvektor currentSoilMoisture,
					const Schlagdaten& schlag, double quSoll,
					const TechnTyp& ischlTechn, const WiDatTyp& wiDat, IrrigationMode im,
					const std::vector<WetterTyp>& weather, ostream& out)
{
	HRes res;
	QuType& qu = res.qu;
	TechnTyp& schlTechn = res.schlTechn;
	schlTechn = ischlTechn;

	int direction = 0, irrigationDays = 0;

	// ANFANGSWERTE
	qu.qu00 = 0;
	qu.quhh = 0;
	qu.qusoll = quSoll;

	//loeschGabenImHeap();
	irrigationDays = max(4, min(schlTechn.turnus, 14));

	qu.quef = qu.qusoll - interpolatedValue(wiDat.day2ef, relDCDay);

	schlTechn.hhOpt = 0;
	if(qu.qusoll < 0.1)
		schlTechn.empfText = "Entw/Zeitr";
	else
	{
		BFxRes res = bfXTage(irrigationDays, schlag.absPreCalculationDay(), relDCDay,
												 currentSoilMoisture, schlag, true, wiDat, im, list<ZusR>(),
												 weather, out); // q-ist bei hhOpt=0
		qu.qu00 = res.quAkt;
		qu.qusoll = res.quSoll;
		if(qu.qu00 < qu.qusoll)
		{
			if(qu.quef < qu.qu00)
				schlTechn.empfText = "in ca 4 Tg";
			else
			{
				schlTechn.hhOpt = schlTechn.hhNorm;
				direction = 0;
				for(;;)
				{
					list<ZusR> gaben;
					gaben.push_back(ZusR(schlag.absCalculationDay+1, schlTechn.hhOpt));
					BFxRes res = bfXTage(irrigationDays, schlag.absPreCalculationDay(), relDCDay,
															 currentSoilMoisture, schlag, true, wiDat, im, gaben, weather, out);
					qu.quhh = res.quAkt;
					qu.qusoll = res.quSoll;
					//if qu.quhh > (qu.qusoll+qu.quef)/2
					if(qu.quhh > (1.00 + qu.qusoll)/2.0) // Evtl. Reduzierung
					{
						if(direction > 0 || schlTechn.hhOpt == schlTechn.hhMin)
							goto ende;
						else // Reduzierung von hhOpt
						{
							schlTechn.hhOpt = max(schlTechn.hhMin, schlTechn.hhOpt - schlTechn.hhStufung);
							if(direction < 0)
								goto ende;
							direction = -1;
						}
					}
					else // evtl. Erhoehung
					{
						if(qu.quhh > qu.quef || direction < 0 || schlTechn.hhOpt == schlTechn.hhMax)
							goto ende;
						else // Erhoehung von hhOpt
						{
							schlTechn.hhOpt = min(schlTechn.hhOpt + schlTechn.hhStufung, schlTechn.hhMax);
							if(qu.quhh < qu.quef)
								direction = 1;
							else
								direction++;
							if(direction > 3)
								goto ende;
						}
					}
				}
			}
		}
		else
			schlTechn.empfText = "BF opt.";
	}
ende:
	if(schlTechn.hhOpt > 0)
	{
		if(schlTechn.hhOpt == schlTechn.hhMax && qu.quhh < qu.quef)
			schlTechn.empfText = "S.K. erh.";
		else
			schlTechn.empfText = "Gabe opt.";
	}

	return res;
}

pair<TechnTyp, bool> technologie(int snru, int absCurrentSoilMoistureDay,
																 Bodenvektor currentSoilMoisture, const Schlagdaten& s,
																 const std::vector<WetterTyp>& weather)
{
	TechnTyp schlTechn;

	//the bfDatum has to be temporary the date of the vorRechentag as is the case with the original BEREST
	//thus even though we actually increased the bfDatum by 7 days at this point (as we have the predication soil moisture),
	//we still use the soil parameters calculated for the vorRechentag
	schlTechn.hhMax = round(infiltrationBarrier(s.fk[1], s.pwp[1], absCurrentSoilMoistureDay, 1*10) +
													infiltrationBarrier(s.fk[2], s.pwp[2], absCurrentSoilMoistureDay, (1 + 2)*10) -
													(currentSoilMoisture[1] + currentSoilMoisture[2]));
	int vb60 = round(infiltrationBarrier(s.fk[3], s.pwp[3], absCurrentSoilMoistureDay, (1 + 2 + 3)*10) - currentSoilMoisture[3]);
	if((schlTechn.hhMax * vb60) > 0)
		schlTechn.hhMax += vb60;

	for(int i=1; i<=5; i++)
	{
		//read weather forecast
		//WetterTyp wetterBf = weatherAt(weather, s.absPreCalculationDay() + i);
		WetterTyp wetterBf = weatherAt(weather, absCurrentSoilMoistureDay - 1 + i);
		schlTechn.hhMax += round(wetterBf.verdunstung - wetterBf.niederschlag);
	}

	schlTechn.hhMax = max(0, schlTechn.hhMax);
	schlTechn.turnus = 1;
	schlTechn.hht = 1;
	if(schlTechn.hhMax < 15)
		schlTechn.hhMin = 0;
	else
	{
		auto res = technikLesen(snru);
		if(wasSuccess(res))
		{
			TechnTyp tt = getValue(res);
			schlTechn.turnus = round(tt.turnus);
			schlTechn.hhNorm = round(tt.hhNorm);

			switch(s.hangN)
			{
			case 1: case 2: schlTechn.hhn = 50; break;
			case 3: schlTechn.hhn = 40; break;
			case 4: case 5: schlTechn.hhn = 30; break;
			case 6: schlTechn.hhn = 20; break;
			default: ;
			}

			schlTechn.hht = round(tt.hht);
			schlTechn.hhMin = round(tt.hhMin);
			schlTechn.hhStufung = round(tt.hhStufung);
		}
		else
		{
			schlTechn.turnus = 0;
			schlTechn.hht = 0;
		}

		if(schlTechn.turnus > 0 && schlTechn.hht > 0)
		{
			schlTechn.hhMax = min(schlTechn.hht, min(schlTechn.hhn, schlTechn.hhMax));
			while(schlTechn.hhMax < schlTechn.hhNorm)
				schlTechn.hhNorm -= schlTechn.hhStufung;
		}
	}
	schlTechn.hhOpt = 0;

	return make_pair(schlTechn, schlTechn.turnus > 0 && schlTechn.hht > 0);
}

int calcAbsSearchDCDay(const map<int, int>& dc2day, int knownDC, int absKnownDCDay,
											 int searchDC)
{
//	int relFoundSearchDC;
	int relSearchDCDay = round(interpolatedValue(dc2day, searchDC));//, relFoundSearchDC));
	int relKnownDCDay = round(interpolatedValue(dc2day, knownDC));

	return absKnownDCDay - (relKnownDCDay - relSearchDCDay);
}

struct CSDCRes
{
	int searchDC, relSearchDCDay;
};
CSDCRes calcSearchDC(const map<int, int>& dc2day, int knownDC, int absKnownDCDay,
										int absSearchDCDay)
{
	CSDCRes res;

	int deltaAbsDays = absSearchDCDay - absKnownDCDay;
	int relKnownCurveDCDay = round(interpolatedValue(dc2day, knownDC));
	res.relSearchDCDay = relKnownCurveDCDay + deltaAbsDays;
	res.searchDC = round(interpolatedValue(swapMap(dc2day), res.relSearchDCDay));

	return res;
}

void Berest::run(Schlagdaten schlag, const vector<WetterTyp>& weather, IrrigationMode im)
{
	ostringstream output;

	ofstream ofs;
	ofs.open("out.csv");
	if(ofs.bad())
		return;
	ostream& out = ofs;
	//ostream& out = cout;

	out << "C++ day,"
			<< "C++ precip,"
			<< "C++ evap,"
			<< "C++ irrWater,"
			<< "C++ pet,"
			<< "C++ aet,"
			<< "C++ aet/pet,"
			<< "C++ aet/pet soll,"
			<< "C++ infil 15dm,"
			<< "C++ mm 10cm,"
			<< "C++ mm 10-30cm,"
			<< "C++ mm 30-60cm,"
			<< "C++ mm 60-100cm,"
			<< "C++ mm 100-150cm,"
			<< "C++ mm 0-30cm,"
			<< "C++ mm 30-60cm,"
			<< "C++ rel DC day,"
			<< "C++ lambda"
			<< endl;

	int snru = atoi(schlag.schlagnummer.c_str());
	output << schlag.schlagnummer.substr(0, 3) << "-"
				 << schlag.schlagnummer.substr(3, 1) << " "
				 << schlag.faNrAfVz() << " " << schlag.symbol << "      ";

	//get initial soilmoisture
	//------------------------------

	ReBFRes res1 = ReBF(snru, 0, schlag.fk, schlag.pwp);
	Bodenvektor initialSoilMoisture = res1.initialSoilMoisture;
	schlag.groundwaterlevel = res1.groundwaterLevel;
	int absInitialSoilMoistureDay = res1.absInitialSoilMoistureDay;

	//reads at least all the irrigation water into heap structure
	list<ZusR> gaben = readIrrigationDonations(snru, schlag.beFlaeche);

	//calculate when crop period starts, to determine length of fallow period
	//if there is no DC=1, then the return DC day will be from a later
	//(but the first available) stage, but in this case this doesn't matter
	//because we care for actually the first stage and not necessarily DC=1
	WiDatTyp plantWiDat = getValue(wiDatLesen(schlag.faNrAfVz()));
	int absCropStartDay = max(calcAbsSearchDCDay(plantWiDat.dc2day, schlag.knownDC,
																							 schlag.knownDCDay, 1),
														absInitialSoilMoistureDay);
	Bodenvektor startCropSoilMoisture = initialSoilMoisture;

	//calculate soilmoisture after fallow period
	//-----------------------------------

	//but even more: first run everything with fallow
	//we run as far as possible with fallow but only until the day before the
	//calculation day
	int fallowDays = min(schlag.absPreCalculationDay(), absCropStartDay) - absInitialSoilMoistureDay;
	if(fallowDays > 0)
	{
		WiDatTyp fallowWiDat = getValue(wiDatLesen("0000/1/0"));
		BFxRes res2 = bfXTage(fallowDays, absInitialSoilMoistureDay, 0, initialSoilMoisture,
													schlag, false, fallowWiDat, im, gaben, weather, out);
		startCropSoilMoisture = res2.soilMoisture;
		//we've proceeded for fallow days
		_ASSERTE(absInitialSoilMoistureDay + fallowDays == absCropStartDay);
		//		Bodenfeuchte bf = createBF(snru, schlag.bfDatum, schlag.wmm, schlag.fk,
		//															 schlag.pwp);
		//		cout << "fallow BF ist: " << bf.toString() << endl;
	}

	if(schlag.absPreCalculationDay() < absCropStartDay)
	{
		cout << "Berest could just run fallow. No need for irrigation so far." << endl;
		cout << "Press button." << endl;
		getchar();
		exit(0);
	}

	//calculate soilmoisture in past crop period
	//---------------------------------------

	int plantDays = schlag.absPreCalculationDay() - absCropStartDay;
	if(plantDays <= 0)
	{
		cout << "No plant days to calculate. Exiting." << endl;
		cout << "Press button." << endl;
		getchar();
		exit(0);
	}

	int relCropStartDCDay = calcSearchDC(plantWiDat.dc2day, schlag.knownDC,
																			 schlag.knownDCDay, absCropStartDay).relSearchDCDay;
	BFxRes res3 = bfXTage(plantDays, absCropStartDay, relCropStartDCDay, startCropSoilMoisture,
											 schlag, false, plantWiDat, im, gaben, weather, out);
	Bodenvektor currentSoilMoisture = res3.soilMoisture;
	int absCurrentSoilMoistureDay = absCropStartDay + plantDays;

	Bodenvektor pnfk;
	for(int i=1; i<=5; i++)
		pnfk[i] = mm2pNFK(currentSoilMoisture[i], schlag.fk[i], schlag.pwp[i]);
	
//	Bodenvektor mm;
//	mm.fill(0);
//	mm[1] = pNFK2mm(2.0, schlag.fk[1], schlag.pwp[1]);
//	mm[2] = pNFK2mm(0.0, schlag.fk[2], schlag.pwp[2]);
//	mm[3] = pNFK2mm(9.0, schlag.fk[3], schlag.pwp[3]);
//	mm[4] = pNFK2mm(22.0, schlag.fk[4], schlag.pwp[4]);
//	mm[5] = pNFK2mm(99.0, schlag.fk[5], schlag.pwp[5]);
//	currentSoilMoisture = mm;
	auto p = doy2date(absCurrentSoilMoistureDay);
	//		Bodenfeuchte bf = createBF(snru, schlag.bfDatum, schlag.wmm, schlag.fk,
	//															 schlag.pwp);
	//		cout << "BF ist: " << bf.toString() << endl;

	//prognosis of soilmoisture in next 7 days
	//--------------------------------------------------

	//get the dc we would have reached in 7 days
	//this is taken as reference for the prognosis as the machinery on the plots
	//and the farmers themselfs can't use any better resolution
	//thus the resolution of berest (regarding dc states) is 7 days
	DCWRes res5 = nextDC(plantWiDat.dc2day, schlag.knownDC, schlag.knownDCDay,
											 schlag.absCalculationDay+7);
	schlag.knownDC = res5.nextDC;
	schlag.knownDCDay = res5.absNextDCDay;

	//even though we just have a 7 day resolution regarding the dc states
	//we need to get the current exact relative dc day for some of the other
	//needed curves, as these could use different milestones than the dc2day curve
	int relMilestoneDCDay = round(interpolatedValue(plantWiDat.dc2day, schlag.knownDC)+0.1);
	int relDCDay = relMilestoneDCDay + (absCurrentSoilMoistureDay - schlag.knownDCDay);

	Bodenfeuchte bf = createBF(snru, absCurrentSoilMoistureDay, currentSoilMoisture,
														 schlag.fk, schlag.pwp);
	auto aktDCDate = doy2date(schlag.knownDCDay);
	output << schlag.knownDC << "/" << aktDCDate.first << "." << aktDCDate.second << ".   "
		<< bf.pNFK_0_30cm << "  " << bf.pNFK_30_60cm << "    ";

	//calculate the soil moisture we'd have in 7 days given a
	//5 day weather forecast
	BFxRes res4 = bfXTage(7, absCurrentSoilMoistureDay, relDCDay, currentSoilMoisture,
												schlag, true, plantWiDat, im, gaben, weather, out);
	QuType qu;
	qu.qu00 = res4.quAkt;
	qu.qusoll = res4.quSoll;
	Bodenvektor predicted7DaySoilMoisture = res4.soilMoisture;
	int abs7DaySoilMoistureDay = absCurrentSoilMoistureDay + 7;

	Bodenfeuchte bf2 = createBF(snru, abs7DaySoilMoistureDay, predicted7DaySoilMoisture,
															schlag.fk, schlag.pwp);
	//cout << "BF wird: " << bf2.toString() << endl;
	output << bf2.pNFK_0_30cm << "  " << bf2.pNFK_30_60cm << "  ";

	//calculate the irrigation recommendation
	//-----------------------------------------

	SekTyp1 schSrec;
	schSrec.empf_mm = 0;

	if(schlag.beFlaeche > 0 && schlag.beFlaeche <= schlag.faFlaeche)
	{
		TechnTyp schlTechn = getValue(technologie(snru, absCurrentSoilMoistureDay,
																							currentSoilMoisture, schlag, weather));

		if(qu.qu00 < qu.qusoll)
		{
			if(schlTechn.hhMin == 0)
				schlTechn.empfText = "Bf-hoch";
			else if(schlTechn.hhMax >= schlTechn.hhMin)
			{
				HRes res = hohe(relDCDay, currentSoilMoisture, schlag, qu.qusoll,
												schlTechn, plantWiDat, im, weather, out);
				qu = res.qu;
				schlTechn = res.schlTechn;
			}
			else
				schlTechn.empfText = "Tech.min";
		}
		else if(qu.qusoll < 0.1)
			schlTechn.empfText = "Entw/Zeitr";
		else
			schlTechn.empfText = "Bf-opt";
		//		SpeichereEmpf(iSnru);

		schSrec.empf_mm = schlTechn.hhOpt;
		schSrec.empf_text = schlTechn.empfText;

		output << "X " << schSrec.empf_mm << " " << schlTechn.hhMax << " " << schSrec.empf_text;
	}

	cout << output.str() << endl;

	ofs.close();

	cout << "Press button." << endl;
	getchar();
}
