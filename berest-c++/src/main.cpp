#include <iostream>
#include <map>

#include "boost/foreach.hpp"

#include "berest.h"
#include "helper.h"
#include "client/dbclient.h"

using namespace Berest;
using namespace std;
using namespace mongo;
using namespace Tools;

void run() 
{
	DBClientConnection& c = dbConnection();

	BSONObj p = BSON( "name" << "Joe" << "age" << 33 );
	c.insert("tutorial.persons", p);
	c.insert("tutorial.persons", BSON( "name" << "Michael" << "age" << 34));

	cout << "count:" << c.count("tutorial.persons") << endl;

}

void create()
{
//	BtType b;
//	b.btkbz = "test";
//	b.anzMest = 1;
//	b.bzNr = "5";
//	b.krNr = "9";
//	b.raNr = "";
//	btTypSpeichern(b);

	Schlagdaten s1;
	s1.schlagnummer = "0400";
	s1.faFlaeche = 1.0;
	s1.beFlaeche = 1.0;
	s1.nrNieMst = 1;
	s1.faNummer = "0703";
	s1.faAf = "1";
	s1.faVz = "0";
	s1.symbol = "KART3";
	s1.stTyp = 6212;
	s1.hangN = 1;
	array<int, 3+1> t1 = {-9999, 3, 3, 9};
	s1.dicke3 = t1;
	array<double, 3+1> t12 = {-9999.9, 17.7, 13.7, 15.7};
	s1.fk3 = t12;
	array<double, 3+1> t13 = {-9999.9, 3.4, 2.9, 3.8};
	s1.pwp3 = t13;
//	s1.bfDatum = -366;
//	s1.anzGaben = 0;
	s1.knownDC = 1;
	s1.knownDCDay = date2doy(18,4);
	schlagSpeichern(s1);

	BMTyp bm1;
	bm1.snru = 400;
	bm1.snrus = "0400";
	bm1.tag = date2doy(31,3);
	bm1.dc = 45;
	bm1.flaeche = 1.0;
	bm1.grws = 20.0;
	bm1.einheit = pFK;
	bm1.dicke2wert[3] = 80.0;
	bm1.dicke2wert[6] = 90.0;
	bm1.dicke2wert[9] = 100.0;
	bm1.dicke2wert[15] = 100.0;
	bodenfeuchteSpeichern(bm1);

	TechnTyp tt1;
	tt1.turnus = 4;
	tt1.hhMin = 5;
	tt1.hht = 30;
	tt1.hhNorm = 20;
	tt1.hhStufung = 5;
	technikSpeichern(400, tt1);

	//-----------------------------------------------

	Schlagdaten s2;
	s2.schlagnummer = "0010";
	s2.faFlaeche = 1.0;
	s2.beFlaeche = 1.0;
	s2.nrNieMst = 1;
	s2.faNummer = "0703";
	s2.faAf = "1";
	s2.faVz = "0";
	s2.symbol = "KART3";
	s2.stTyp = 6212;
	s2.hangN = 1;
	array<int, 3+1> t2 = {-9999, 3, 3, 9};
	s2.dicke3 = t2;
	array<double, 3+1> t22 = {-9999.9, 18, 14, 16};
	s2.fk3 = t22;
	array<double, 3+1> t23 = {-9999.9, 3, 3, 4};
	s2.pwp3 = t23;
//	s2.bfDatum = -366;
//	s2.anzGaben = 0;
	s2.knownDC = 1;
	s2.knownDCDay = date2doy(18,4);
	schlagSpeichern(s2);

	BMTyp bm2;
	bm2.snru = 10;
	bm2.snrus = "0010";
	bm2.tag = date2doy(31,3);
	bm2.dc = 45;
	bm2.flaeche = 1.0;
	bm2.grws = 20.0;
	bm2.einheit = pNFK; //=2
	bm2.dicke2wert[15] = 0.0;
	bodenfeuchteSpeichern(bm2);

	TechnTyp tt2;
	tt2.turnus = 4;
	tt2.hhMin = 5;
	tt2.hht = 30;
	tt2.hhNorm = 20;
	tt2.hhStufung = 5;
	technikSpeichern(10, tt2);

	//-----------------------------------------------

	Schlagdaten s3;
	s3.schlagnummer = "0020";
	s3.faFlaeche = 1.0;
	s3.beFlaeche = 1.0;
	s3.nrNieMst = 1;
	s3.faNummer = "0703";
	s3.faAf = "1";
	s3.faVz = "0";
	s3.symbol = "KART3";
	s3.stTyp = 6212;
	s3.hangN = 1;
	array<int, 3+1> t3 = {-9999, 3, 3, 9};
	s3.dicke3 = t3;
	array<double, 3+1> t32 = {-9999.9, 15, 15, 15};
	s3.fk3 = t32;
	array<double, 3+1> t33 = {-9999.9, 3, 3, 3};
	s3.pwp3 = t33;
//	s3.bfDatum = -366;
//	s3.anzGaben = 0;
	s3.knownDC = 1;
	s3.knownDCDay = date2doy(18,4);
	schlagSpeichern(s3);

	BMTyp bm3;
	bm3.snru = 20;
	bm3.snrus = "0020";
	bm3.tag = date2doy(31,3);
	bm3.dc = 45;
	bm3.flaeche = 1.0;
	bm3.grws = 20.0;
	bm3.einheit = pNFK; //=2
	bm3.dicke2wert[15] = 0.0;
	bodenfeuchteSpeichern(bm3);

	TechnTyp tt3;
	tt3.turnus = 4;
	tt3.hhMin = 5;
	tt3.hht = 30;
	tt3.hhNorm = 20;
	tt3.hhStufung = 5;
	technikSpeichern(20, tt3);

	//-----------------------------------------------

	Schlagdaten s4;
	s4.schlagnummer = "0030";
	s4.faFlaeche = 1.0;
	s4.beFlaeche = 1.0;
	s4.nrNieMst = 1;
	s4.faNummer = "0101";
	s4.faAf = "1";
	s4.faVz = "0";
	s4.symbol = "WW";
	s4.stTyp = 6212;
	s4.hangN = 1;
	array<int, 3+1> t4 = {-9999, 3, 3, 9};
	s4.dicke3 = t4;
	//bersim
//	array<double, 3+1> t42 = {-9999.9, 17.7, 13.7, 15.7};
	array<double, 3+1> t42 = {-9999.9, 17.7, 13.7, 15.7};
	s4.fk3 = t42;
	array<double, 3+1> t43 = {-9999.9, 3.4, 2.9, 3.8};
	s4.pwp3 = t43;
	//bersim
//	s4.knownDC = 30;
//	s4.knownDCDay = date2doy(24,3);
	s4.knownDC = 31;
	s4.knownDCDay = date2doy(30,4);
	schlagSpeichern(s4);

	BMTyp bm4;
	bm4.snru = 30;
	bm4.snrus = "0030";
	//bersim
//	bm4.tag = date2doy(1,4);
	bm4.tag = date2doy(31,3);
	bm4.dc = 45;
	bm4.flaeche = 1.0;
	bm4.grws = 20.0;
	bm4.einheit = pFK; //=2
//	bm4.dicke2wert[3] = 90.0;
//	bm4.dicke2wert[6] = 85.0;
//	bm4.dicke2wert[15] = 85.0;
	bm4.dicke2wert[3] = 80.0;
	bm4.dicke2wert[6] = 90.0;
	bm4.dicke2wert[9] = 100;
	bm4.dicke2wert[15] = 100.0;
	bodenfeuchteSpeichern(bm4);

	TechnTyp tt4;
	tt4.turnus = 4;
	tt4.hhMin = 5;
	tt4.hht = 30;
	tt4.hhNorm = 20;
	tt4.hhStufung = 5;
//	tt4.turnus = 7;
//	tt4.hhMin = 5;
//	tt4.hht = 40;
//	tt4.hhNorm = 25;
//	tt4.hhStufung = 5;
	technikSpeichern(30, tt4);

	//bersim
//	EMType1 g41;
//	g41.snru = 30;
//	g41.beginn = date2doy(21,5);
//	g41.ende = g41.beginn;
//	g41.menge = 40;
//	gabeSpeichern(g41);

//	EMType1 g42;
//	g42.snru = 30;
//	g42.beginn = date2doy(31,5);
//	g42.ende = g42.beginn;
//	g42.menge = 15;
//	gabeSpeichern(g42);

//	EMType1 g43;
//	g43.snru = 30;
//	g43.beginn = date2doy(10,6);
//	g43.ende = g43.beginn;
//	g43.menge = 40;
//	gabeSpeichern(g43);

	//-----------------------------------------------

	WiDatTyp wd;
	//silomais
	wd.faNrAfVz = "1030/1/0";
	wd.dc2day[20] = 1;
	wd.dc2day[30] = 15;
	wd.dc2day[40] = 25;
	wd.dc2day[50] = 50;
	wd.dc2day[60] = 70;
	wd.dc2day[70] = 80;
	wd.dc2day[80] = 120;
	wd.dc2day[90] = 150;
	wd.dc2name[20] = "Aussaat";
	wd.dc2name[30] = "Aufgang";
	wd.dc2name[40] = "4-Blattstadium";
	wd.dc2name[50] = "8-Blattstadium";
	wd.dc2name[60] = "Fahnenschieben";
	wd.dc2name[70] = "Bluete";
	wd.dc2name[80] = "Milchreife";
	wd.dc2name[90] = "Erntereife";
	wd.day2bd[15] = 0;
	wd.day2bd[80] = 1;
	wd.day2n[15] = 2;
	wd.day2n[80] = 9;
	wd.day2n[90] = 10;
	wd.day2t[60] = 1.0;
	wd.day2t[85] = 1.2;
	wd.day2t[140] = 1.2;
	wd.day2t[170] = 0.1;
	wd.day2qu[40] = 0.0;
	wd.day2qu[60] = 0.6;
	wd.day2qu[90] = 0.9;
	wd.day2qu[115] = 0.9;
	wd.day2qu[140] = 0.7;
	wd.day2qu[150] = 0;
	wd.day2ef[1] = 0.13;
	wiDatSpeichern(wd);

	wd = WiDatTyp();
	//Kartoffeln Reifegruppe 3
	wd.faNrAfVz = "0703/1/0";
	wd.dc2day[1] = 1;
	wd.dc2day[5] = 35;
	wd.dc2day[10] = 50;
	wd.dc2day[15] = 70;
	wd.dc2day[17] = 85;
	wd.dc2day[28] = 115;
	wd.dc2day[31] = 125;
	wd.dc2name[1] = "Pflanzung";
	wd.dc2name[5] = "Auflaufen";
	wd.dc2name[10] = "Sichtbarwerden der 1. Knospen";
	wd.dc2name[15] = "Bluehbeginn";
	wd.dc2name[17] = "Bluehende";
	wd.dc2name[28] = "Braunreife";
	wd.dc2name[31] = "Krautabtoetung Erntebeginn";
	wd.day2bd[35] = 0;
	wd.day2bd[65] = 1;
	wd.day2bd[120] = 1;
	wd.day2bd[135] = 0;
	wd.day2n[35] = 2;
	wd.day2n[50] = 5;
	wd.day2n[60] = 7;
	wd.day2t[50] = 1;
	wd.day2t[70] = 1.2;
	wd.day2t[115] = 1.2;
	wd.day2t[120] = 1;
	wd.day2qu[35] = 0.0;
	wd.day2qu[50] = 0.2;
	wd.day2qu[55] = 0.7;
	wd.day2qu[85] = 0.8;
	wd.day2qu[105] = 0.8;
	wd.day2qu[110] = 0.7;
	wd.day2qu[115] = 0;
	wd.day2ef[1] = 0.10;
	wiDatSpeichern(wd);

	//bersim
//	wd = WiDatTyp();
//	//Winterweizen Erntejahr
//	wd.faNrAfVz = "0101/1/0";
//	wd.dc2day[21] = 60;
//	wd.dc2day[31] = 128;
//	wd.dc2day[51] = 158;
//	wd.dc2day[61] = 165;
//	wd.dc2day[75] = 180;
//	wd.dc2day[92] = 215;
//	wd.dc2name[21] = "Bestockungsbeginn";
//	wd.dc2name[31] = "Schossbeginn";
//	wd.dc2name[51] = "Ährenschieben";
//	wd.dc2name[61] = "Blüte";
//	wd.dc2name[75] = "Milchreife";
//	wd.dc2name[92] = "Todreife";
//	wd.day2bd[30] = 0.6;
//	wd.day2bd[115] = 0.8;
//	wd.day2bd[125] = 1.0;
//	wd.day2n[90] = 6;
//	wd.day2n[120] = 8;
//	wd.day2n[150] = 10;
//	wd.day2n[170] = 12;
//	wd.day2t[118] = 1;
//	wd.day2t[128] = 1.2;
//	wd.day2t[190] = 1.2;
//	wd.day2t[200] = 1;
//	wd.day2t[220] = 0.1;
//	wd.day2qu[75] = 0.0;
//	wd.day2qu[90] = 0.2;
//	wd.day2qu[128] = 0.8;
//	wd.day2qu[180] = 0.8;
//	wd.day2qu[200] = 0.6;
//	wd.day2qu[210] = 0;
//	wd.day2ef[1] = 0.17;
//	wiDatSpeichern(wd);

	//berest
	wd = WiDatTyp();
	//Winterweizen Erntejahr
	wd.faNrAfVz = "0101/1/0";
	wd.dc2day[21] = 60;
	wd.dc2day[31] = 128;
	wd.dc2day[51] = 158;
	wd.dc2day[61] = 165;
	wd.dc2day[75] = 180;
	wd.dc2day[92] = 215;
	wd.dc2name[21] = "Bestockungsbeginn";
	wd.dc2name[31] = "Schossbeginn";
	wd.dc2name[51] = "Ährenschieben";
	wd.dc2name[61] = "Blüte";
	wd.dc2name[75] = "Milchreife";
	wd.dc2name[92] = "Todreife";
	wd.day2bd[30] = 0.6;
	wd.day2bd[115] = 0.8;
	wd.day2bd[125] = 1.0;
	wd.day2n[90] = 6;
	wd.day2n[120] = 8;
	wd.day2n[150] = 12;
	wd.day2n[180] = 18;
	wd.day2t[118] = 1;
	wd.day2t[128] = 1.2;
	wd.day2t[180] = 1.6;
	wd.day2t[210] = 1;
	wd.day2t[220] = 0.5;
	wd.day2qu[75] = 0.0;
	wd.day2qu[90] = 0.2;
	wd.day2qu[128] = 0.8;
	wd.day2qu[180] = 0.8;
	wd.day2qu[200] = 0.6;
	wd.day2qu[210] = 0;
	wd.day2ef[1] = 0.17;
	wiDatSpeichern(wd);

	wd = WiDatTyp();
	//Brache
	wd.faNrAfVz = "0000/1/0";
	wd.dc2day[1] = 1;
	wd.dc2name[1] = "Brache";
	wd.day2bd[1] = 0;
	wd.day2n[1] = 1;
	wd.day2t[1] = 1;
	wd.day2qu[1] = 0;
	wd.day2ef[1] = 0;
	wiDatSpeichern(wd);
}

