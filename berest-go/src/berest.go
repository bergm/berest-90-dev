/**
 * Created by IntelliJ IDEA.
 * User: michael
 * Date: 17.12.2010
 * Time: 10:38:36
 * To change this template use File | Settings | File Templates.
 */
package main

import "fmt"
//import "time"
import "math"
import "container/list"

func main() {
	schlag := Schlagdaten{
		schlagnummer: "0011",
  	faFlaeche: 0.9,
  	beFlaeche: 0.9,
		nrNieMst: 1,
  	faNummer: "627",
 		faAf: "2",
  	faVz: "1",
  	stTyp: 6212,
  	hangN: 3,
  	dicke: [4]int{-9999, 3, 3, 8},
  	fk: [4]float64{-9999.9, 21.0, 20.0, 16.0},
  	pwp: [4]float64{-9999.9, 4.0, 5.0, 6.0}}
	fmt.Printf("schlag: %+v\n", schlag)

	bodenschlag := fkpwla(100, schlag)
	fmt.Printf("bodenschlag: %+v\n", bodenschlag)


  //type xy struct { x,y int }
  
  //y := []xy{ xy{1,2}, xy{2,3}, xy{3,4}}

  //s := []int { 50, 100, 200, 300 }
  //w := []float64 { 50, 100, 200, 300 }

  //x := []int{1,2,3}

  //fmt.Printf("Hello world! %v\n", y)
  //fmt.Printf("round(4.0)=%v\n", round(5.0))
  //res, _ := fvonx(s[:],w[:],40,1)
  //fmt.Printf("fvonx=%v\n", res)

}

const tiefenstufen = 5
var snru string
var iSnru, altiSnru int
var berFlStr string
var glob GlobTyp
var schlFehler SFehlerTyp
var archTC [4]Arch
var minTC Arch
var we30 We30Type
var iPruef int
var feStr string
var aktFaNrAfVz string
var altAktDatum int
var minZus int
var minDatum int
var schlagOk bool
var minWetter int
var abDatum string
var ersterFaTag int
var berInterrupt bool

type We30Type struct {
	tag int
	verd string
	temp,
	minTemp,
	maxTemp string
	nied [5]string
	tagStr string
}


type Bodenvektor [tiefenstufen+1]float64

type Schlagdaten struct {
  schlagnummer string "Schlagnummer"
  faFlaeche float64 "Schlagfäche"
  beFlaeche float64 "Beregnungsfläche"
  //betrId int "Betriebs-ID"
  nrNieMst int "Nr der Niederschlagsmessstelle"
  //wetterstation int "Nr. der Wetterstation"
  //fruchtart_id int "Fruchtart-ID"
  faNummer string "Fruchtart-Nr."
  faAf string
  faVz string
  faNrAfVz string
  faSymbol string "Fruchtart-Symbol"
  faName string "Fruchtart-Name"
  stTyp int "Standort-Typ"
  hangN int "Hangneigung"
  dicke [4]int "Dicke"
  fk [4]float64 "FK"
  pwp [4]float64 "PWP der Bodenschichten"
  verdTiefe int "Schadverdichtungstiefe"
  verdFlae float64 "Schadverdichtungsfläche"
  aktDatum int "Datum der letzen Berechnung"
  bfAkt Bodenvektor "Aktuelle Bodenfeuchte"
  aktDCTag int "Tag zu aktdc"
  aktDC int "DC"
  fka [4]float64
  pwpa [4]float64
  grws int "Grundwasserstand"
  dcstr string "DC zu Ausgabe"
  dcTagStr string "DC-Datum zur Ausgabe"
  anzGaben int "Anzahl der Regengaben"
}

type Bodenschlag struct {
  fk Bodenvektor "Feldkapazität"
  pwp Bodenvektor "potentieller Welkepunkt"
  nwk Bodenvektor "Nutzwasserkapazität"
  fk10 Bodenvektor "Feldkapazität je dm"
  pw10 Bodenvektor "potentielle Welkepunkt je dm"
  pw7 Bodenvektor "modifizierter Arbeitswert"
  pw9 Bodenvektor "modifizierter Arbeitswert"
  fkm9 Bodenvektor "modifizierter Arbeitswert"
  xsm Bodenvektor "modifizierter Arbeitswert"
  spm Bodenvektor "modifizierter Arbeitswert"
  vsgr Bodenvektor "Versickerung ab mm"
  lambda Bodenvektor "Sickerparameter"
  kpo Bodenvektor "modifizierter Arbeitswert"
  kne Bodenvektor "modifizierter Arbeitswert"
  wmm Bodenvektor "Bodenfeuchte operativ"
  grundwastand int "Grundwasserstand"
  ivd int "Versickerungstiefe dm"
}

type WetterTyp struct {
  verdunstung float64
  niederschlag float64
}

type WiDatTyp struct {
  faNrAfVz string
  satznummer int
  faSymbol string "(*   9  *)"
  faName string "(*  16  *)"
  ikurv [50+1]int "(* 100  *)"
  rkurv [50+1]float64 "(* 100  *)"
  dcName string "(* 101  *)"
  dci,bdi,ni, ti,qui,efi,dcPName int "(*   7  *)"
  dcn,bdn,nn,tn,qun,efn,dcLName int " (*   7  *)"
} // (* 339  Record der Datzei *)
//bd = bedeckungsgrad
//dc = decimal code
//t = temperatur


type TechnTyp struct {
  hhMax,
  hhMin,
  hhn,
  hht,
  hhNorm,
  hhStufung,
  turnus,
  hhOpt int
  empfText string
}

type QuType struct {
  qusoll,
  qu00,
  quhh,
  quef float64 "{24}"
}

type GlobvarTyp struct {
  datum string
  rechentag int
  vorDatum string
  vorReTag int
  fruchtart string
  firstinFeld, einfuegen bool
  //Ramdisk     : Char;
  schlagvlw string
  jjff string
}
type PathTyp struct {
  ods string
  wdat string
  list string
  seku string
}
type GlobTyp struct {
  vari GlobvarTyp
  path PathTyp
  systemdatum string
  pckfilename string
  cfgFilename string
}


type KopfTyp struct {
  liKopf [5]string
  taKopf [5]string
  unterstreichungsbreite int
  fensterSpalten int
  fensterZeilen int
  aktBildSpalte int
  lw string
  pfad string
  name string
  liste string
  listeOpen bool
}
type Tabft struct {
  gewaehlt bool
  breite,
  zeile,
  seitenlaenge,
  seite int
  nl bool
}
type DatTyp [4]Tabft
type TabTyp struct {
  ta KopfTyp
  dat DatTyp
}

type SFNum [21]int
type SFehlerTyp struct {
  anz int
  num SFNum
}


type BMTyp1 struct {
  snru,
  code,
  tag,
  dc int
  flaeche float64
  wert [4]float64
}

type BMTyp2 struct {
  snru string
  code string
  tag int
  dc string
  flaeche string
  wert [4]string
  tagS string
}

type BtType struct {
	btkbz string
	datumWe int
	anzMest int
	bzNr,
	krNr string
	raNr string
	daWe string
}
//var btrec BtType

type EMType1 struct {
	snru int
	code int
	beginn int
	ende int
	kenn string "char"
	arbart int
	beafl float64
	material int
	menge float64
	qunote int
}

/*
type EMType2 struct {
	eMsnru    : String[4];
	eMcode    : String[4];
	EMbeginn  : integer;
	EMende    : integer;
	EMkenn    : char;
	EMarbart  : String[3];
	EMbeafl   : String[5];
	EMmaterial: String[4];
	EMmenge   : String[6];
	EMqunote  : integer;
	EMbeg     : String[4];
	EMend    : String[4];
}
*/

