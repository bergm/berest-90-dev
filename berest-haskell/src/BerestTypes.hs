-----------------------------------------------------------------------------
--
-- Module      :  Types
-- Copyright   :
-- License     :  AllRightsReserved
--
-- Maintainer  :
-- Stability   :
-- Portability :
--
-- |
--
-----------------------------------------------------------------------------

module BerestTypes where

import Data.Time.Calendar

-- Unterstruktur zu Fruchtartstammdaten „widat_kurve“
data Widat_kurve = Widat_kurve {
	i_tag :: Integer ,
	d_wert :: Double
} deriving (Show)

-- Fruchtartstammdaten „widat_fa“
data Widat_fa = Widat_fa {
   frucht_id :: Integer , -- Fruchtart-ID
   bedeckungsgrad :: [Widat_kurve] , -- Bedeckungsgrad Wertepaare
   n_bedeckungsgrad :: Integer , --	Anz. Bedeckungsgrad-Wertepaare
   entnahmetiefe  :: [Widat_kurve] , -- Entnahmetiefe Wertepaare
   n_entnahmetiefe :: Integer , -- Anz.  Entnahmetiefe Wertepaare
   transpiration :: [Widat_kurve] , -- Transpirations-Wertepaare
   n_transpiration	 :: Integer , -- Anz.  Transpirations-Wertepaare
   quotient :: [Widat_kurve] , -- Quotient-Wertepaare
   n_quotient :: Integer , -- Anz.  Quotient-Wertepaare
   effektivitaet :: [Widat_kurve] , -- Effektivität-Wertepaare
   n_effektivitaet :: Integer -- Anz.  Effektivität-Wertepaare
} deriving (Show)

data Schlagdaten = Schlagdaten {
    id :: Integer, -- Schlag-ID
    schlagnummer :: String, -- Schlagnummer
    faflaeche :: Double, -- Schlagfäche
    beflaeche :: Double, -- Beregnungsfläche
    betrid :: Integer, -- Betriebs-ID
    nrniemst :: Integer, -- Nr. der Niederschlagsmessstelle
    wetterstation :: Integer, -- Nr. der Wetterstation
    fruchtart_id :: Integer, -- Fruchtart-ID
    fanummer :: String, -- Fruchtart-Nr.
    faaf :: String, -- -
    favz :: String, -- -
    fanrafvz :: String, -- -
    fasymbol :: String, -- Fruchtart-Symbol
    faname :: String, -- Fruchtart-Name
    sttyp :: Integer, -- Standort-Typ
    hangn :: Integer, -- Hangneigung
    dicke :: [Integer], -- Dicke
    sd_fk :: [Double], -- FK
    sd_pwp :: [Double], -- PWP der Bodenschichten
    verdtiefe :: Integer, -- Schadverdichtungstiefe
    verdflae :: Double, -- Schadverdichtungsfläche
    aktdatum :: Day, -- Datum der letzen Berechnung
    bfakt :: [Double], -- Aktuelle Bodenfeuchte
    aktdctag :: Day, -- Tag zu aktdc
    aktdc :: Integer, -- DC
    fka :: [Double], -- -
    pwpa :: [Double], -- -
    grws :: Integer, -- Grundwasserstand
    dcstr :: String, -- DC zu Ausgabe
    dctagstr :: String, -- DC-Datum zur Ausgabe
    anzgaben :: Integer -- Anzahl der Regengaben
} deriving (Show)

data Bodenschlagtyp = Bodenschlagtyp {
    bst_fk :: [Double], -- Feldkapazität
    bst_pwp :: [Double], -- potentieller Welkepunkt
    nwk :: [Double], -- Nutzwasserkapazität
    fk10 :: [Double], -- Feldkapazität je dm
    pw10 :: [Double], -- potentielle Welkepunkt je dm
    pw7 :: [Double], -- modifizierter Arbeitswert
    pw9 :: [Double], -- modifizierter Arbeitswert
    fkm9 :: [Double], -- modifizierter Arbeitswert
    xsm :: [Double], -- modifizierter Arbeitswert
    spm :: [Double], -- modifizierter Arbeitswert
    vsgr :: [Double], -- Versickerung ab mm
    lambda :: [Double], -- Sickerparameter
    kpo :: [Double], -- modifizierter Arbeitswert
    kne :: [Double], -- modifizierter Arbeitswert
    wmm :: [Double], -- Bodenfeuchte operativ
    grundwastand :: Integer, -- Grundwasserstand
    ivd :: Integer -- Versickerungstiefe dm
} deriving (Show)