int main(int argc, char *argv[])
{
	if(argc > 1)
	{
		string command(argv[1]);
		if(command == "create")
		{
			create();
		}
		// addw | addp month "liste von wetter/prognose daten"
		else if(command == "addw" || command == "addp")
		{
			if(argc > 3)
			{
				int month = atoi(argv[2]);
				string data(argv[3]);

				int lastDay = 0;
				BOOST_FOREACH(WetterTyp wt, parseWeatherData(data))
				{
					if(lastDay > wt.tag)
						month++;

					lastDay = wt.tag;
					wt.istPrognose = command == "addp";
					int dim = wt.tag;
					wt.tag = date2doy(wt.tag, month);
					wetterSpeichern(wt.tag, wt);
					cout << (wt.istPrognose ? "prognose:" : "wetter:")
							 << " date: " << dim << "." << month
							 << " tag: " << wt.tag << " nied: " << wt.niederschlag
							 << " verd: " << wt.verdunstung << " hinzugefügt" << endl;
				}
			}
		}
		else if(command == "addg")
		{
			if(argc > 2)
			{
				string data(argv[2]);

				EMType1 gabe = parseIrrigationDonation(data);
				if(gabe.snru > 0)
				{
					gabeSpeichern(gabe);
					auto b = doy2date(gabe.beginn);
					auto e = doy2date(gabe.ende);
					cout << "snru: " << gabe.snru
						<< " begin: " << b.first << "." << b.second << ". (" << gabe.beginn << ")" 
						<< " end: " << e.first << "." << e.second << ". (" << gabe.ende << ")"
						<< " irrArea: " << gabe.beafl
						<< " amount: " << gabe.menge << endl;
				}
			}
		}
		// run rechentag
		else if(command == "run")
		{
			if(argc > 1)
			{
				Schlagdaten s = getValue(schlagLesen("0400"));

				//rechentag is either obtained from user or automatically
				//chosen to be the day after our last available weather data
				if(argc > 2)
					s.absCalculationDay = atoi(argv[2]);

				vector<WetterTyp> wts = wetterLesen(1, 0, asc, false);
				vector<WetterTyp> wtsp = wetterLesen(1, 0, asc, true);
				if(!wts.empty())
				{
					s.minWetter = wts.back().tag;
					if(!s.absCalculationDay)
						s.absCalculationDay = s.minWetter + 1;
					else
						//remove all the unnecessary weather data from vector until prognosis time
						wtsp.erase(wtsp.begin() + (s.absCalculationDay - wtsp.front().tag),
											 wtsp.end());

//					BtType b = getValue(btTypLesen("test"));
//					WiDatTyp wd = getValue(wiDatLesen(s.faNrAfVz()));

					run(s, wtsp, sprinkleLosses);
				}
			}
		}
		else if(command == "runsim")
		{
			if(argc > 1)
			{
				Schlagdaten s = getValue(schlagLesen("0030"));

				//rechentag is either obtained from user or automatically
				//chosen to be the day after our last available weather data
				if(argc > 2)
					s.absCalculationDay = atoi(argv[2]);

				string precipFile = "precip89.WET", evapFile = "evap89.WET";
				if(argc > 4)
				{
					precipFile = argv[3];
					evapFile = argv[4];
				}

				vector<WetterTyp> wts = loadBersimWeatherData(precipFile, evapFile);
				if(!wts.empty())
				{
					s.minWetter = wts.back().tag;
					if(!s.absCalculationDay)
						s.absCalculationDay = s.minWetter + 1;

					run(s, wts, sprinkleLosses);
				}
			}
		}
		else if(command == "run-compare")
		{
			if(argc > 1)
			{
				Schlagdaten s = getValue(schlagLesen("0400"));

				//rechentag is either obtained from user or automatically
				//chosen to be the day after our last available weather data
				if(argc > 2)
					s.absCalculationDay = atoi(argv[2]);

				string csvFile = "klima-potsdam-2006.csv";
				if(argc > 3)
					csvFile = argv[3];
				
				vector<WetterTyp> wts = loadCSVWeatherData(csvFile);
				if(!wts.empty())
				{
					s.minWetter = wts.back().tag;
					if(!s.absCalculationDay)
						s.absCalculationDay = s.minWetter + 1;

					run(s, wts, sprinkleLosses);
				}
			}
		}
	}

}
