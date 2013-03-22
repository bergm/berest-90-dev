#ifndef BEREST_H
#define BEREST_H

#include <string>
#include <array>
#include <map>
#include <sstream>

#include "client/dbclient.h"

namespace Berest
{
	enum SortOrder { asc = 1, desc = -1 };

	struct X 
	{
		//int dummy;
	};

	const int noOfSoilLayers = 5;

	typedef std::array<double,noOfSoilLayers+1> Bodenvektor; //[tiefenstufen]float64
	typedef std::array<double,3+1> A3ofreal;

	Bodenvektor operator*(const Bodenvektor& v1, const Bodenvektor& v2);

	double sum(const Bodenvektor& v);

	struct Schlagdaten : public X
	{
		Schlagdaten() :
			faFlaeche(0.0),
			beFlaeche(0.0),
			nrNieMst(0),
			stTyp(0),
			hangN(0),
			verdTiefe(0),
			verdFlae(0.0),
			//bfDatum(0),
			knownDCDay(0),
			knownDC(0),
			groundwaterlevel(0),
			//anzGaben(0),
			absCalculationDay(0),
			minWetter(0)
		{
			fk3.fill(0);
			pwp3.fill(0);
			wmm.fill(0);
			fk.fill(0);
			pwp.fill(0);
			lambdaWithoutCorrection.fill(0);
		}

		std::string schlagnummer;
		double faFlaeche; //Schlagfläche
		double beFlaeche; //Beregnungsfläche
		int nrNieMst; //Nr der Niederschlagsmessstelle
		std::string faNummer; //Fruchtart Nr.
		std::string faAf; //Fruchtart Anbauform
		std::string faVz; //Fruchtart Verwendungszweck
		std::string faName; //Fruchtart-Name
		std::string symbol;
		int stTyp; //Standort-Typ
		int hangN; //Hangneigung
		std::array<int,3+1> dicke3; //Dicke
		A3ofreal fk3; //Feldkapazität [1-3]
		A3ofreal pwp3; //Permanenter Welkepunkt der Bodenschichten [1-3]
		int verdTiefe; //Schadverdichtungstiefe
		double verdFlae; //Schadverdichtungsfläche
//		int bfDatum; //Datum der letzen Berechnung
//		Bodenvektor wmm; //Aktuelle Bodenfeuchte
		int knownDCDay; //Tag zu aktdc
		int knownDC; //DC
		int groundwaterlevel; //Grundwasserstand
//		int anzGaben; //Anzahl der Regengaben
//		std::array<int, tiefenstufen+1> layerDepths;

		int absCalculationDay;
		int minWetter;

		int absPreCalculationDay() const { return absCalculationDay - 1; }

		Bodenvektor wmm;

		std::string faNrAfVz()
		{
			std::ostringstream ss;
			ss << faNummer << "/" << faAf << "/" << faVz;
			return ss.str();
		}

		void setup();

		Bodenvektor lambda(int aktTag) const;

		int ivd() const
		{
			return (verdFlae > 0.5 * faFlaeche ? verdTiefe : 200) / 10.0;
		}

		Bodenvektor fk; //Feldkapazität
		Bodenvektor pwp; //potentieller Welkepunkt
		Bodenvektor lambdaWithoutCorrection;

		std::map<int, std::string> transaction2info;
	};

	struct WetterTyp
	{
		WetterTyp()
			: tag(0),
				verdunstung(0.0),
				niederschlag(0.0),
				istPrognose(false)
		{}

		int tag;
		double verdunstung;
		double niederschlag;
		bool istPrognose;
	};

	struct WiDatTyp
	{
		WiDatTyp()
			:	satznummer(0)//,
				//dci(0), bdi(0), ni(0), ti(0), qui(0), efi(0), dcPName(0),
				//dcn(0), bdn(0), nn(0), tn(0), qun(0), efn(0), dcLName(0)
		{}

		std::string faNrAfVz; //Fruchtart Nr Anbauform Verwendungszweck
		int satznummer;
		std::string faSymbol; //(*   9  *)
		std::string faName; //(*  16  *)
//		std::array<int,50+1> ikurv; //(* 100  *)
//		std::array<double,50+1> rkurv; //(* 100  *)
//		std::string dcName; //(* 101  *)
//		int dci, bdi, ni, ti, qui, efi, dcPName; //(*   7  *)
//		int dcn, bdn, nn, tn, qun, efn, dcLName; //(*   7  *)

		std::map<int, int> dc2day; //decimal code
		std::map<int, double> day2bd; //bedeckungsgrad
		std::map<int, int> day2n; //entnahmetiefe
		std::map<int, double> day2t; //transpiration
		std::map<int, double> day2qu; //quotient
		std::map<int, double> day2ef; //effektivität
		std::map<int, std::string> dc2name; // name decimal code

	}; // (* 339  Record der Datzei *)

	struct TechnTyp
	{
		TechnTyp()
			: hhMax(0),
				hhMin(0),
				hhn(0),
				hht(0),
				hhNorm(0),
				hhStufung(0),
				turnus(0),
				hhOpt(0)
		{}

		int hhMax;
		int hhMin;
		int hhn;
		int hht;
		int hhNorm;
		int hhStufung;
		int turnus;
		int hhOpt;
		std::string empfText;
	};

	struct QuType
	{
		QuType()
			: qusoll(0.0),
				qu00(0.0),
				quhh(0.0),
				quef(0.0)
		{}

		double qusoll;
		double qu00;
		double quhh;
		double quef; //"{24}"
	};

	enum CodeEinheit { pFK = 1, pNFK = 2, Volp = 3, mm };