type Arch struct {
	tag,
	code int
}

type SekTyp1 struct {
	fgB float64 //{ xxx.x }
	bha_Ist float64 //{ xxxxx.x }
	bha_Zug float64 //{ xxxxx.x             (*) }
	bha_Soll float64 //{ xxxxx.x }
	m3_Ist float64 //{ xxx }
	m3_Zug float64 //{ xxx                 (*) }
	m3_Soll float64 //{ xxx }
	lfd_Bha float64 //{ xxx.x }
	empf_mm int //{ xx }
	//{ bis hier zur  bergabe an Seku (*) - nach bergabe l”schen }
	empf_Tag string //{ ttmm }
	empf_text string //{ xxxxxxxxxx }
	gabe_Tag string //{ ttmm }
	gabe_mm int //{ xx }
	nrGabeTt int //{ xx Nr der letzten Gabe (Zahl der 700-Gruppen)}
	nrGabe int //{ xx Nr der letzten gltigen Gabe}
	bha_I_Ges float64 //{ xxxxx.x }
	mm_I_Ges int //{ xxx }
}

var schSrec SekTyp1


func minI(left, right int) (res int) {
	if left < right {
		res = left
	} else {
		res = right
	}
	return
}

func abs(value int) int {
	return int(math.Fabs(float64(value)))
}

func round(value float64) (res float64) {
  x := math.Trunc((value - math.Trunc(value)) * 10)
  if x >= 5 {
    res = math.Trunc(value + 0.5)
  } else {
    res = math.Trunc(value)
  }
  return res
}

// Procedure zur Berechnung von Funktionswerten
// einer Stuetzstellenfunktion
func fvonx(stuetz []int, wert []float64, x int, faktor float64) (res float64, error bool) {
  if ls, lw := len(stuetz), len(wert); ls < 2 || lw < 2 || ls != lw {
    return 0, true
  }

  if x < stuetz[0] {
    return wert[0], false
  }
  if li := len(stuetz)-1; x > stuetz[li] {
    return wert[li], false
  }

  i := 0
  for ; i < len(stuetz) && stuetz[i] <= x; i++ {}

  sif, sim1f, xf := float64(stuetz[i]), float64(stuetz[i-1]), float64(x)
  res = faktor*(wert[i-1]+(xf-sim1f)/(sif-sim1f)*(wert[i]-wert[i-1]))

  return res, false
}

//Procedure FKPWLA(aktTag:integer)
//BEREITSTELLEN DER BODENPARAMETER 
func fkpwla(aktTag int, schlag Schlagdaten) (bodenschlag Bodenschlag) {
  var idick [4]int
  var izaecm int

  type TW struct { tag, wert int }

  fkuTage := []int{90,135,175,210,240,282,306}
  kfuWerte := []float64{135,135,17,177,17,13,95}

  vsGrTage := []int{80,140,100,200}
  vsGrWerte := []float64{140,7,10,8}

  type FKLA struct {fkw, law float64 }

  fkpwpl := [10][2]FKLA{
    [2]FKLA{ FKLA{fkw:100,law:115}, FKLA{fkw: 90,law:115}},
    [2]FKLA{ FKLA{fkw:140,law: 68}, FKLA{fkw:120,law: 68}},
    [2]FKLA{ FKLA{fkw:200,law: 50}, FKLA{fkw:180,law: 50}},
    [2]FKLA{ FKLA{fkw:240,law: 33}, FKLA{fkw:220,law: 33}},
    [2]FKLA{ FKLA{fkw:260,law: 33}, FKLA{fkw:250,law: 33}},
    [2]FKLA{ FKLA{fkw:300,law: 25}, FKLA{fkw:280,law: 25}},
    [2]FKLA{ FKLA{fkw:312,law: 25}, FKLA{fkw:364,law: 25}},
    [2]FKLA{ FKLA{fkw:327,law: 17}, FKLA{fkw:344,law: 17}},
    [2]FKLA{ FKLA{fkw:356,law: 15}, FKLA{fkw:373,law: 15}},
    [2]FKLA{ FKLA{fkw:1000,law: 15}, FKLA{fkw:1000,law: 15}}}

  if schlag.verdFlae > 0.5*schlag.faFlaeche {
    bodenschlag.ivd = schlag.verdTiefe
  } else {
    bodenschlag.ivd = 200
  }

  lambkor, _ := fvonx(fkuTage[:],kfuWerte[:],aktTag,0.01)
  vspz, _ := fvonx(vsGrTage[:1],vsGrWerte[:1],aktTag,0.1)
  vsp2, _ := fvonx(vsGrTage[2:],vsGrWerte[2:],aktTag,0.1)

  for isp:=1; isp<=3; isp++ {
    if sd := schlag.dicke[isp]; 1 <= sd && sd <= 9 {
      idick[isp] = idick[isp-1] + schlag.dicke[isp]*10
    } else {
      idick[isp] = 0
    }
  }

  if idick[2]==150 {
  } else {
    if idick[3]==150 {
    } else {
      if idick[3] > 0 {
        idick[3] = 150
      } else {
        idick[2] = 150
      }
    }
  }

  izaecm = 0
  is  := 1
  isp := 0

  for isc:=1; isc<=5; isc++ {
    bodenschlag.lambda[isc] = 10
    isd := isc*10

    fmt.Printf("1")

    for j:=1; j<=isd; j++ {
      izaecm++
      if izaecm<=idick[isp] {
        if izaecm==31 {
          isp--
        } else {
          goto summe
        }
      }

			fmt.Printf("2")

      if izaecm==31 {
        is = 2
      }

      var ladm float64
      stt := schlag.nrNieMst
      switch {
        case 1121<=stt && stt<=1323:
          ladm = 100
        case 713<=stt && stt<=723:
          ladm = 5
        case true:
          i := 1
          for ; round(schlag.fk[isp+1]*10)<=fkpwpl[i][is].fkw; i++ {}
          ladm = fkpwpl[i][is].law
      }

      if bodenschlag.ivd < izaecm {
        ladm = 1
      }
      ladm = ladm*0.01;
      isp++

			fmt.Printf("3")

      summe:
      bodenschlag.fk[isc] = bodenschlag.fk[isc]  + schlag.fk[isc] * 0.1
      bodenschlag.pwp[isc] = bodenschlag.pwp[isc] + schlag.pwp[isc] * 0.1
      if ladm < bodenschlag.lambda[isc] {
        bodenschlag.lambda[isc] = ladm
      }
    }

    bodenschlag.fk10[isc] = bodenschlag.fk[isc] / float64(isc)
    bodenschlag.nwk[isc] = bodenschlag.fk[isc] - bodenschlag.pwp[isc]
    bodenschlag.pw10[isc] = bodenschlag.pwp[isc] / float64(isc)
    bodenschlag.spm[isc] =
      (bodenschlag.fk10[isc]+40.667-bodenschlag.fk10[isc]*0.408)*float64(isc);

    var vs float64
    if izaecm>30 {
      vs = vsp2
    } else {
      vs = vspz
    }
    
    if vs<=1 {
      pwfk := bodenschlag.pwp[isc] / bodenschlag.fk[isc]-1+vs;
      if pwfk>0 {
        vs = vs + (1-vs)*pwfk*0.95/(0.66-0.30)
      }
    }

    bodenschlag.vsgr[isc] = bodenschlag.nwk[isc]*vs+bodenschlag.pwp[isc];
    bodenschlag.lambda[isc] = bodenschlag.lambda[isc]/float64(isc*isc)*lambkor;
    bodenschlag.pw7[isc] = bodenschlag.pwp[isc]-0.12*bodenschlag.nwk[isc];
    bodenschlag.pw9[isc] = bodenschlag.pwp[isc]-0.04*bodenschlag.nwk[isc];
    bodenschlag.fkm9[isc] = 1.0/(bodenschlag.fk[isc]-bodenschlag.pw9[isc]);
    if bodenschlag.fk10[isc]<10 || bodenschlag.fk10[isc]>46 {
      bodenschlag.xsm[isc] = bodenschlag.nwk[isc]*0.81+bodenschlag.pwp[isc]
    } else {
      bodenschlag.xsm[isc] =
                      (67.77+
                       3.14      *bodenschlag.fk10[isc]-
                       0.2806    *math.Pow(bodenschlag.fk10[isc],2)+
                       0.008131  *math.Pow(bodenschlag.fk10[isc],3)-
                       0.0000735 *math.Pow(bodenschlag.fk10[isc],4))*
                       bodenschlag.nwk[isc]*0.01+bodenschlag.pwp[isc]
      bodenschlag.kpo[isc] = (bodenschlag.fk[isc] -bodenschlag.xsm[isc])*0.0666
      bodenschlag.kne[isc] = (bodenschlag.xsm[isc]-bodenschlag.pwp[isc])*0.3333
    }
  }

  bodenschlag.grundwastand = schlag.grws
  bodenschlag.ivd = int(bodenschlag.ivd / 10)

  return
}


