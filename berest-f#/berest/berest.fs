module berest =

    open Printf
    open Microsoft.FSharp.Math
    
    
    [<Measure>] type dm
    [<Measure>] type cm
    [<Measure>] type mm
    [<Measure>] type m

//    let length = 12<cm>
//    let l = 1<m>
    
    let noOfSoilLayers = 20
    
    let interpolatedValueGeneric ((keyToFloat : 'K -> float), (valueToFloat : 'V -> float)) =
        let interpolatedValue map key : float = 
            match Map.tryFind key map with
            | Some(value) -> value |> valueToFloat
            | None -> 
                let low, high = map |> Map.partition (fun k v -> k < key)
                let _, high' = high |> Map.partition (fun k v -> key < k)
                let first m = m |> Map.toList |> List.head
                let last m = m |> Map.toArray |> (fun a -> a.[a.Length-1])
                let tf (k, v) = (k |> keyToFloat, v |> valueToFloat)
                match (low, high') with
                | (l, h) when Map.isEmpty l -> first h |> snd |> valueToFloat  
                | (l, h) when Map.isEmpty h -> last l |> snd |> valueToFloat
                | (l, h) -> 
                    let (lowerKey, lowerValue), (upperKey, upperValue) = (last l |> tf, first h |> tf)
                    lowerValue + (keyToFloat key - lowerKey)/(upperKey - lowerKey)*(upperValue - lowerValue)
        interpolatedValue 

    let interpolatedValueI2I = interpolatedValueGeneric ((float : int -> float), (float : int -> float))
    let interpolatedValueI2F = interpolatedValueGeneric ((float : int -> float), fun a -> a)

    type Schlagdaten = {
        schlagnummer : string;
        faFlaeche : float; //Schlagfläche
        beFlaeche : float; //Beregnungsfläche
        faNummer : string; //Fruchtart Nr.
        faAf : string; //Fruchtart Anbauform
        faVz : string; //Fruchtart Verwendungszweck
        faName : string; //Fruchtart-Name
        symbol : string;
        stTyp : int; //Standort-Typ
        hangN : int; //Hangneigung
        dicke3 : vector; //Dicke
        fk3 : vector; //Feldkapazität [1-3]
        pwp3 : vector; //Permanenter Welkepunkt der Bodenschichten [1-3]
        verdTiefe : int; //Schadverdichtungstiefe
        verdFlae : float; //Schadverdichtungsfläche
        knownDCDay : int; //Tag zu aktdc
        knownDC : int; //DC
        groundwaterlevel : int; //Grundwasserstand
        absCalculationDay : int;
        minWetter : int;
        ivd : int;

        fk : vector; //Feldkapazität
        pwp : vector; //potentieller Welkepunkt
        lambdaWithoutCorrection : vector;
        //void setup();
    }
                
    let absPreCalculationDay s = s.absCalculationDay - 1

    let faNrAfVz s = s.faNummer + "/" + s.faAf + "/" + s.faVz

    let ivd (s : Schlagdaten) = (if s.verdFlae > 0.5 * s.faFlaeche then float s.verdTiefe else 200.) / 10.

    let lambda (s : Schlagdaten) day = 
        let fku = Map.ofList [(90, 8); (135, 135); (175, 17); (210, 177); (240, 17); (282, 13); (306, 95)]
        let lambdaCorrection = interpolatedValueI2I fku day |> (*) 0.01
        s.lambdaWithoutCorrection |> Vector.map (fun v -> v * lambdaCorrection)     

    type Weather = {
        day : int;
        evaporation : float;
        precipitation : float;
        isPrognosis : bool;
    }

    let longtermEvapPrecip day = 
        let longtermAverageEvaporationValues = [| 
            (*01.04.*) 1.1; 1.2; 1.2; 1.2; 1.3; 1.3; 1.3; 1.4; 1.4; 1.4;
            (*11.04.*) 1.4; 1.5; 1.5; 1.5; 1.6; 1.6; 1.6; 1.7; 1.7; 1.7;
            (*21.04.*) 1.8; 1.8; 1.8; 1.9; 1.9; 1.9; 2.0; 2.0; 2.0; 2.1;
            (*01.05.*) 2.1; 2.1; 2.2; 2.2; 2.2; 2.3; 2.3; 2.3; 2.4; 2.4;
            (*11.05.*) 2.4; 2.5; 2.5; 2.5; 2.6; 2.6; 2.6; 2.7; 2.7; 2.7;
            (*21.05.*) 2.7; 2.8; 2.8; 2.8; 2.9; 2.9; 2.9; 2.9; 3.0; 3.0; 3.0;
            (*01.06.*) 3.0; 3.1; 3.1; 3.1; 3.2; 3.2; 3.2; 3.2; 3.3; 3.3;
            (*11.06.*) 3.3; 3.4; 3.4; 3.4; 3.4; 3.4; 3.4; 3.4; 3.4; 3.4;
            (*21.06.*) 3.4; 3.4; 3.4; 3.4; 3.4; 3.4; 3.4; 3.4; 3.4; 3.4;
            (*01.07.*) 3.4; 3.4; 3.3; 3.3; 3.3; 3.3; 3.3; 3.3; 3.3; 3.3;
            (*11.07.*) 3.3; 3.3; 3.3; 3.3; 3.3; 3.3; 3.3; 3.2; 3.2; 3.2;
            (*21.07.*) 3.2; 3.2; 3.2; 3.1; 3.1; 3.1; 3.1; 3.1; 3.1; 3.1; 3.0;
            (*01.08.*) 3.0; 3.0; 3.0; 3.0; 3.0; 2.9; 2.9; 2.9; 2.9; 2.9;
            (*11.08.*) 2.9; 2.9; 2.8; 2.8; 2.8; 2.8; 2.7; 2.7; 2.7; 2.7;
            (*21.08.*) 2.6; 2.6; 2.6; 2.6; 2.5; 2.5; 2.4; 2.4; 2.4; 2.4; 2.4;
            (*01.09.*) 2.3; 2.3; 2.3; 2.2; 2.2; 2.2; 2.2; 2.1; 2.1; 2.1;
            (*11.09.*) 2.0; 2.0; 2.0; 2.0; 1.9; 1.9; 1.9; 1.8; 1.8; 1.8;
            (*21.09.*) 1.7; 1.7; 1.6; 1.6; 1.6; 1.5; 1.5; 1.5; 1.4; 1.4;
            (*01.10.*) 1.4; 1.3; 1.3; 1.3; 1.2; 1.2; 1.2; 1.1; 1.1; 1.0;
            (*11.10.*) 1.0; 1.0; 0.9; 0.9; 0.9; 0.7; 0.6; 0.7; 0.5; 0.8;
            (*21.10.*) 1.0; 1.0; 0.9; 0.9; 0.9; 0.7; 0.6; 0.7; 0.5; 0.8;
        |]

        let longtermAveragePrecipitationValues = [| 
            (*01.04.*) 1.0; 1.7; 0.6; 0.5; 1.1; 0.9; 0.9; 0.9; 1.9; 1.5;
            (*11.04.*) 1.1; 0.8; 1.2; 1.5; 2.2; 0.9; 1.4; 1.1; 2.0; 0.9;
            (*21.04.*) 0.7; 1.3; 0.9; 0.9; 0.4; 0.6; 0.9; 1.0; 2.0; 1.6;
            (*01.05.*) 1.3; 1.1; 2.0; 1.5; 1.6; 1.8; 1.9; 1.3; 1.0; 1.3;
            (*11.05.*) 4.2; 0.6; 1.6; 1.5; 1.3; 0.6; 0.9; 1.9; 1.4; 4.6;
            (*21.05.*) 1.0; 0.9; 0.4; 0.9; 2.7; 1.0; 3.6; 2.8; 0.7; 2.2; 2.3;
            (*01.06.*) 1.1; 2.2; 0.6; 1.3; 1.0; 0.8; 0.7; 2.7; 4.4; 3.5;
            (*11.06.*) 2.0; 6.0; 1.3; 1.0; 1.8; 1.9; 1.5; 1.0; 3.3; 1.5;
            (*21.06.*) 1.9; 2.8; 0.7; 0.6; 3.6; 2.4; 4.1; 3.3; 3.5; 1.9;
            (*01.07.*) 1.6; 1.5; 1.9; 3.0; 3.4; 1.9; 1.1; 0.9; 2.5; 1.2;
            (*11.07.*) 1.3; 2.2; 1.5; 1.0; 2.5; 2.0; 1.9; 3.4; 1.1; 4.3;
            (*21.07.*) 3.6; 3.7; 3.6; 1.5; 0.9; 1.4; 2.1; 1.0; 1.4; 1.2; 0.9;
            (*01.08.*) 0.8; 0.5; 2.4; 1.7; 1.0; 1.3; 0.8; 1.7; 1.9; 1.3;
            (*11.08.*) 2.0; 1.7; 1.4; 0.3; 2.3; 1.7; 2.8; 1.1; 1.1; 3.1;
            (*21.08.*) 1.6; 2.9; 1.2; 1.4; 2.6; 1.4; 2.4; 3.2; 4.0; 1.6; 0.6;
            (*01.09.*) 2.1; 0.5; 0.3; 1.0; 1.4; 1.6; 3.1; 1.8; 2.6; 2.3;
            (*11.09.*) 2.9; 1.0; 1.2; 1.9; 0.6; 2.0; 1.8; 1.1; 0.7; 1.2;
            (*21.09.*) 0.6; 1.5; 0.6; 2.3; 1.2; 0.9; 0.6; 2.2; 2.3; 1.0;
            (*01.10.*) 0.6; 0.8; 2.5; 0.4; 0.7; 0.5; 2.1; 0.5; 1.1; 2.4;
            (*11.10.*) 0.8; 0.2; 0.9; 1.6; 1.0; 2.5; 1.7; 1.6; 1.5; 1.0;
            (*21.10.*) 2.2; 2.9; 1.8; 1.4; 1.2; 0.6; 1.3; 2.0; 0.4; 1.9;
        |]

        if 90 < day && day <= 90+223 then
            let index = day - 90 - 1
            (longtermAverageEvaporationValues.[index], longtermAveragePrecipitationValues.[index])
        else 
            (0.0, 0.0)

    let weatherAt map day = 
        match Map.tryFind day map with
        | Some(weather) -> weather
        | None -> 
            let e, p = longtermEvapPrecip day
            { day = day; evaporation = e; precipitation = p; isPrognosis = false; }
          
    type Donation = {
        day : int;
        amount : int;
    }

    let donationsAt donations day = 
        donations |> List.fold (fun acc {day=d; amount=a;} -> if d = day then acc + a else 0) 0
            
    type WiDatTyp = {
        faNrAfVz : string; //Fruchtart Nr Anbauform Verwendungszweck
        dc2day : Map<int, int>; //decimal code
        day2bd : Map<int, float>; //bedeckungsgrad
        day2n : Map<int, int>; //entnahmetiefe
        day2t : Map<int, float>; //transpiration
        day2qu : Map<int, float>; //quotient
        day2ef : Map<int, float>; //effektivität
        dc2name : Map<int, string>; // name decimal code
    }

    type TechnTyp = {
        hhMax : int;
        hhMin : int;
        hhn : int;
        hht : int;
        hhNorm : int;
        hhStufung : int;
        turnus : int;
        hhOpt : int;
        empfText : string;
    }

    type QuType = {
        qusoll : float;
        qu00 : float;
        quhh : float;
        quef : float; 
    }
    
    type CodeEinheit = | PFK | PNFK | Volp | MM

    type BMTyp = {
        snru : int;
        snrus : string;
        tag : int;
        dc : int;
        flaeche : float;
        grws : int<m>; //[dm]
        einheit : CodeEinheit;
        dicke2wert: Map<int, double>;
    }

    type IrrigationMode = | SprinkleLosses | NoSprinkleLosses 

    ///wie = WiE = Wassergehalt am Ende des Zeitabschnittes i (mm) = finalExcessWater
    let glugla (wia : double) (nist : double) (lai : double) : double =
        //more water will fit into the current layer (below infiltration barrier)
        if wia < 0. then
            let wie = nist + wia
            if nist >= 0. && wie > 0. then
                let b2 = -exp(-2.0*sqrt(lai*nist)*(1.0 + wia/nist))
                sqrt(nist/lai)*(1.0 + b2)/(1.0 - b2)
            else
                wie
        //current layer already above infiltration barrier 
        else 
            if nist = 0. then
                wia/(1.0 + lai*wia)
            // Entzug
            elif nist < 0. then
                let n7l = sqrt(-nist/lai)
                let nl = sqrt(-nist*lai)
                let at = atan(wia/n7l)/nl
                if at <= 1. then
                    nist*(1.0 - at)
                else
                    let beta = sin(nl)/cos(nl)
                    n7l*(wia - n7l*beta)/(n7l + wia*beta)
            //if nist > 0 // kein Entzug
            else 
                let n7l = sqrt(nist/lai)
                let nl = sqrt(lai*nist)
                let alpha = (wia - n7l)/(wia + n7l)*exp(-2.0*nl)
                n7l*(1.0 + alpha)/(1.0 - alpha)
             
    let interception (precipitation : double) (evaporation : double) (irrigation : double) (transpirationFactor : double) (im : IrrigationMode) =
        let null5 : double = 0.5
        let null2 : double = 0.2
        let tf = max 1. transpirationFactor 

        // Berechnung fuer natuerlichen Regen
        let tin = null5 + (evaporation - 1.)*tf*null2

        let interceptionPrecipitation, pet = 
            if precipitation > 0. then
                (tin, evaporation - tin*null5)
            else
                (0., evaporation)

        // Berechnung fuer Zusatzregen/Spruehverluste
        let interceptionIrrigation, sprinkleLoss, pet' = 
            if irrigation > 1. then 
                let ii, sl = 
                    match im with
                    | SprinkleLosses -> (0.6*tin*(1.0 + irrigation*0.05), (1. + (evaporation - 2.5)*null2)*null2*irrigation)
                    | NoSprinkleLosses -> (0., 0.)
                if precipitation > 0. then
                    (ii, sl, evaporation - (ii + interceptionPrecipitation)*null5)
                else
                    (ii, sl, evaporation - ii*0.75)
            else
                (0., 0., pet)
        
        (max 0. pet', 
         precipitation - interceptionPrecipitation, 
         irrigation - interceptionIrrigation - sprinkleLoss, 
         irrigation - sprinkleLoss)   
                
    let waterAbstraction maxDepth = 
        let fraction (layerNo : int) =
            let ln, nol = (double layerNo, double maxDepth)
            (2. - (2.*ln - 1.)/nol)/nol
        [| for i in 1 .. noOfSoilLayers -> if i <= maxDepth then fraction i else 0. |]
    
    let f1Koitzsch (maxDepthDM : int) isCovered =
        match isCovered with
        | false -> [| 0.625; 0.3; 0.075 |] //to be fixed
        | true -> min 20 maxDepthDM |> waterAbstraction 
        |> Vector.ofArray

//    let take no l = 
//        let revl = List.rev l
//        let rec helper no rl = 
//            if no <= 0 then rl
//            else helper (no - 1) (List.tail rl)
//        l |> List.rev |> helper (l.Length - no) |> List.rev

    let giKoitzsch isCovered extractionDepthDM (re : vector) =
        let ffs = List.init re.Length (fun i -> ((i+1), f1Koitzsch (i+1) isCovered)) 
        let rec findLargest rmax gi ffs = 
            match ffs with
            | (depthDM, ff)::rest -> 
                let gj = re .* ff
                let rij = Vector.sum gj
                if (rmax - 0.002 > rij && depthDM >= 5) || depthDM > extractionDepthDM then
                    gj
                else
                    findLargest rij gj rest
            | [] -> gi
        findLargest 0. (Vector.zero noOfSoilLayers) ffs
                
    /// fk in a one dm layer                
    let capillaryRiseBarrier fk = fk + 40.667 - fk*0.408

    let inline nFK fk pwp = fk - pwp

    let infiltrationBarrier fk pwp absCurrentDay layerDepthCM =
        let barriers = 
            if layerDepthCM <= 30 then 
                [(80, 11); (140, 7)]
            else 
                [(100, 10); (200, 8)] 
            |> Map.ofList
        let vs = interpolatedValueI2I barriers absCurrentDay |> (*) 0.1
        let vs' = 
            let pwfk = pwp/fk - 1. + vs
            if vs < 1. && pwfk > 0. then
                vs + (1.0 - vs)*pwfk*0.95/(0.66 - 0.30)
            else
                vs
        (nFK fk pwp)*vs' + pwp

    let inline pwp4p fk pwp = pwp - 0.04*(nFK fk pwp)

    ///calculate reduction factors for uncovered soil
    let reu (fks : vector) (pwps : vector) (soilMoisture : vector) =
        Vector.mapi 
            (fun i sm -> 
                let fk, pwp = (fks.[i], pwps.[i])
                let pwp4p' = pwp4p fk pwp
                let r = 
                    if sm > pwp4p' then 
                        min ((sm - pwp4p')/(fk - pwp4p')) 1.0 
                    else 
                        0.
                r*r
            ) soilMoisture

    let reb extractionDepthDM (fks : vector) (pwps : vector) (soilMoisture : vector) pet =
        Vector.mapi 
            (fun i sm ->
                let fk, pwp = (fks.[i], pwps.[i])
                let pwp4p' = pwp4p fk pwp
                if i <= extractionDepthDM && sm < fk then
                    let nfk = nFK fk pwp
                    let xsm = 
                        if fk < 10. || fk > 46. then nfk*0.81 + pwp
                        else    (67.77
                                + 3.14*fk
                                - 0.2806*fk*fk
                                + 0.008131*fk*fk*fk
                                - 0.0000735*fk*fk*fk*fk
                                )*nfk*0.01 + pwp
                    let rk = if pet > 3.0 then (fk - xsm)*0.0666 else (xsm - pwp)*0.3333
                    let smCrit = xsm + (pet - 3.0)*rk
                    if sm < smCrit then
                        let pwp12p = pwp - 0.12*nfk
                        if pwp < sm then 
                            (sm - pwp12p)/(smCrit - pwp12p)
                        elif pwp4p' < sm then 
                            let sreb = 3. // { sreb=(pwp-pw7)/(pwp-pw9)=3.0 }
                            (sm - pwp4p')/(smCrit - pwp12p)*sreb
                        else 
                            0.
                    else 
                        1.
                else
                    1.

            ) soilMoisture

    let sm1day extractionDepthDM coverDegree (pet : double) absCurrentDay 
        (fks : vector) (pwps : vector) (lambdas : vector) (soilMoisture : vector)
        isSoilMoisturePrognosis (evaporation : float) ivd groundwaterlevel 
        dailyPrecipitationAndIrrigation = 
        let reu' = reu fks pwps soilMoisture
        let reb' = 
            if coverDegree > 0.01 then 
                reb extractionDepthDM fks pwps soilMoisture pet
            else
                vector [ 1. .. 20. ] 
        //for at least partly uncovered ground
        let aet, waterAbstraction = 
            (0., 
                if coverDegree >= 0.99 then
                    Vector.zero 20
                 else
                    reu' |> giKoitzsch false 6 |> (*) evaporation
            )
        //for at least partly covered ground
        let aet', waterAbstraction' =
            if coverDegree <= 0.01 then
                (0., waterAbstraction)
            else
                let extractionDepthDM' = min extractionDepthDM 6 
                let gi = reb' |> giKoitzsch true extractionDepthDM' 
                (
                    pet*gi |> Vector.sum,
                    waterAbstraction*(1. - coverDegree) + gi*pet*coverDegree
                )

        //calculation of soil moisture
        let infiltrationFromPrevLayer = dailyPrecipitationAndIrrigation

        let groundwaterInfiltration, sms = 
            Vector.foldi 
                (fun i (infiltrationFromPrevLayer, sms) sm ->
                    let fk, pwp = (fks.[i], pwps.[i])
                    let crBarrier = capillaryRiseBarrier fk
                    let depthDM = i + 1
                    if depthDM <= groundwaterlevel then
                        //above that barrier the water will start to infiltrate to the next layer
                        let infBarrier = infiltrationBarrier fk pwp absCurrentDay (depthDM*10)

                        //in the next steps we basically care just about the excess water
                        //not about the layers full water content, so we calculate
                        //just the difference (positive or negative) to the infiltration barrier
                        //(in case of the first layer, possibly including todays precipitation)
                        let initialExcessWater = sm - infBarrier

                        let netInfiltrationFromAboveLayer = infiltrationFromPrevLayer - waterAbstraction'.[i]

                        //the excess water after calculation of the infiltration to the next layer
                        let finalExcessWater = glugla initialExcessWater netInfiltrationFromAboveLayer lambdas.[i]

                        //calculate the resulting infiltration for the next layer from this layer
                        let infiltrationFromPrevLayer' = initialExcessWater + netInfiltrationFromAboveLayer - finalExcessWater

                        //add the (positive/negative) excess water back to the infiltration barrier
                        //to obtain the actual water content now in this layer
                        //(after water could infiltrate to the next layer)
                        let pwp4p = pwp - 0.04*(nFK fk pwp)
                        let sm' = max (infBarrier + finalExcessWater) pwp4p

                        (infiltrationFromPrevLayer', Array.append sms [|sm'|])
                    else
                        (0., Array.append sms [|crBarrier|])
                ) (0., Array.empty) soilMoisture

        //VERTIKALER AUSGLEICH BEI UEBERFEUCHTUNG
        //calculate the capillary rise if applicable
        let soilMoisture' =
            let length = sms.Length
            sms |> Array.rev |> Array.fold 
                (fun (i, excessWater, sms) sm ->
                    let fk = fks.[length-i-1]
                    let crBarrier = capillaryRiseBarrier fk
                    let sm' = sm + excessWater
                    let excessWater' = max 0.0 (sm' - crBarrier)
                    (
                        i + 1, 
                        excessWater',
                        Array.append sms [|sm' - excessWater'|]
                    )
                 ) (0, 0., Array.empty)
                |> (fun (_,_,t) -> t) |> Array.rev
          
        (aet', vector soilMoisture', groundwaterInfiltration)                  

    
    let bfXdays xDays absCurrentDay relDCDay (soilMoisture : vector)
        schlag bfprognose (wiDat : WiDatTyp) im donations weather out = 
        
        let rec bfXdays' days quAkt quSum aet soilMoisture groundwaterInfiltration =
            if days = 0 then
                (quAkt, quSum, aet, soilMoisture, groundwaterInfiltration)
            else
                let relDCDay', absCurrentDay' = 
                    let delta = xDays - days
                    (relDCDay + delta, absCurrentDay + delta) 
                let prevDayCoverDegree = interpolatedValueI2F wiDat.day2bd (relDCDay' - 1)
                let currentQuSoll = if prevDayCoverDegree < 0.01 then 0. else interpolatedValueI2F wiDat.day2qu relDCDay'
                let coverDegree = interpolatedValueI2F wiDat.day2bd relDCDay'
                let roundedExtractionDepthDM = 
                    if coverDegree <= 0.001 then 0 else interpolatedValueI2I wiDat.day2n relDCDay' |> (+) 0.1 |> round |> int
                let wetterBf = weatherAt weather absCurrentDay'
                let hhzus = donationsAt donations absCurrentDay' |> float
                let transpirationFactor = interpolatedValueI2F wiDat.day2t relDCDay'
                let pet', effectivePrecipitation, effectiveIrrigation, effectiveIrrigationUncovered = 
                    interception wetterBf.precipitation wetterBf.evaporation hhzus transpirationFactor im
                let pet = if coverDegree < 0.001 then 0. else pet' * transpirationFactor
                let dailyPrecipitationAndIrrigation =
                    (effectivePrecipitation + effectiveIrrigation) * coverDegree +
                    (wetterBf.precipitation + effectiveIrrigationUncovered) * (1.0 - coverDegree)

                let (aet', soilMoisture', groundwaterInfiltration) = 
                    sm1day roundedExtractionDepthDM coverDegree pet absCurrentDay' schlag.fk
                        schlag.pwp (lambda schlag absCurrentDay') soilMoisture
                        bfprognose wetterBf.evaporation schlag.ivd
                        schlag.groundwaterlevel dailyPrecipitationAndIrrigation

                let quAet7Pet = 
                    if coverDegree < 0.001 then 0.
                    elif pet > 0.01 then aet'/pet
                    else 1.

                let out = 
                    sprintf "%d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %d"
                        absCurrentDay wetterBf.precipitation (-wetterBf.evaporation) hhzus pet aet quAet7Pet 
                        currentQuSoll groundwaterInfiltration
                        (soilMoisture.[1] |> round)
                        (soilMoisture.[2] |> round) 
                        (soilMoisture.[3] |> round) 
                        (soilMoisture.[4] |> round)
                        (soilMoisture.[5] |> round)
                        (soilMoisture.[1..2] |> Vector.sum) 
                        soilMoisture.[3] 
                        relDCDay

                bfXdays' 
                    (days - 1) 
                    (if quAet7Pet < currentQuSoll then quAkt + currentQuSoll - quAet7Pet else quAkt)
                    (quSum + currentQuSoll)
                    aet' soilMoisture' groundwaterInfiltration

        let quAkt, quSum, aet, soilMoisture, groundwaterInfiltration = bfXdays' xDays 0. 0. 0. (vector []) 0.
        
        ((quSum - quAkt) / double xDays, quSum / float xDays)
        
    let pFK2mm percentValue fk = 0.01*percentValue*fk
    let pNFK2mm percentValue fk pwp = pwp + 0.01*percentValue*(fk - pwp)
    let mm2pNFK mmValue fk pwp = (mmValue - pwp)/(fk - pwp)*100.0
    let volP2mm value = value

//struct  ReBFRes
//{
//	ReBFRes() { initialSoilMoisture.fill(0); }
//	int absInitialSoilMoistureDay;
//	Bodenvektor initialSoilMoisture;
//	double groundwaterLevel;
//};
//    let reBF snru minDatum (fk : vector) (pwp : vector) = 
//        let calculationThickness = [| -9999; 1; 3; 6; 10; 15; 16 |]
//
//        BMTyp bm = getValue(bodenfeuchteLesen(snru, minDatum));
//	res.groundwaterLevel = bm.grws;
//
//	int i = 1, k = 1;
//	auto ci = bm.dicke2wert.begin();
//	while(i < 16 && ci != bm.dicke2wert.end())
//	{
//		int thickness = ci->first;
//		double value = ci->second;
//		switch(bm.einheit)
//		{
//		case pFK:
//			res.initialSoilMoisture[k] += pFK2mm(value, fk[k])/double(k);
//			break;
//		case pNFK:
//			res.initialSoilMoisture[k] += pNFK2mm(value, fk[k], pwp[k])/double(k);
//			break;
//		case Volp: case mm:
//			res.initialSoilMoisture[k] += volP2mm(value);
//			break;
//		default: ;
//		}
//		if(i == calculationThickness[k])
//			k++;
//		i++;
//		if(thickness < calculationThickness[k])
//			ci++;
//	}
//	res.absInitialSoilMoistureDay=bm.tag;
//
//	return res;
//}

































    //vector [ 1.0; 1.0 ] |> giKoitzsch true 20 |> ignore
    
    #if INTERACTIVE
    #I @"C:\Users\michael\development\irrigation\src\f#\berest\packages\mongocsharpdriver.1.4\lib\net35"
    #r "MongoDB.Bson.dll"
    #r "MongoDB.Driver.dll"
    #r "FSharp.PowerPack.Linq.dll"
    #r "System.Core.dll"
    #endif

    open System
    open MongoDB.Bson
    open MongoDB.Driver
    open MongoDB.Driver.Linq
    open System.Linq
    
    let server = MongoServer.Create()
    //let server = MongoServer.Create @"mongodb://localhost"
    let db = server.GetDatabase("berest")
    db.ToString() |> printf "db: %s\n"

//    db.["movies"].Insert((new Document()).Append("title", "Star Wars").Append("releaseDate", DateTime.Now))

    let schlags = db.["schlags"].AsQueryable() :> IQueryable<BsonDocument>
        
    db.GetCollectionNames() |> Seq.iter (printf "%s\n")

    let schlags2 = db.["schlags"]
    let schlags3 = schlags2.AsQueryable() :> IQueryable<BsonDocument>
    let one = schlags2.FindOne()
    let b = one.["pwp"].AsBsonArray
    b.ToArray() |> Seq.iter (fun bv -> bv.AsDouble |> printf "%f\n")


    let s1 = schlags3 |> Seq.toList
    //let x = s1.["schlagnummer"].AsString = "0400"

    let s = 
        query {  
            for schlag in schlags3 do
                if schlag.["faFlaeche"].AsInt32 = 1 then
                    yield schlag.["schlagnummer"].AsString 
        } |> Seq.toList
        
    s |> List.iter (printf "schlagnummer: %s\n")

    let a = 1
  
//    let todaysWeather = { day = 1; evaporation = 2.; precipitation = 10.; isPrognosis = false }
//    
//    #if INTERACTIVE
//    #I @"C:\Users\michael\development\irrigation\src\f#\berest\packages\RavenDB.1.0.888\lib\net40"
//    #r "Raven.Abstractions.dll"
//    #r "Raven.Client.Debug.dll"
//    #r "Raven.Client.Lightweight.dll"
//    #r "Raven.Client.Lightweight.FSharp.dll"
//    #I @"C:\Users\michael\development\irrigation\src\f#\berest\packages\Newtonsoft.Json.4.0.8\lib\net40"
//    #r "Newtonsoft.Json.dll"
//    #I @"C:\Users\michael\development\irrigation\src\f#\berest\packages\NLog.2.0.0.2000\lib\net40"
//    #r "NLog.dll"
////    #r "FSharp.PowerPack.Linq.dll"
//    #r "System.Core.dll"
//    #endif
//
//    open Raven.Client
//    open Raven.Client.Document
//    open Raven.Client.Operators
//    
////    let store = new DocumentStore(Url = "http://myravendb.mydomain.com/")
////    store.Conventions.CustomizeJsonSerializer <- (fun s -> s.Converters.Add(new UnionTypeConverter()))
//    let docStore = DocumentStore.OpenInitializedStore "berest-ravendb"
////    using (docStore.OpenSession())
////        (fun s -> store todaysWeather |> run s |> ignore)    
//        
//    let session = docStore.OpenSession()
//        //store todaysWeather |> run session |> ignore
//    
//    let query = 
//        raven { 
//            return! query (where <@ fun x -> x.day < 10 @>)
//        }
//
//    let result = 
//        use session = docStore.OpenSession()
//        query |> run session
//
//    let a = 1
//
//    session.Dispose ()

//    let pathToSQLiteDB = @"C:\Users\michael\development\irrigation\src\f#\berest\monica.sqlite"
//    #if INTERACTIVE
//    #I @"C:\Users\michael\development\irrigation\src\f#\berest\packages\System.Data.SQLite.x64.1.0.80.0\lib\net40"
//    //#r @"C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.0\System.Data.Linq.dll"
//    //#r "D:\Projects\Research\Libraries\FSharp.PowerPack.dll"
//    //#r @"D:\Projects\Research\Libraries\FSharp.PowerPack.Linq.dll"
//    #r "System.Data"
//    #r "System.Data.SQLite.dll"
//    #r "System.Data.SQLite.Linq.dll"
//    #endif
//
//    open System.Data.Linq
//    open System.Data.Linq.Mapping
//    open System.Data.SQLite
//
//    open Microsoft.FSharp.Linq
//    //open Microsoft.FSharp.Linq.Query
//
//    [<Table(Name="bodengebiete")>]
//    type Bodengebiet (desc : string, count : int, bo_num : int) =
//        let mutable _description = desc
//        let mutable _count = count
//        let mutable _bo_num = bo_num
//
//        new() = Bodengebiet("leer", 0, 0)
//
//        [<Column(Name="bodengebie")>]
//        member this.description
//            with get() = _description
//            and set(value) = _description <- value
//
//        [<Column(Name="count")>]
//        member this.count
//            with get() = _count
//            and set(value) = _count <- value
//
//        [<Column(Name="bo_num")>]
//        member this.bo_num
//            with get() = _bo_num
//            and set(value) = _bo_num <- value
//
//        override this.ToString() = sprintf "desc: %s count: %d boNum: %d" _description _count _bo_num
//
//    let connString = sprintf "Data Source=%s;UTF8Encoding=True;Version=3" pathToSQLiteDB
//    let conn = new SQLiteConnection(connString);
//    let db = new DataContext(conn)
//    let bodengebiete = db.GetTable<Bodengebiet>()
//
//    //query for company names
//    let descs = 
//        seq {
//            for bg in bodengebiete do
//                yield bg.description
//        } |> Seq.toList
//
//    descs |> List.iter (printf "%s\n")
//
//    
//    //find company with id "ALFKI"
//    let bg771 = 
//        query { 
//            for bg in bodengebiete do
//                if bg.count = 771 then
//                    yield bg
//        } |> Seq.head 
//
//    //update name and save to DB
//    bg771.bo_num <- 8
//    
//    db.SubmitChanges()
//
//    let a = 1
