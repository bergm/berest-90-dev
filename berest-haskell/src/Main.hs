-----------------------------------------------------------------------------
--
-- Module      :  Main
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

module Main where

import BerestTypes
import Data.Time.Calendar
import Data.Ix
import Data.List
import Data.Array
import Data.Maybe

main = putStrLn "Berest"

depth = 5



berechneSchlaghh :: Integer -> Day -> IO()
berechneSchlaghh al_schl_id adt_aktdatum = putStrLn ""


fkpwla :: (Schlagdaten, Bodenschlagtyp) -> Day -> (Schlagdaten, Bodenschlagtyp)
fkpwla (sd, bst) day = (sd, bst)
    where
        fkuTage2Werte = [(90,8), (135,135), (175,17) ,(210,177), (240,17), (282,13) , (306,95)]
        vsGrTage2Werte = [(80,11), (140,7), (100,10), (200,8)]
--        FK      LA       FK       LA
--       (MM/DM)  ( 1 DM)  (MM/DM)  ( 1 DM)
--        - 1 BIS 30 CM -----31 BIS 150 CM ---
        fkPwpL = [[(100,115), (90,115)],
                  [(40,68), (120,68)],
                  [(200,50), (180,50)],
                  [(240,33), (220,33)],
                  [(260,33), (250,33)],
                  [(300,25), (280,25)],
                  [(312,25), (364,25)],
                  [(327,17), (344,17)],
                  [(356,15), (373,15)],
                  [(1000,15),(1000,15)]]
        (year,_,_) = toGregorian day
        dayInYear = day `diffDays` (fromGregorian year 1 1)
        lambkor = (fvonx fkuTage2Werte dayInYear) * 0.01
        vspz = (fvonx (take 2 vsGrTage2Werte) dayInYear) * 0.1
        vsp2 = (fvonx (drop 2 vsGrTage2Werte) dayInYear) * 0.1


fvonx :: (Fractional a) => [(Integer, Integer)] -> Integer -> a
fvonx list key =
    case list of
        [(k,v)] -> fi v
        fp@(fk,fv):sp@(sk,sv):rest
            | key < fk -> fi fv
            | null rest && sk < key -> fi sv
            | inRange (fk, sk) key ->
                if fk == key
                then fi fv
                else interpolate (fip fp) (fip sp) (fi key)
            | otherwise -> fvonx (sp:rest) key
    where
        interpolate (pk,pv) (nk,nv) key = pv + ((key - pk) / (nk - pk) * (nv - pv))
        fip (k,v) = (fromInteger k, fromInteger v)
        fi = fromInteger

-- potential evapotranspiration
-------------------------------------

-- t = T = dayly average temperature
-- rf = RF = dayly average of relative humidity [%]
-- g = G = dayly sum of global radiation [MJ m-2]
-- gss = G** = dayly sum of global radiation at a cloudless sky [MJ m-2]
vaIwanow t g gs = 3.6e-5 * (t+25)^2 * (100-rf)
    where rf = (94-24*g) / gs

-- t = T = dayly average temperature
-- g = G = dayly sum of global radiation [MJ m-2]
vaTurc t g = 0.31 * t / ((t+15)*(g+2.09))

-- gss = G** = dayly sum of global radiation at a cloudless sky [MJ m-2]
-- t = T = dayly average temperature
-- c = C = parameter 54 (in coastal areas (30km)) and 93 (in the backcountry < 400m altitude)
vaWendling gss c t = (gss+c)*(t+22) / (150*(t+123))

petWinter = vaIwanow

peWinter = vaIwanow

-- ft = F(t) = ontogenesis and crop-cover dependent correction factor
-- 1 <= F(t) < 1.4 ... 1.6
pet ft t g = ft * (vaTurc t g)

pe = vaTurc

-- interzeption
----------------------------------

-- h = H = crop height [m]
-- bg = BG = degree of crop cover [0 ... 1]
-- t = at calender day
ct h bg = 2.5 * h * bg

-- pet is a value, no function
pei pet = 1.3 * pet


-- Schneeschmelze
-------------------------------




-- Bodenfeuchteberechnungen
----------------------------

glugla wia nist la =
  if wia < 0 -- vorher unter Sickergrenze
  then
    let
      b = -exp (-2 * (sqrt (la*nist)) * (1+wia/nist))
    in
      if nist >= 0 && nist + wia > 0
      then (sqrt (nist/la)) * (1+b)/(1-b)
      else nist + wia
  else -- vorher ueber Sickergrenze
    if nist == 0
    then wia / (1+la*wia)
    else
      if nist < 0
      then -- Entzug
        let
          b2 = sqrt (-nist/la)
          b3 = sqrt (-nist*la)
          b1 = atan (wia/b2)/b3
        in
          if b1 <= 1
          then nist * (1-b1)
          else
            let
              b = (sin b3) / (cos b3)
            in
              b2 * (wia-b2*b) / (b2+wia*b)
      else -- kein Entzug
        let
          b1 = sqrt (nist/la)
          b2 = (wia-b1) * (exp (-2*(sqrt (la*nist))))/(wia+b1)
        in
          b1 * (1+b2)/(1-b2)