func holeWetter(tag, nr int) (wetter WetterTyp) {
  if tag<366 && tag>=glob.vari.rechentag {
    nr = 1
  }

  //ods('le',Nr,'rc=',tag,tag,Wetter);

  if tag<366 && tag>glob.vari.rechentag+4 {
    wetter.niederschlag = 0
  }
  if wetter.verdunstung<0 {
    wetter.verdunstung = 0
  }
  if wetter.niederschlag<0 {
    wetter.niederschlag = 0
  }

  return wetter
}

type ZusR struct {
  tag int
	hh int
}
var gaben = list.New()

func gabeZumHeap(tag, hh int) {
  gabe := new(list.Element)
  gabe.Value = ZusR{tag, hh}
  gaben.PushBack(gabe)
}

func gabenAm(tag int) (res int) {
  for g:=gaben.Front(); g!=nil; g=g.Next() {
	  if gabe, ok := g.Value.(ZusR); ok && gabe.tag == tag {
      res = res + gabe.hh
    }
  }
  return
}

func loeschGabenImHeap() {
  gaben.Init()
}

func bfxTage(xTage, aktTag, faTag int, ibodenschlag Bodenschlag,
  schlag Schlagdaten, wd WiDatTyp, qu QuType, irrigationMode int,
  bfprognose bool) (quakt float64, bodenschlag Bodenschlag) {
    
  bodenschlag = ibodenschlag
  var nt = []int{1,3,6,10,16}

  copy(bodenschlag.wmm[:], schlag.bfAkt[:])
  var qusumme float64 = 0
  quakt = 0

  bedgrakt, _ := fvonx(wd.ikurv[wd.bdi:wd.bdi+wd.bdn],
    wd.rkurv[wd.bdi:wd.bdi+wd.bdn],faTag-1,0.01)

  var tage int

  if xTage>0 { // Zyklus ueber die xTage
    // gotoxy(1,Bildschirmzeilen-2); write('zu rechnende Tage');

    tage = xTage
    for xTage>0 {
      aktTag++
      faTag++
      // gotoxy(19,Bildschirmzeilen-2); write(xTage:3);
      if bedgrakt<0.01 {
        qu.qusoll = 0
      } else {
        qu.qusoll, _ = fvonx(wd.ikurv[wd.qui:wd.qui+wd.qun],
          wd.rkurv[wd.qui:wd.qui+wd.qun],faTag,0.01)
      }

      //bedgrvor := bedgrakt;
      bedgrakt, _ = fvonx(wd.ikurv[wd.bdi:wd.bdi+wd.bdn],
        wd.rkurv[wd.bdi:wd.bdi+wd.bdn],faTag,0.01)

      var nbs, nbsd int
      if bedgrakt<=0.001 {
        nbs = 0
        nbsd = 0
      } else {
        temp, _ := fvonx(wd.ikurv[wd.ni:wd.ni+wd.nn],
          wd.rkurv[wd.ni:wd.ni+wd.nn],faTag,1)
        nbsd = int(round(temp+0.1))
        nbs = 1
        for nbsd>nt[nbs] {
          nbs++
        }
      }

	    wetterBf := holeWetter(aktTag, schlag.nrNieMst)

	    hhzus := gabenAm(aktTag)

      t, _ := fvonx(wd.ikurv[wd.ti:wd.ti+wd.tn],
        wd.rkurv[wd.ti:wd.ti+wd.tn],faTag,0.01)

      var effnied, pt, effzusbed, effzusunbed float64
      effnied, pt, effzusbed, effzusunbed = inzp(wetterBf.niederschlag,
        wetterBf.verdunstung, float64(hhzus), t, irrigationMode, pt)

      // Wassergehalt der oberen Schicht wird um den Niederschlag
      // und den Zusatzregen zu Beginn des Tages erhoeht
      bodenschlag.wmm[1] = bodenschlag.wmm[1]+(effnied+effzusbed)*bedgrakt+
        (wetterBf.niederschlag+effzusunbed)*(1.0-bedgrakt)

      pt = pt*t

      if bedgrakt<0.001 {
        pt = 0
      }

      bodenschlag = bf1Tag(bodenschlag, wetterBf, bedgrakt, pt, nbsd, bfprognose)

      var quatpt, at float64 = 0, 0
      if bedgrakt<0.001 {
        quatpt = 0
      } else {
        if pt>0.01 {
          quatpt = at/pt
        } else {
          quatpt = 1
        }
      }
      if quatpt<qu.qusoll {
        quakt = quakt+qu.qusoll-quatpt
      }
      qusumme = qusumme+qu.qusoll
      xTage--
    }

    ftage := float64(tage)
    quakt = (qusumme-quakt)/ftage
    qu.qusoll = qusumme/ftage
  }

  return quakt, bodenschlag
}