	struct BMTyp : public X
	{
		BMTyp()
			: snru(0),
			tag(0),
			dc(0),
			flaeche(0.0)
		{}

		int snru;
		std::string snrus;
		//int code;
		int tag;
		int dc;
		double flaeche;
		int grws; //[dm]
		CodeEinheit einheit;
		std::map<int, double> dicke2wert;
	};

	struct Bodenfeuchte
	{
		Bodenfeuchte() : snru(0), tag(0),
			pNFK_0_30cm(0), pNFK_30_60cm(0), pNFK_0_60cm(0) {}
		int snru;
		int tag;
		double pNFK_0_30cm;
		double pNFK_30_60cm;
		double pNFK_0_60cm;

		std::string toString() const
		{
			std::ostringstream ss;
			ss << "tag: " << tag << " %nFK 0-3: " << pNFK_0_30cm << " %nFK 3-6: " << pNFK_30_60cm;
			return ss.str();
		}
	};

	struct BtType
	{
		BtType()
			: datumWe(0),
				anzMest(0)
		{}

		std::string btkbz;
		int datumWe;
		int anzMest;
		std::string bzNr;
		std::string krNr;
		std::string raNr;
		std::string daWe;
	};

	struct EMType1 : public X
	{
		EMType1()
			: snru(0), 
			code(0),
			beginn(0),
			ende(0),
			arbart(0),
			beafl(1.0),
			material(0),
			menge(0.0),
			qunote(0)
		{}

		int snru;
		int code;
		int beginn;
		int ende;
		std::string kenn;// "char"
		int arbart;
		double beafl;
		int material;
		int menge;
		int qunote;
	};

	struct SekTyp1
	{
		SekTyp1()
			: fgB(0.0),
				bha_Ist(0.0),
				bha_Zug(0.0),
				bha_Soll(0.0),
				m3_Ist(0.0),
				m3_Zug(0.0),
				m3_Soll(0.0),
				lfd_Bha(0.0),
				empf_mm(0),
				gabe_mm(0),
				nrGabeTt(0),
				nrGabe(0),
				bha_I_Ges(0.0),
				mm_I_Ges(0)
		{}

		double fgB; //{ xxx.x }
		double bha_Ist; //{ xxxxx.x }
		double bha_Zug; //{ xxxxx.x             (*) }
		double bha_Soll; //{ xxxxx.x }
		double m3_Ist; //{ xxx }
		double m3_Zug; //{ xxx                 (*) }
		double m3_Soll; //{ xxx }
		double lfd_Bha; //{ xxx.x }
		int empf_mm; //{ xx }
		//{ bis hier zur  bergabe an Seku (*) - nach übergabe löschen }
		std::string empf_Tag; //{ ttmm }
		std::string empf_text; //{ xxxxxxxxxx }
		std::string gabe_Tag; //{ ttmm }
		int gabe_mm; //{ xx }
		int nrGabeTt; //{ xx Nr der letzten Gabe (Zahl der 700-Gruppen)}
		int nrGabe; //{ xx Nr der letzten gültigen Gabe}
		double bha_I_Ges; //{ xxxxx.x }
		int mm_I_Ges; //{ xxx }
	};

	enum IrrigationMode { sprinkleLosses = 0, noSL = 1 };

	void run(Schlagdaten schlag, const std::vector<WetterTyp>& weather, IrrigationMode im);

	mongo::DBClientConnection& dbConnection();

	std::pair<BtType, bool> btTypLesen(std::string btkbz = "test");
	void btTypSpeichern(BtType b, bool isNew = false);

	std::pair<Schlagdaten, bool> schlagLesen(std::string schlagnummer = "0400");
	void schlagSpeichern(const Schlagdaten& s, bool isNew = false);

	std::pair<WiDatTyp, bool> wiDatLesen(std::string faNrAfVz = "1030/1/0");
	void wiDatSpeichern(const WiDatTyp& wd, bool isNew = false);

	std::pair<BMTyp, bool> bodenfeuchteLesen(int schlagnummer = 400, int tag = 0);
	void bodenfeuchteSpeichern(BMTyp bm, bool isNew = false);

	std::pair<Bodenfeuchte, bool> aktPrognBodenfeuchteLesen
	(int snru, int tag, bool readAktFeuchte);
	void aktuelleBFSpeichern(Bodenfeuchte bf, bool isNew = false);
	void prognostischeBFSpeichern(Bodenfeuchte bf, bool isNew = false);

	std::vector<EMType1> gabenLesen(int snru = 400);
	void gabeSpeichern(const EMType1& g, bool isNew = false);
	EMType1 parseIrrigationDonation(std::string data);

	std::pair<TechnTyp, bool> technikLesen(int snru = 400);
	void technikSpeichern(int snru, const TechnTyp& t, bool isNew = false);

	std::vector<WetterTyp> wetterLesen(int station = 1, int tag = 0,
																		 SortOrder so = asc,
																		 bool includePrognosisData = false);
	WetterTyp weatherAt(const std::vector<WetterTyp>& v, int day);
	std::pair<double, double> longtermEvapPrecip(int day);
	void wetterSpeichern(int tag, const WetterTyp& w, int station = 1,
											 bool isNew = false);

	std::vector<WetterTyp> parseWeatherData(const std::string& data);
	std::vector<WetterTyp> loadBersimWeatherData(const std::string& precipFile, 
		const std::string& evapFile);
	std::vector<WetterTyp> loadCSVWeatherData(const std::string& precipFile);

	struct GlobVars
	{
		int rechentag, vorReTag, minWetter;
	};
	std::pair<GlobVars, bool> globVarsLesen();
	void globVarsSpeichern(const GlobVars& gvs, bool isNew = false);

}

#endif // BEREST_H