--var   i,l,kk,j  : integer;
--      fvi,fvd     : real;
 --     vorw        : boolean;
--for i:=1 to TiefenStufen do ff[i]:=0;

f1koitzsch :: Integer -> Integer -> Bool -> [Double]
f1koitzsch n ivd bedeckt =
  -- BERECHNUNG DER ENTNAHMEVERTEILUNG
  -- MODELL   : KOITZSCH
  -- PROGRAMM : NEUMEYER / Berg
  let
    fn1 = listArray (1,15)
      [3.00000 ,1.25000, 0.777778, 0.56250, 0.44000,
      0.36111 ,0.30612 ,0.26563 ,0.23457 ,0.21000,
      0.19008, 0.17361 ,0.15976 ,0.14796, 0.13777]
    fn2 = listArray (1,15)
      [2.00000 ,0.50000 ,0.222222,0.12500 ,0.08000,
      0.055555,0.040815,0.031251,0.024691,0.02000,
      0.016528,0.013888,0.0118335,0.010204,0.0088875]
    fnub = [0.625 ,0.3, 0.075]
    is = [1,3,6,10,15]
    emptyArray = listArray (1,depth) (replicate depth 0.0)
    emptyList size = (replicate size 0.0)
    ivd' = if ivd > 1 then 20 else ivd
  in
    if bedeckt
    then -- BEDECKT
      let
        n' = if n > 15 then 15 else n
        fvi = fn1!n
        fvd = fn2!n
        --f :: Array Int Double -> Int -> Bool -> Int -> [Double]
        f array l vorw kk =
          if l == n'+1
          then elems array
          else
            let
              (kk',vorw') =
                if vorw
                then
                  if kk+1 > ivd'
                  then (ivd',False)
                  else (kk+1,vorw)
                else
                  if kk-1 < 1
                  then (1,True)
                  else (kk-1,vorw)
            in
              let
                -- assume we get an index
                i = (1+) $ fromJust $ findIndex (\e -> kk' >= e) is
                l' = fromIntegral l
                array' = array // [(i,array!i+fvi-fvd*l')]
              in
                f array' (l+1) vorw' kk'
      in
        f emptyArray 1 True 0
    else -- UNBEDECKT
      fnub ++ emptyList (depth - 3)

-- Procedure GiKoitzsch(NS,NSD :integer;
--                     bedeckt:boolean;
--                     REB    :Bodenvektor;
--                 var GI     :Bodenvektor;
--                     ivd    :integer);
{-
gikoitzsch :: Integer -> Integer -> Bool -> [Double] -> Integer -> [Double]
gikoitzsch ns nsd bedeckt reb ivd = -- gi
-- UNTERPROGRAMM ZUR BERECHNUNG DER ENTNAHMEMENGEN
-- MODELL  /  PROGRAMM : KOITZSCH  / NEUMEYER, Berg
--var  gj,ff : Bodenvektor;
--     i,j,l,m,n,nmd,km : integer;
--     Rmax,rij      : real;
--label 20;
  let
    ns' = if ns > 5 then 5 else ns
    emptyArray size = listArray (1,size) (replicate size 0.0)
    gi = emptyArray ns
    gj = emptyArray ns
    rmax = 0
    nmd = 0

for i:=1 to NS do
begin
  for l:=1 to I do
  begin
  NMD := NMD+1;
  IF NMD > NSD
    then GOTO 20
    else
    begin
    IF bedeckt AND (NMD<NSD) AND (NMD<2)
      then
      else
      begin
      RIJ:=0.0;
      F1Koitzsch(FF,NMD,ivd,bedeckt);
      for J:=1 to I do begin
                        GJ[J] := REB[J]*FF[J];
                        RIJ:=RIJ+GJ[J];
                        end;
      IF RMAX-0.002 > RIJ
         then if nmd < 5 then
                         else if n = 1 then  goto 20
                                       else  n:=1
         else
         begin
         RMAX:=RIJ;
         for M:=1 to I do gi[m] := gj[m];
         km := nmd;
         n:=0;
         end;
       end;
     end;
    end;
  end;
20:;
end; { GiKoitzsch }
-}