// PROGRAMM ZUR BERECHNUNG DER BODENFEUCHTE NACH
// DEM MEHRSCHICHTMODELL FUER 5 SCHICHTEN AN EINEM TAG
func bf1Tag(bodenschlag Bodenschlag, wetterBf WetterTyp, bedgrakt, pt float64,
  nbsd int, bfprognose bool) Bodenschlag {
  const sreb = 3  // sreb :=(pwp-pw7)/(pwp-pw9) := 3.0
  const bedeckt = true
  const nus = 3
  const nud = 6

  // VORBEREITUNGSRECHNUNGEN
  nb  := 0
  nbs := 1

  // Berechnung der aktuellen Reduktionsparameter
  var r float64 = 0.0
  var reu, reb Bodenvektor
  for j:=1; j<=tiefenstufen; j++ {
    if bodenschlag.wmm[j]>bodenschlag.pw9[j] {
      r := (bodenschlag.wmm[j]-bodenschlag.pw9[j])*bodenschlag.fkm9[j]
      if r>1 {
        r = 1.0
      }
    } else {
      r = 0
    }
    reu[j] = r*r
    reb[j] = 1.0
    if bedgrakt>0.01 && nb<nbsd {
      nbs = j
      nb = nb + j
      if bodenschlag.wmm[j]<bodenschlag.fk[j] {
        rk := bodenschlag.kne[j]
        if pt>3.0 {
          rk = bodenschlag.kpo[j]
        }
        xsi := bodenschlag.xsm[j]+(pt-3.0)*rk
        if bodenschlag.wmm[j]<xsi {
          if bodenschlag.wmm[j]>=bodenschlag.pwp[j] {
            reb[j] = (bodenschlag.wmm[j]-bodenschlag.pw7[j])/(xsi-bodenschlag.pw7[j])
          } else {
            if bodenschlag.wmm[j]>bodenschlag.pw9[j] {
              reb[j] = (bodenschlag.wmm[j]-bodenschlag.pw9[j])*sreb/(xsi-bodenschlag.pw7[j])
            } else {
              reb[j] = 0.0
            }
          }
        }
      }
    }
  }

  // BERECHNUNG DER ENTNAHME BODENS
  var at float64 = 0
  var entnahme Bodenvektor

  if bedgrakt<0.99 { // unbedeckter Boden
    gi := giKoitzsch(nus, nud, !bedeckt, reu, bodenschlag.ivd)
    for j:=1; j<=nus; j++ {
      entnahme[j] =  wetterBf.verdunstung*gi[j]
    }
  }

  if bedgrakt>0.01 { // bedeckter Boden
    if bfprognose {
      if nbsd>6 {
        nbsd = 6
        nbs = 3
      }
      gi := giKoitzsch(nbs,nbsd,bedeckt,reb,bodenschlag.ivd)
      for j:=1; j<=nbs; j++ {
        at = at + pt * gi[j];
        entnahme[j] = entnahme[j]*(1.0-bedgrakt)+pt*gi[j]*bedgrakt
      }
    }
  }

  // BERECHNUNG DER BODENFEUCHTE
  var versickAusJ float64 = 0
  aktTiefe := 0
  var jsmb int;
  for j:=1; j<=tiefenstufen; j++ {
    aktTiefe = aktTiefe+j
    jsmb = 0
    faktTiefe, fj := float64(aktTiefe), float64(j)
    if faktTiefe-0.5*fj<=float64(bodenschlag.grundwastand) {
      wAnfang := bodenschlag.wmm[j] - bodenschlag.vsgr[j]
      wEnde := glugla(wAnfang, versickAusJ-entnahme[j],bodenschlag.lambda[j])
      versickAusJ = wAnfang+versickAusJ-entnahme[j]-wEnde
      bodenschlag.wmm[j] = bodenschlag.vsgr[j]+wEnde
      if bodenschlag.wmm[j]<bodenschlag.pw9[j] {
        bodenschlag.wmm[j] = bodenschlag.pw9[j]
      }
      if bodenschlag.wmm[j]>bodenschlag.spm[j] {
        jsmb = j
      }
    } else {
      bodenschlag.wmm[j] = bodenschlag.spm[j]
      versickAusJ = 0
    }
  }

  // VERTIKALER AUSGLEICH BEI UEBERFEUCHTUNG
  if jsmb!=0 {
    for i:=1; i<=jsmb; i++ {
      j := jsmb-i+1
      wus:=bodenschlag.wmm[j]-bodenschlag.spm[j]
      if wus<0 {
        wus = 0
      }
      if j>1 {
        bodenschlag.wmm[j-1] = bodenschlag.wmm[j-1]+wus
      }
      bodenschlag.wmm[j] = bodenschlag.wmm[j]-wus
    }
  }

  return bodenschlag
}

func glugla(wia, nist, la float64) (res float64) {
  if wia < 0 { // vorher unter Sickergrenze
    res = nist + wia
    if nist >= 0 {
      if nist+wia > 0 {
        b2 := -math.Exp(-2*math.Sqrt(la*nist)*(1+wia/nist))
        res = math.Sqrt(nist/la)*(1+b2)/(1-b2)
      }
    }
  } else { // vorher ueber Sickergrenze
    if nist == 0 {
      res = wia/(1+la*wia)
    } else {
      if nist < 0 { // Entzug
        b2 := math.Sqrt(-nist/la)
        b3 := math.Sqrt(-nist*la)
        b1 := math.Atan(wia/b2)/b3
        if b1<=1 {
          res = nist*(1-b1)
        } else {
          b1 = math.Sin(b3)/math.Cos(b3)
          res = b2*(wia-b2*b1)/(b2+wia*b1)
        }
      } else { // kein Entzug
        b1 := math.Sqrt(nist/la)
        b2 := (wia-b1)*math.Exp(-2*math.Sqrt(la*nist))/(wia+b1)
        res = b1*(1+b2)/(1-b2)
      }
    }
  }
  return res
}

// UNTERPROGRAMM ZUR BERECHNUNG DER ENTNAHMEMENGEN
// MODELL  /  PROGRAMM : KOITZSCH  / NEUMEYER
func giKoitzsch(ns, nsd int, bedeckt bool, reb Bodenvektor, ivd int) (gi Bodenvektor) {
  var gj Bodenvektor

  if ns>5 {
    ns = 5
  }

  var rmax float64 = 0
  nmd := 0

  for i:=1; i<=ns; i++ {
    for l:=1; l<=i; l++ {
      nmd++
      if nmd>nsd {
        goto leave
      } else {
        if bedeckt && nmd<nsd && nmd<2 {
        } else {
          var rij float64 = 0
          ff := f1Koitzsch(nmd,ivd,bedeckt)
          for j:=1; j<=i; j++ {
            gj[j] = reb[j]*ff[j]
            rij = rij+gj[j]
          }
          n := 0
          if (rmax-0.002)>rij {
            if nmd<5 {
            } else {
              if n==1 {
                goto leave
              } else {
                n = 1
              }
            }
          } else {
            rmax = rij
            for m:=1; m<=i; m++ {
              gi[m] = gj[m]
            }
             //km = nmd
             n = 0
          }
        }
      }
    }
  }
leave:
  return
}

// BERECHNUNG DER ENTNAHMEVERTEILUNG
// MODELL   : KOITZSCH
// PROGRAMM : NEUMEYER
func f1Koitzsch(n, ivd int, bedeckt bool) (ff Bodenvektor) {
  var fn1 = []float64 {
    3.00000 ,1.25000, 0.777778, 0.56250, 0.44000,
    0.36111 ,0.30612 ,0.26563 ,0.23457 ,0.21000,
    0.19008, 0.17361 ,0.15976 ,0.14796, 0.13777}
  var fn2 = []float64 {
    2.00000 ,0.50000 ,0.222222,0.12500 ,0.08000,
    0.055555,0.040815,0.031251,0.024691,0.02000,
    0.016528,0.013888,0.0118335,0.010204,0.0088875}
  var fnub = []float64 {0.625 ,0.3, 0.075}
  var is = []int {1,3,6,10,15}

  if ivd>1 {
    ivd = 20
  }

  if bedeckt { // BEDECKT
    if n>15 {
      n = 15
    }
    fvi := fn1[n]
    fvd := fn2[n]
    kk := 0
    vorw := true
    for l:=1; l<=n; n++ {
      if vorw {
        kk++
        if kk>ivd {
          kk = ivd
          vorw = false
        } else {
          kk--
          if kk<1 {
            kk = 1
            vorw = true
          }
        }
        j:=1
        for kk>is[j] {
          j++
        }
        fl := float64(l)
        ff[j] = ff[j]+fvi-fvd*fl
      }
    }
  } else { // UNBEDECKT
    copy(ff[:], fnub[:])
  }
  return ff
}

func inzp(ntab, vtab, zus, t float64, irrigationMode int, pe float64) (nef, rpe, gef, gefu float64) {
  const null = 0
  const null5 = 0.5
  const null2 = 0.2

  if t<1 {
    t = 1
  }

  // Berechnung fuer natuerlichen Regen
  tin := null5+(vtab-1)*t*null2;
  var inz float64
  if ntab>null {
    inz = tin
    pe = vtab-inz*null5
    if pe<null {
      pe = null
    }
  } else {
     pe = vtab
     inz = null
  }

  // Berechnung fuer Zusatzregen/Spruehverluste
  var iz, s float64
  if zus>1 {
    if irrigationMode == 0 {
      iz = 0.6*tin*(1.0+zus*0.05)
      s = (1+(vtab-2.5)*null2)*null2*zus
    } else {
      iz = 0
      s = 0
    }

  // KORREKTUR DER PE
    if ntab>null {
      pe = vtab-(iz+inz)*null5
    } else {
      pe = vtab-iz*0.75
    }
  } else {
   s = null
   iz = null
  }

  if pe<null {
    pe = null
  }
  
  rpe = pe
  nef = ntab-inz
  gef = zus-iz-s
  gefu = zus-s

  return
}

func hohe(ersterFaTag int, st TechnTyp, wd WiDatTyp, ibodenschlag Bodenschlag,
  schlag Schlagdaten, irrigationMode int) (qu QuType, bs Bodenschlag) {

  var richtung, bertage int = 0, 0
  var bodenschlag, bodenschlag2 Bodenschlag

  // ANFANGSWERTE
  bfPrognose := true
  qu.qu00 = 0
  qu.quhh = 0

  //begin
  loeschGabenImHeap()
  bertage = st.turnus;
  if bertage < 4 {
    bertage = 4
  }
        
  // Vorgabe
  t, _ := fvonx(wd.ikurv[wd.efi:wd.efi+wd.efn],
                              wd.rkurv[wd.efi:wd.efi+wd.efn],
                              ersterFaTag, 0.01)
  qu.quef = qu.qusoll - t

  st.hhOpt = 0
  if qu.qusoll < 0.1 {
	  st.empfText = "Entw/Zeitr"
  } else {

    qu.qu00, bodenschlag = bfxTage(bertage, glob.vari.rechentag-1,
      ersterFaTag, ibodenschlag, schlag, wd, qu, irrigationMode,
      bfPrognose) // q-ist bei hhOpt=0
    if qu.qu00 < qu.qusoll {
      //begin
      if qu.quef < qu.qu00 {
			  st.empfText = "in ca 4 Tg"
      } else {
        st.hhOpt = st.hhNorm
        richtung = 0
        for ;; {
				  loeschGabenImHeap()
          gabeZumHeap(glob.vari.rechentag+1,st.hhOpt)
          qu.quhh, bodenschlag2 = bfxTage(bertage, glob.vari.rechentag-1,
            ersterFaTag, bodenschlag, schlag, wd, qu, irrigationMode,
         	 	bfPrognose)
          //if qu.quhh > (qu.qusoll+qu.quef)/2
          if qu.quhh > (1.00+qu.qusoll)/2 { // Evtl. Reduzierung
            if richtung > 0 || st.hhOpt == st.hhMin {
              goto ende
            } else { // Reduzierung von hhOpt
              st.hhOpt = st.hhOpt - st.hhStufung
              if st.hhOpt < st.hhMin {
                st.hhOpt = st.hhMin
              }
              if richtung < 0 {
                goto ende
              }
              richtung = -1
            }
          } else { // evtl. Erhoehung
            if qu.quhh > qu.quef || richtung < 0 || st.hhOpt == st.hhMax {
              goto ende
            } else { // Erhoehung von hhOpt
              st.hhOpt = st.hhOpt + st.hhStufung
              if st.hhOpt > st.hhMax {
                st.hhOpt = st.hhMax
              }
              if qu.quhh < qu.quef {
                richtung = 1
              } else {
                richtung++
              }
              if richtung > 3 {
                goto ende
              }
            }
          }
        }
      }
    } else {
      st.empfText = "BF opt."
    }
  }
ende:
  if st.hhOpt > 0 {
    if st.hhOpt == st.hhMax && qu.quhh < qu.quef {
	    st.empfText = "S.K. erh."
    } else {
      st.empfText = "Gabe opt."
    }
  }
  //bfprognose = false

  bs = bodenschlag2
  return
}

func faDatenOk(faNrAfVz string, wd WiDatTyp) (res bool) {
	//var keyFaN string
	var satznummer int

	if faNrAfVz != wd.faNrAfVz {
		ok := true //FindKey(faWiIndex,satznummer,faNrAfVz);
		if ok {
			if satznummer != wd.satznummer {
				//GetRec(FaWiDat,Satznummer,WiDat.FaSymbol);
				wd.satznummer = satznummer
			}
			wd.faNrAfVz = faNrAfVz
			res = ok
		} else  {
			res = false
		}
	}	else {
	 	res = true
	}

	return
}

func liesZusatzregen(isnru int) int {
 	return 10;
}

func liesAlteZus(minDate, isnru int, schlag Schlagdaten, em EMType1) (ok bool) {
	var tag int
	//ods('le',252,'end',null,null,EM);
	em.code =700
	ok = true
	for ; ok; {
		//ods('le',252,'<ob=',iSnru,EM.Code,EM);
   	if ok && em.kenn=="M" {
      tag = em.qunote // { hier steht der Tag, fr den die }
			if tag > minDate { // { Gabe erstmals verrrechnet wurde }
        if em.beafl > (0.5 * schlag.beFlaeche) && em.menge > 0 {
        	gabeZumHeap(tag, int(round(em.menge)))
        }
      } else {
      	ok = false
      }
    }
  }
  return
}

func berechneSchlaghh(iEmpfTab TabTyp, ibfAkt Bodenvektor,
	schlag Schlagdaten, wd WiDatTyp,
	irrigationMode int, btrec BtType, bm BMTyp1, em EMType1,
	qu QuType, schlTechn TechnTyp) (empfTab TabTyp) {
/*
func bodenSchlag_mm(ibfAkt Bodenvektor, bodenschlag Bodenschlag,
	schlag Schlagdaten, wd WiDatTyp, irrigationMode int) (bfAkt Bodenvektor) {
	bfAkt = ibfAkt

	var bodenschlag2 Bodenschlag

	for i:=1; i<=5; i++ {
		bfAkt[i] = bodenschlag.wmm[i]
	}
*/

	var ok bool
	var bodenschlag Bodenschlag

	empfTab = iEmpfTab

	//var tc string
  var faTag, adVorz int

	const bfCode = 8003
  //var laelz string = "Ihre Dateneingabe erfordert laengere Laufzeit !"

	ausgabeUndSpeichern := func(datum, bfCode int) {
		//ods('le',254,Pruef[1],iSnru,BFCode,BM); { ob= }
	  /*
		if ok
			 then if BfCode = 8903
							 then if BM.Tag=Datum
											 then iPruef := 2 { =rc }
											 else iPruef := 3 { nob }
							 else iPruef := 2 { =rc }
			 else iPruef := 3; { nob }
		if iPruef = 3 { nob }
			 then ods('le',254,Pruef[4],null,null,BM); { rc= }
		BM.Snru:=iSnru;
		BM.Code:=BFCode;
		BM.Tag:=Datum;
		BM.Wert[1]:=NWKProzent(1,2);
		BM.Wert[2]:=NWKProzent(3,3);
		BM.Wert[3]:=NWKProzent(1,3);
		ods('sc',254,pruef[iPruef],iSnru,BFCode,BM);
		BerInterrupt := not Ausgabe(EmpfTab,concat(RealzuStr(BM.Wert[1],4,0),
																RealzuStr(BM.Wert[2],4,0),' '),0,0);
		*/
	}

	//>>> here schlag data have actually been read from ods
	//now will be supplied to function
	//{ Daten des Schlages lesen }
	//ods('le',6,'ob=',iSnru,null,Schlag);

  //>>> check the schlag data
	//{ Prufen der Schlagdaten }
	feStr, schlagOk = sg_ok(schlag, btrec)

	aktFaNrAfVz = fmt.Sprintf("%s%s%s", schlag.faNummer, schlag.faAf, schlag.faVz)
	schlag.faNrAfVz = aktFaNrAfVz
	if schlag.faNrAfVz == "" {
		schlag.faNrAfVz = "      "
	}
	if aktFaNrAfVz == "" || schlag.aktDC > 98 {
		aktFaNrAfVz = "000000"
	}
	if !faDatenOk(aktFaNrAfVz, wd) {
   	feStr = feStr + " 14"
   	schlagOk = false
  }
	if schlagOk { //{ Bodendaten des Schlages sind ok und FA-Daten sind da }
		bodenschlag = fkpwla(100, schlag)
		if schlag.aktDatum < -365 { // { noch keine Feuchte drin }
		  if !reBF(iSnru, 0, bm, schlag, bodenschlag) {
      	if !reBF(0, 0, bm, schlag, bodenschlag) { // { Feuchte mit Shift F10 ? }
					//esFehlt(concat(Snru,' Startfeuchte'),0);
					feStr = feStr + " 10"
					schlagOk = false
				}
			}
      if schlagOk {
      	//ods('sc',7,'=rc',iSnru,null,Schlag)
      }
		} else {
			schlag.aktDatum = abs(schlag.aktDatum)
		}

		if schlagOk { //{ Startbodenfeuchte ist vorhanden }
			//begin
      altAktDatum = schlag.aktDatum
      if reBF(iSnru,0, bm, schlag, bodenschlag) {
      	//ods('sc',7,'=rc',iSnru,null,Schlag) { neuer Startfeuchte liegt vor }
      }

	  	// { Zusatzregengaben  }
      minZus = liesZusatzregen(iSnru)
      minDatum = minI(schlag.aktDatum, minWetter-1)
      minDatum = minI(minDatum, minZus-1)
      if minDatum < altAktDatum { //{ Es wurde  zurckdatiert }
			  if altAktDatum > schlag.aktDatum { //{ wegen neuer Feuchte }
					minDatum = schlag.aktDatum
        } else {
        	reBF(iSnru, minDatum, bm, schlag, bodenschlag)
				}
			}
      //else { es wurde micht zurckdatiert }

      if altAktDatum > schlag.aktDatum || minZus < glob.vari.rechentag {
      	ok = liesAlteZus(schlag.aktDatum,iSnru, schlag, em)
      }

      //{ Berechnung der Bodenfeuchte }
      //speicherStatusZeile;
			abDatum, _ = dateStr(schlag.aktDatum+1,0,4)
			//insert('.',abDatum,3);
			//insert('.',abDatum,6);
			//schreibStatusZeile(concat(' Berechne Schlag ',Snru,
			//													' - Feuchte  ab ',abDatum));
			//with  Schlag do
			//begin
      //{ if SitzJahr < Erntejahr then ADVorz := -1
      //                             else }
      adVorz = 1
			if aktFaNrAfVz == "000000" {
				schlag.aktDCTag = 0
				abDatum = ""
			} else {
				//dcWeiter(schlag.aktDC, schlag.aktDCTag, glob.vari.rechentag+7)
				abDatum, _ = dateStr(schlag.aktDCTag, 0, 4)
				//insert('.',abDatum,3);
				if abDatum[4-1] == '0' {
					//insert('.',abDatum,6);
					//delete(abDatum,4,1);
				}
			}

			t, _ := fvonx(wd.ikurv[wd.dci:wd.dci+wd.dcn],
				wd.rkurv[wd.dci:wd.dci+wd.dcn], schlag.aktDC, 1)
			faTag = int(round(t+0.1))
			ersterFaTag := faTag + schlag.aktDatum - schlag.aktDCTag
			bfPrognose := false
			//{ if (glob.vari.VorRetag) > aktDatum then ArchivBf(iSnru,aktDatum);}

			if schlag.aktDatum < glob.vari.vorReTag {
				for bof_bis := glob.vari.vorReTag - 1 ; bof_bis == glob.vari.vorReTag; { //{ Zykluss ber die Archivierung Beginn }
					bof_arch := (((schlag.aktDatum + 1) % 7) + 1) * 7
					fkpwla(bof_arch-3, schlag)
					bof_bis = bof_arch;
					if bof_bis > glob.vari.vorReTag {
						bof_bis = glob.vari.vorReTag
					}

					qu.quhh, bodenschlag = bfxTage(bof_bis - schlag.aktDatum,
						schlag.aktDatum, ersterFaTag, bodenschlag, schlag, wd, qu,
						irrigationMode, bfPrognose)

					for i:=1; i<=5; i++ {
						schlag.bfAkt[i] = bodenschlag.wmm[i]
					}
					ersterFaTag = ersterFaTag + bof_bis - schlag.aktDatum
					schlag.aktDatum = bof_bis
					if bof_bis == bof_arch {
						//bFzuArchiv(Bof_Arch)
					}
				}
			} else {
				for i:=1; i<=5; i++ {
					bodenschlag.wmm[i] = schlag.bfAkt[i]
				}
			}

      //{ fuer Prognose Bf }
      fkpwla(schlag.aktDatum, schlag);

      //{ Ausgabe der Fruchtartnummer }
			//berInterrupt = not Ausgabe(EmpfTab,
      //     concat(copy(Schlag.faNrAfVz,1,4),'/',copy(Schlag.faNrAfVz,5,1),'/',
      //            copy(Schlag.faNrAfVz,6,1),' ',Widat.faSymbol),0,0);

      //{ lesen der Fruchtartdaten  }
      //{ und Ausgabe des DC }
 	    if aktFaNrAfVz != "000000" {
     		//berInterrupt := not Ausgabe(EmpfTab,
        //                         concat(IntZuStr(aktDC,2),'/',abDatum),0,0)
      } else {
      	//berInterrupt := not Ausgabe(EmpfTab,'        ',0,0);
      }

      //{ Ausgabe der Bodenfeuchte  }
	    ausgabeUndSpeichern(adVorz*schlag.aktDatum,8903)

      //{ BerInterrupt := not Ausgabe(EmpfTab,'|',0,0);}
      //{ Prognose Bf }
      //schreibStatusZeile(concat(' Berechne Schlag ', Snru,' - Feuchteprognose'));
      bfPrognose = true

      qu.quhh, bodenschlag = bfxTage(7, schlag.aktDatum,
						ersterFaTag, bodenschlag, schlag, wd, qu, irrigationMode, bfPrognose)

			//{ Ausgabe der prognostischen Bodenfeuchte  }
      ausgabeUndSpeichern(adVorz*(schlag.aktDatum+7),8904)
      //berInterrupt := not Ausgabe(EmpfTab,IntZuStr(SchSrec.NrGabe+1,2),0,0);

      //{ Empfehlungsteil }
      schSrec.empf_mm = 0
      //schSrec.empf_Tag = concat(copy(Glob.vari.Datum,1,2),
      //                               copy(Glob.vari.Datum,4,2));  { ttmm }
      schSrec.empf_text = "    -     " // { xxxxxxxxxx }
      //  {  if BEREST_
      //        then  }
			//begin
			if aktFaNrAfVz != "000000" { //{ Berechungsflaeche pruefen }
				if schlag.beFlaeche > 0 && schlag.beFlaeche <= schlag.faFlaeche {
					//schreibStatusZeile(concat(' Berechne Schlag ',
					//																	 Snru,' - Empfehlung'));
					schlagOk = technologie(iSnru) || technologie(0)
					if !schlagOk {
						//RestoreStatusZeile;
						//EsFehlt(concat(snru,' Technik'),0);
						feStr = feStr + " 16"
						//SpeicherStatusZeile;
						//SchreibStatusZeile(concat(' Berechne Schlag ',
						//													 Snru,' - Empfehlung'));
					} else {
						if qu.qu00 < qu.qusoll {
							if schlTechn.hhMin == 0 {
								schlTechn.empfText = "Bf-hoch"
							} else if schlTechn.hhMax >= schlTechn.hhMin {
								hohe(ersterFaTag, schlTechn, wd, bodenschlag, schlag,
									irrigationMode)
							} else {
								schlTechn.empfText = "Tech.min"
							}
						} else if qu.qusoll < 0.1 {
							schlTechn.empfText = "Entw/Zeitr"
						} else {
							schlTechn.empfText = "Bf-opt"
						}
						//SpeichereEmpf(iSnru);
						schSrec.empf_text = schlTechn.empfText
						schSrec.empf_Tag, ok = dateStr(em.beginn,0,4)
						if !ok {
						 schSrec.empf_Tag = "...."
						}
						//BerInterrupt := not Ausgabe(EmpfTab,
						//		concat(IntzuStr(SchlTechn.hhopt,3),
						//				IntZuStr(SchlTechn.hhmax,3),' ',
						//										 SchlTechn.EmpfText),0,0);
					}
				} else if schlag.beFlaeche < 0.1 {
					//berInterrupt := not Ausgabe(EmpfTab,
					//																	 ' kein Ber.Schlag',0,0)
				} else {
					//berInterrupt := not Ausgabe(EmpfTab,
					//																	 ' Ber.-Fl. > FA.-Fl.',0,0)
				}
			} else if schlag.faNrAfVz > "000000" {
				//berInterrupt := not Ausgabe(EmpfTab,
				//														' keine Aussaat/Pflanzung',0,0);
			}

			schlag.aktDatum = adVorz*schlag.aktDatum

    	//{ Daten des Schlages schreiben }
			//ods('sc',7,'=rc',iSnru,null,Schlag);
    	//RestoreStatusZeile;
	 	} //{ Schlagok nach Startfeuchte }
  } //{ Bodendaten des Schlages sind ok und FA-Daten sind da }
	if feStr != "" {
  	//BerInterrupt := not Ausgabe(EmpfTab,concat(' Fehler',FeStr),0,0);
  }
	if berInterrupt {
		iSnru = 0
	}

  return
}

func technologie(schlagNr int) bool {
/*
var vb60, i : integer;

const obtech : array[1..2] of Integer = (9101,9102);

begin
{     BERECHNUNG VON SchlTech.hhmax  }
SchlTechn.hhmax := round(BodenSchlag.VSGR[1]+BodenSchlag.VSGR[2] -
                  (Schlag.BfAkt[1]+Schlag.BfAkt[2]));
vb60 := round(BodenSchlag.VSGR[3]-Schlag.BfAkt[3]);
if (SchlTechn.hhmax * vb60) > 0
   then   SchlTechn.hhmax := SchlTechn.hhmax + vb60;

for i:= 1 to 5 do
    begin
	 holewetter(glob.vari.VorReTag+i,1,wetterbf);
	 SchlTechn.hhmax := SchlTechn.hhmax +
                    round(Wetterbf.Verdunstung - Wetterbf.Niederschlag);
    end;
if SchlTechn.hhmax <  0 then  SchlTechn.hhmax  := 0;
SchlTechn.turnus := 1;
SchlTechn.hht := 1;
if SchlTechn.hhmax < 15
   then  SchlTechn.hhmin := 0
   else begin
	ods('le',254,'ob=',SchlagNr,obtech[1],BM);
   if ok
      then begin
      if BM.Wert[1] > 0
         then begin
         SchlTechn.turnus  := round(BM.Wert[1]);
			SchlTechn.hhNorm  := round(BM.Wert[2]);
			case Schlag.hangN of
			1,2  : SchlTechn.hhn     := 50;
			3	  : SchlTechn.hhn     := 40;
			4,5  : SchlTechn.hhn     := 30;
			6    : SchlTechn.hhn     := 20;
			else;
			end; { case }
         end
         else SchlTechn.turnus := 0;
      end
      else SchlTechn.turnus := 0;
	ods('le',254,'ob=',SchlagNr,obtech[2],BM);
   if ok
      then begin
      if BM.Wert[1] > 0
         then begin
         SchlTechn.hht     := round(BM.Wert[1]);
         SchlTechn.hhmin   := round(BM.Wert[2]);
			SchlTechn.hhStufung := round(BM.Wert[3]);
         end
         else SchlTechn.hht := 0;
      end
      else SchlTechn.hht := 0;
   if (SchlTechn.turnus > 0) and (SchlTechn.hht > 0)
      then begin
      if SchlTechn.hhn  < SchlTechn.hhMax  then  SchlTechn.hhMax := SchlTechn.hhn;
      if SchlTechn.hht  < SchlTechn.hhMax  then  SchlTechn.hhMax := SchlTechn.hht;
		while SchlTechn.hhMax < SchlTechn.hhNorm   do
				SchlTechn.hhNorm  := SchlTechn.hhNorm - SchlTechn.hhStufung;
      end;
   end;
	SchlTechn.hhopt := 0;
Technologie := (SchlTechn.turnus > 0 ) and (SchlTechn.hht > 0);
*/
	return true
}

func addFehler(nr int) {
  schlFehler.anz++
  schlFehler.num[schlFehler.anz] = nr
}

func archivDatum(bm BMTyp1) (bfStart int, res bool) {
	//code := 8920
	ok := true //ods('le',254,Pruef[1],iSnru,code,BM); { ob= }
	if ok {
  	//begin
   	bfStart = bm.tag;
   	for i:=1; i<=3; i++ {
      archTC[i].tag = int(round(bm.wert[i]))
      if archTC[i].tag > 365 {
				archTC[i].tag = 0
			}
      archTC[i].code = (i-1)*2+1 // 1,3,5
    }
   	for i:=1; i<=2; i++ {
    	for j:=i+1; j<=3; j++ {
     		if archTC[i].tag > archTC[j].tag {
        	minTC = archTC[j]
        	archTC[j] = archTC[i]
        	archTC[i] = minTC
        }
      }
    }
   	res = true
  } else {
  	res = false
  }
	return
}

func reBF(iReadSnru, minDatum int, bm BMTyp1, schlag Schlagdaten,
	bodenschlag Bodenschlag) (rreBF bool) {
	var id, code, codeEinheit, messTag, i, j, k int
	var messdicke [7]int
	var messwert [6]float64
	var bf_ist bool
	var rechendicke []int = []int{-9999,1,3,6,10,15,16}

	startBF := func(sollneu bool) (id int, rstartBF bool) {
		code = 8910
		if sollneu {
			schlag.grws = 20
			//ods('le',254,Pruef[1],iReadSnru,code,BM); { ob= }
			ok := true //ok from ods(...)
			if ok && iReadSnru > 0 {
				if bm.wert[1] >= 0 {
					schlag.grws = int(round(bm.wert[1]))
				}
			} else if iReadSnru > 0 {
				//ods('le',254,Pruef[4],null,null,BM);  { rc= }
				bm.snru = iSnru
				bm.code = code
				//ods('sc',254,Pruef[3],iSnru,code,BM); { nob }
			} else {
				bm.wert[2] = 0
			}
		}
		if sollneu && bm.wert[2] > 0 { //StartBof ist verarbeitet
			rstartBF = false
		} else { //neue Startfeuchte
      id = 1
			ok := true
			for ; ok; {
				code++
				//ods('le',254,Pruef[1],iReadSnru,code,BM); { ob= }
				if ok && bm.wert[1] > 0 { //Dicke > 0
					//begin
					if iReadSnru == 0 {
						bm.snru = iSnru
						//ods('sc',254,Pruef[3],iSnru,code,BM); { nob }
					}
					messTag = bm.tag
				 	messdicke[id] = int(round(bm.wert[1]))
				 	codeEinheit = int(round(bm.wert[2]))
					if codeEinheit == 4 {
						messwert[id] = bm.wert[3] / bm.wert[1]
					} else {
						messwert[id] = bm.wert[3]
					}
					id++
				}
			}
			id--
			if messdicke[id] == 15 {
				if messTag < 90 || messTag > glob.vari.vorReTag {
					rstartBF = false
				} else { //ArchivBf logisch löschen
				 	code = 8920
					ok := true//ods('le',254,Pruef[1],iSnru,code,BM); { ob= }
					if ok {
						iPruef = 2 // { =rc }
					} else {
						//ods('le',254,Pruef[4],null,code,BM); { rc= }
						bm.snru = iSnru
						bm.code = code
						iPruef = 3 //{ nob }
					}
					bm.tag = messTag
					for i:=1; i<=3; i++ {
						bm.wert[i] = 0.0
					}
					//ods('sc',254,Pruef[iPruef],iSnru,code,BM);
					rstartBF = true
				}
			}	else {
				rstartBF = false
			}
		}
		return
	}

  var liesArchBF = func(mindatum int) (id int, res bool) {
		//bfStart
		_, ad := archivDatum(bm)
		if ad {
			code := 0
			for i:=3; i > 0 && code == 0; {
				if archTC[i].tag > 0 && archTC[i].tag < minDatum {
					code = 8920 + archTC[i].code
				}	else {
					i--
				}
			}
			if code > 0 { //ArchivBF lesen
				//ods('le',254,Pruef[1],iSnru,code,BM); { ob= }
				dicke := 0;
				for i:=1; i<=3; i++ {
					dicke = dicke + i
					messdicke[i] = dicke
					messwert[i] = round(bm.wert[i])
				}
				code++
				//ods('le',254,Pruef[1],iSnru,code,BM); { ob= }
				for i:=4; i<=5; i++ {
					dicke = dicke + i;
					messdicke[i] = dicke
					messwert[i] = round(bm.wert[i-3])
				}
				codeEinheit = 2
				messTag = bm.tag
				id = 5
				res = true
			}
		}
		return
	}

	if minDatum == 0 {
   	id, bf_ist = startBF(true) // es muss neue StartBF sein
  } else {
		//begin
    id, bf_ist = liesArchBF(minDatum);
   	if !bf_ist {
   		id, bf_ist = startBF(false) //{ es soll alte StartBF sein }
   	}
  }
	if bf_ist {
  	if messdicke[id] == 15 {
			//begin
			messTag = abs(bm.tag)
			messdicke[id+1] = 16
			for k:=1; k<=5; k++ {
				schlag.bfAkt[k] = 0
			}
			i, j, k = 1, 1, 1
			for ; i<16; {
				//begin
				switch codeEinheit {
					case 1: schlag.bfAkt[k] = schlag.bfAkt[k] +
						(messwert[j] * 0.01 * bodenschlag.fk10[k])
					case 2: schlag.bfAkt[k] = schlag.bfAkt[k] +
						(messwert[j] * 0.01 * (bodenschlag.fk10[k] - bodenschlag.pw10[k]) +
						bodenschlag.pw10[k])
					case 3, 4: schlag.bfAkt[k] = schlag.bfAkt[k] + messwert[j]
				}
				if i == rechendicke[k] {
				 k++
				}
				i++
				if messdicke[j] < rechendicke[k] {
					j++
				}
			}
			schlag.aktDatum = messTag
			if iSnru > 0 {
				code = 8910
				//ods('le',254,Pruef[1],iSnru,code,BM); { ob= }
				bm.wert[2] = float64(glob.vari.rechentag)
				//ods('sc',254,Pruef[2],iSnru,code,BM); { =rc }
			}
		} else {
			id = 0
		}
	} else {
		id = 0
	}
	return id > 0
}

func sg_ok(schlag Schlagdaten, btrec BtType) (feStr string, res bool) {
	if schlag.faFlaeche > 0 {
  	res = true
  } else {
  	//EsFehlt(concat(Snru,' FA - Fl„che'),15); FeStr := Concat(FeStr,' 1');
 	}
 	if !(1 <= schlag.nrNieMst && schlag.nrNieMst <= btrec.anzMest) { //in [1..Btrec.AnzMest])
		//EsFehlt(concat(Snru,' Nie-Mst'),15); FeStr := Concat(FeStr,' 2');
	}
	/*
	if Schlag.faNummer = '    '
	then begin EsFehlt(concat(Snru,' Fruchtart'),15); FeStr := Concat(FeStr,' 3');
	*/
	if schlag.nrNieMst > 100 && schlag.nrNieMst < 10000 {
	} else {
		//EsFehlt(concat(Snru,' Standorttyp'),15); FeStr := Concat(FeStr,' 4');
	}
	if schlag.hangN < 1 || schlag.hangN > 6 { //in [1..6]
		//EsFehlt(concat(Snru,' Hangneigung'),15); FeStr := Concat(FeStr,' 5');
	}
	if schlag.dicke[3] == 0 {
		schlag.dicke[3] = 128
  }
	if !((schlag.dicke[1] == 3 ) &&
  	(2 <= schlag.dicke[2] && schlag.dicke[2] <= 9) && // in [2..9]) and
    ((2 <= schlag.dicke[3] && schlag.dicke[3] <= 9) || schlag.dicke[3] == 128)) { //(in [2..9,128]))
		//EsFehlt(concat(Snru,' Schichtdicken'),15); FeStr := Concat(FeStr,' 6');
	} else {
  	fkOk := true
   	for i:=1; i<4 && fkOk; { //while (i<4) and FkOk do
    	if schlag.dicke[i] != 128 {
      	fkOk =
       		(schlag.fk[i] > 0) &&
          (schlag.pwp[i] > 0) &&
          (schlag.pwp[i] < schlag.fk[i] * 0.8) &&
          (schlag.pwp[i] > schlag.fk[i] * 0.15)
      } else {
      	fkOk = (schlag.fk[i] <= 0) && (schlag.pwp[i] <= 0)
      }
    	i++
    }
   	if !fkOk {
      //EsFehlt(concat(Snru,' FK / PWP'),15); FeStr := Concat(FeStr,' 8');
    } else {
      dicke := schlag.dicke[1] + schlag.dicke[2]
      if schlag.dicke[3] != 128 {
      	dicke = dicke + schlag.dicke[3]
			}
	   	if dicke < 9 || dicke > 15 {
	   		//EsFehlt(concat(Snru,' Tiefe < 9'),15); FeStr := Concat(FeStr,' 7');
	   	}
		}
  }

  return feStr, feStr == ""
}

//{Kalendertag -> ddmmjj }
func dateStr(iDate, jahr, laenge int) (date string, ok bool) {
	var tabelle = []int{0,31,59,90,120,151,181,212,243,273,304,334,365}

  if iDate > 0 && iDate < 366 {
  	monat := 1
	 	for ; iDate > tabelle[monat]; monat++ {}
	 	date = fmt.Sprintf("%2d%2d%2d", iDate-tabelle[monat-1], monat, jahr)[0:laenge]
	 	//date = date[0:laenge]
	 	ok = true
  } else {
   ok = false
  }
  return
}
