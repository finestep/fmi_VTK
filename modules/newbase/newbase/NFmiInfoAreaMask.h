// ======================================================================
/*!
 * \file NFmiInfoAreaMask.h
 * \brief Interface of class NFmiInfoAreaMask
 */
// ======================================================================

#pragma once

#include "NFmiAreaMaskImpl.h"
#include "NFmiDataIdent.h"
#include "NFmiGrid.h"  // täältä tulee NFmiTimeCache-luokka (tiedän, pitäisi jakaa newbase:a osiin)
#include "NFmiLevel.h"
#include "NFmiMetTime.h"

class NFmiDataModifier;

// Perus areamask-luokka.
// Sisältää myös listan mahdollisiin 'ali'-maskeihin. Niihin yhteys vain
// indeksin kautta. Ylimaski on indeksillä 1 ja muut seuraavat numerojärjestyksessä.

//! Undocumented

class _FMI_DLL NFmiInfoAreaMask : public NFmiAreaMaskImpl
{
 public:
  virtual ~NFmiInfoAreaMask(void);
  NFmiInfoAreaMask(void);
  NFmiInfoAreaMask(const NFmiCalculationCondition &theOperation,
                   Type theMaskType,
                   NFmiInfoData::Type theDataType,
                   const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                   BinaryOperator thePostBinaryOperator = kNoValue);

  NFmiInfoAreaMask(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                   BinaryOperator thePostBinaryOperator = kNoValue);
  NFmiInfoAreaMask(const NFmiInfoAreaMask &theOther);
  NFmiAreaMask *Clone(void) const;
  NFmiInfoAreaMask &operator=(const NFmiInfoAreaMask &theMask) = delete;

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);
  double HeightValue(double theHeight, const NFmiCalculationParams &theCalculationParams);
  double PressureValue(double thePressure, const NFmiCalculationParams &theCalculationParams);
  // Pitää olla ei virtuaalinen versio PressureValue-metodista, jotta tietyissä tilanteissa estetään
  // mahdollisen lapsiluokan metodin virtuaalinen kutsu
  double PressureValueStatic(double thePressure, const NFmiCalculationParams &theCalculationParams);
  double HeightValueStatic(double theHeight, const NFmiCalculationParams &theCalculationParams);

  // erikoistapaus optimoituun käyttöön (ei voi käyttää kaikille luokille!!!!)
  bool IsMasked(int theIndex) const;
  bool IsMasked(const NFmiPoint &theLatLon) const { return NFmiAreaMaskImpl::IsMasked(theLatLon); }
  bool Time(const NFmiMetTime &theTime);
  bool IsWantedParam(const NFmiDataIdent &theParam, const NFmiLevel *theLevel = 0) const;
  boost::shared_ptr<NFmiFastQueryInfo> Info(void) { return itsInfo; };
  void Info(const boost::shared_ptr<NFmiFastQueryInfo> &newInfo);
  void UpdateInfo(boost::shared_ptr<NFmiFastQueryInfo> &theInfo);

  // tehty virtuaaliseksi, koska perusluokassa ei ole valmiuksia paaluttaa arvoa tähän
  const NFmiDataIdent *DataIdent(void) const;

  // tehty virtuaaliseksi, koska perusluokassa ei ole valmiuksia paaluttaa arvoa tähän
  const NFmiParam *Param(void) const;

  // tehty virtuaaliseksi, koska perusluokassa ei ole valmiuksia paaluttaa arvoa tähän
  const NFmiLevel *Level(void) const;
  void Level(const NFmiLevel &theLevel);

  // tehty virtuaaliseksi, koska perusluokassa ei ole valmiuksia paaluttaa arvoa tähän
  bool UseLevelInfo(void) const;
  bool UsePressureLevelInterpolation(void) const { return fUsePressureLevelInterpolation; }
  void UsePressureLevelInterpolation(bool newValue) { fUsePressureLevelInterpolation = newValue; }
  double UsedPressureLevelValue(void) const { return itsUsedPressureLevelValue; }
  void UsedPressureLevelValue(double newValue) { itsUsedPressureLevelValue = newValue; }

  static boost::shared_ptr<NFmiDataModifier> CreateIntegrationFuction(NFmiAreaMask::FunctionType func);
  static bool CalcTimeLoopIndexies(boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
      const NFmiCalculationParams &theCalculationParams,
      double theStartTimeOffsetInHours,
      double theEndTimeOffsetInHours,
      unsigned long *theStartTimeIndexOut,
      unsigned long *theEndTimeIndexOut);
  static bool CalcTimeLoopIndexies(boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
      const NFmiCalculationParams &theCalculationParams,
      const NFmiMetTime &theStartTime,
      const NFmiMetTime &theEndTime,
      unsigned long *theStartTimeIndexOut,
      unsigned long *theEndTimeIndexOut);
  static void AddValuesToFunctionModifier(boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
      boost::shared_ptr<NFmiDataModifier> &theFunctionModifier,
      const NFmiLocationCache &theLocationCache,
      NFmiAreaMask::FunctionType integrationFunction);
 protected:
  virtual double CalcValueFromLocation(const NFmiPoint &theLatLon) const;
  virtual const NFmiString MakeSubMaskString(void) const;

 protected:
  boost::shared_ptr<NFmiFastQueryInfo> itsInfo;
  NFmiDataIdent itsDataIdent;
  NFmiLevel itsLevel;
  NFmiMetTime itsTime;  // jos vanhassa maski jutussa tarvitaan aikainterpolointia, tässä on
                        // interpoloitava aika
  bool fIsTimeIntepolationNeededInValue;  // erikois optimointia Value-metodin ja Time-metodin
                                          // käytössä
  bool fUsePressureLevelInterpolation;    // tämä asetetaan trueksi jos tarvitaan tehdä lennossa
                                          // painepinta interpolointeja
  double itsUsedPressureLevelValue;  // jos fUsePressureLevelInterpolation on true, käytetään
                                     // laskuissa tätä painepintaa

};  // class NFmiInfoAreaMask

//! Tämä luokka toimii kuten NFmiInfoAreaMask mutta kurkkaa halutun x-y hila pisteen yli arvoa
class _FMI_DLL NFmiInfoAreaMaskPeekXY : public NFmiInfoAreaMask
{
 public:
  virtual ~NFmiInfoAreaMaskPeekXY(void);
  NFmiInfoAreaMaskPeekXY(void);
  NFmiInfoAreaMaskPeekXY(const NFmiCalculationCondition &theOperation,
                         Type theMaskType,
                         NFmiInfoData::Type theDataType,
                         const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                         int theXOffset,
                         int theYOffset,
                         BinaryOperator thePostBinaryOperator = kNoValue);

  NFmiInfoAreaMaskPeekXY(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                         int theXOffset,
                         int theYOffset,
                         BinaryOperator thePostBinaryOperator = kNoValue);
  NFmiInfoAreaMaskPeekXY(const NFmiInfoAreaMaskPeekXY &theOther);
  NFmiAreaMask *Clone(void) const;
  NFmiInfoAreaMaskPeekXY &operator=(const NFmiInfoAreaMaskPeekXY &theMask) = delete;

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);
  double PressureValue(double thePressure, const NFmiCalculationParams &theCalculationParams);

 protected:
  NFmiCalculationParams MakeModifiedCalculationParams(
      const NFmiCalculationParams &theCalculationParams);

  int itsXOffset;  // kuinka monen hilapisteen yli kurkataan x-suunnassa
  int itsYOffset;  // kuinka monen hilapisteen yli kurkataan y-suunnassa
 private:
  void CalcGridDiffs(void);

  double itsGridXDiff;  // mikä on x-hilaväli xy-koordinaatistossa
  double itsGridYDiff;  // mikä on y-hilaväli xy-koordinaatistossa

};  // class NFmiInfoAreaMaskPeekXY

//! Tämä luokka toimii kuten NFmiInfoAreaMaskPeekXY käyttää kurkkaukseen editoritavan-datan tai
//! macroParam-datan hilaväliä, tällöin voidaan käyttää uutta GridSizeX ja Y funktioita
class _FMI_DLL NFmiInfoAreaMaskPeekXY2 : public NFmiInfoAreaMask
{
 public:
  virtual ~NFmiInfoAreaMaskPeekXY2(void);
  NFmiInfoAreaMaskPeekXY2(void);
  NFmiInfoAreaMaskPeekXY2(const NFmiCalculationCondition &theOperation,
                          Type theMaskType,
                          NFmiInfoData::Type theDataType,
                          const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                          const boost::shared_ptr<NFmiFastQueryInfo> &theEditedInfo,
                          int theXOffset,
                          int theYOffset,
                          BinaryOperator thePostBinaryOperator = kNoValue);

  NFmiInfoAreaMaskPeekXY2(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                          const boost::shared_ptr<NFmiFastQueryInfo> &theEditedInfo,
                          int theXOffset,
                          int theYOffset,
                          BinaryOperator thePostBinaryOperator = kNoValue);
  NFmiInfoAreaMaskPeekXY2(const NFmiInfoAreaMaskPeekXY2 &theOther);
  NFmiAreaMask *Clone(void) const;
  NFmiInfoAreaMaskPeekXY2 &operator=(const NFmiInfoAreaMaskPeekXY2 &theMask) = delete;

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);
  double PressureValue(double thePressure, const NFmiCalculationParams &theCalculationParams);

 protected:
  NFmiCalculationParams MakeModifiedCalculationParams(
      const NFmiCalculationParams &theCalculationParams);

  int itsXOffset;  // kuinka monen hilapisteen yli kurkataan x-suunnassa
  int itsYOffset;  // kuinka monen hilapisteen yli kurkataan y-suunnassa
 private:
  boost::shared_ptr<NFmiFastQueryInfo>
      itsEditedInfo;  // Tästä katsotaan vain haluttu siirtymä matka

};  // class NFmiInfoAreaMaskPeekXY2

//! Tämä luokka toimii kuten NFmiInfoAreaMaskPeekXY ja  käyttää kurkkaukseen haluttuja kilometri
//! etäisyyksiä
class _FMI_DLL NFmiInfoAreaMaskPeekXY3 : public NFmiInfoAreaMask
{
 public:
  virtual ~NFmiInfoAreaMaskPeekXY3(void);
  NFmiInfoAreaMaskPeekXY3(void);
  NFmiInfoAreaMaskPeekXY3(const NFmiCalculationCondition &theOperation,
                          Type theMaskType,
                          NFmiInfoData::Type theDataType,
                          const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                          const boost::shared_ptr<NFmiFastQueryInfo> &theEditedInfo,
                          double theXOffsetInKM,
                          double theYOffsetInKM,
                          BinaryOperator thePostBinaryOperator = kNoValue);

  NFmiInfoAreaMaskPeekXY3(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                          const boost::shared_ptr<NFmiFastQueryInfo> &theEditedInfo,
                          double theXOffsetInKM,
                          double theYOffsetInKM,
                          BinaryOperator thePostBinaryOperator = kNoValue);
  NFmiInfoAreaMaskPeekXY3(const NFmiInfoAreaMaskPeekXY3 &theOther);
  NFmiAreaMask *Clone(void) const;
  NFmiInfoAreaMaskPeekXY3 &operator=(const NFmiInfoAreaMaskPeekXY2 &theMask) = delete;

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);
  double PressureValue(double thePressure, const NFmiCalculationParams &theCalculationParams);

 protected:
  NFmiCalculationParams MakeModifiedCalculationParams(
      const NFmiCalculationParams &theCalculationParams);

  double itsXOffsetInKM;  // kuinka monen kilometrin yli kurkataan x-suunnassa
  double itsYOffsetInKM;  // kuinka monen kilometrin yli kurkataan y-suunnassa
 private:
  boost::shared_ptr<NFmiFastQueryInfo>
      itsEditedInfo;  // Tästä katsotaan vain haluttu siirtymä matka

};  // class NFmiInfoAreaMaskPeekXY3

class _FMI_DLL NFmiInfoAreaMaskMetFuncBase : public NFmiInfoAreaMask
{
 public:
  ~NFmiInfoAreaMaskMetFuncBase(void);
  NFmiInfoAreaMaskMetFuncBase(const NFmiCalculationCondition &theOperation,
                              Type theMaskType,
                              NFmiInfoData::Type theDataType,
                              const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                              bool thePeekAlongTudes,
                              MetFunctionDirection theMetFuncDirection,
                              BinaryOperator thePostBinaryOperator = kNoValue);
  NFmiInfoAreaMaskMetFuncBase(const NFmiInfoAreaMaskMetFuncBase &theOther);
  NFmiInfoAreaMaskMetFuncBase &operator=(const NFmiInfoAreaMaskMetFuncBase &theMask) = delete;
  void Initialize(void);

  bool Time(const NFmiMetTime &theTime);

 protected:
  typedef std::vector<std::pair<int, float> > CalcFactorVector;

  virtual void SetDividers(void) = 0;
  virtual void InitCalcFactorVectors(void) = 0;
  virtual const CalcFactorVector &LowerEdgeFactors(void) const = 0;
  virtual const CalcFactorVector &UpperEdgeFactors(void) const = 0;
  virtual const CalcFactorVector &MiddleAreaFactors(void) const = 0;

  NFmiLocationCache CalcLocationCache(const NFmiPoint &theLatlon);
  void SetGridSizeVariables(void);
  bool IsDataOperatable(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo) const;
  float Peek(const NFmiLocationCache &theLocationCachePoint, int offsetX, int offsetY);
  NFmiLocationCache CalcPeekedLocation(const NFmiLocationCache &theLocationCachePoint,
                                       int theOffsetX,
                                       int theOffsetY);
  float CalcMetFuncWithFactors(const NFmiLocationCache &theLocationCachePoint,
                               bool fDoX,
                               const CalcFactorVector &theCalcFactors);
  float CalcUpperEdgeMetFunc(const NFmiLocationCache &theLocationCachePoint, bool fDoX);
  float CalcLowerEdgeMetFunc(const NFmiLocationCache &theLocationCachePoint, bool fDoX);
  float CalcMiddleAreaMetFunc(const NFmiLocationCache &theLocationCachePoint, bool fDoX);
  float CalcMetFuncComponent(const NFmiLocationCache &theLocationCachePoint,
                             bool fDoX,
                             double theRealGridPointComponent,
                             int theGridSizeComponent);

  NFmiTimeCache
      itsTimeCache;  // tähän lasketaan time-metodissa optimoinnissa käytetty aika-cache laskuri
  int itsGridSizeX;
  int itsGridSizeY;
  float itsGridPointWidthInMeters;
  float itsGridPointHeightInMeters;
  float itsMetFuncDividerX;  // tähän lasketaan luku, millä jaetaan laskut X-suunnassa
  float itsMetFuncDividerY;  // tähän lasketaan luku, millä jaetaan laskut Y-suunnassa
  bool fTotalWindParam;  // jos parametrina on total wind- itää se ottaa tietyissä tapauksissa
                         // erikseen huomioon laskuissa
  bool fPeekAlongTudes;  // tehdäänkö xy-suuntaiset kurkkaukset datan oman hilan suunnassa (false)
                         // vaiko pitkin latitude ja longitude linjoja (true)
};

//! Tämä luokka laskee gradientin tai divergenssin.
class _FMI_DLL NFmiInfoAreaMaskGrad : public NFmiInfoAreaMaskMetFuncBase
{
 public:
  ~NFmiInfoAreaMaskGrad(void);
  NFmiInfoAreaMaskGrad(const NFmiCalculationCondition &theOperation,
                       Type theMaskType,
                       NFmiInfoData::Type theDataType,
                       const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                       bool thePeekAlongTudes,
                       MetFunctionDirection theMetFuncDirection,
                       BinaryOperator thePostBinaryOperator = kNoValue);
  NFmiInfoAreaMaskGrad(const NFmiInfoAreaMaskGrad &theOther);
  NFmiAreaMask *Clone(void) const;
  NFmiInfoAreaMaskGrad &operator=(const NFmiInfoAreaMaskGrad &theMask) = delete;

  bool CalculateDivergence(void) const { return fCalculateDivergence; }
  void CalculateDivergence(bool newValue) { fCalculateDivergence = newValue; }
  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);
  double PressureValue(double thePressure, const NFmiCalculationParams &theCalculationParams);
  void InitCalcFactorVectors(void);

 protected:
  void SetDividers(void);
  const CalcFactorVector &LowerEdgeFactors(void) const { return itsLowerEdgeFactors; }
  const CalcFactorVector &UpperEdgeFactors(void) const { return itsUpperEdgeFactors; }
  const CalcFactorVector &MiddleAreaFactors(void) const { return itsMiddleAreaFactors; }

 private:
  static CalcFactorVector itsLowerEdgeFactors;
  static CalcFactorVector itsUpperEdgeFactors;
  static CalcFactorVector itsMiddleAreaFactors;
  bool fCalculateDivergence;  // tämä muuttuja määrää lasketaanko gradientti (oletus) vai
                              // divergenssi. Laskenta on muuten sama,
                              // mutta x- ja y-componentit lasketaan vain erilailla lopuksi yhteen

  static bool fCalcFactorVectorsInitialized;

};  // class NFmiInfoAreaMaskGrad

class _FMI_DLL NFmiInfoAreaMaskAdvection : public NFmiInfoAreaMaskGrad
{
 public:
  ~NFmiInfoAreaMaskAdvection(void);
  NFmiInfoAreaMaskAdvection(const NFmiCalculationCondition &theOperation,
                            Type theMaskType,
                            NFmiInfoData::Type theDataType,
                            const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                            const boost::shared_ptr<NFmiFastQueryInfo> &theInfoUwind,
                            const boost::shared_ptr<NFmiFastQueryInfo> &theInfoVwind,
                            bool thePeekAlongTudes,
                            MetFunctionDirection theMetFuncDirection,
                            BinaryOperator thePostBinaryOperator = kNoValue);
  NFmiInfoAreaMaskAdvection(const NFmiInfoAreaMaskAdvection &theOther);
  NFmiAreaMask *Clone(void) const;

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);

 private:
  void SetupWindComponents(void);

  boost::shared_ptr<NFmiFastQueryInfo> itsInfoUwind;  // Tästä katsotaan tuulen u-komponentti
  boost::shared_ptr<NFmiFastQueryInfo> itsInfoVwind;  // Tästä katsotaan tuulen v-komponentti
};

//! Tämä luokka laskee laplacen (eli nablan?).
class _FMI_DLL NFmiInfoAreaMaskLaplace : public NFmiInfoAreaMaskMetFuncBase
{
 public:
  ~NFmiInfoAreaMaskLaplace(void);
  NFmiInfoAreaMaskLaplace(const NFmiCalculationCondition &theOperation,
                          Type theMaskType,
                          NFmiInfoData::Type theDataType,
                          const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                          bool thePeekAlongTudes,
                          MetFunctionDirection theMetFuncDirection,
                          BinaryOperator thePostBinaryOperator = kNoValue);
  NFmiInfoAreaMaskLaplace(const NFmiInfoAreaMaskLaplace &theOther);
  NFmiAreaMask *Clone(void) const;
  NFmiInfoAreaMaskLaplace &operator=(const NFmiInfoAreaMaskLaplace &theMask) = delete;

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);
  void InitCalcFactorVectors(void);

 protected:
  void SetDividers(void);
  float CalcLaplaceX(const NFmiLocationCache &theLocationCachePoint);
  float CalcLaplaceY(const NFmiLocationCache &theLocationCachePoint);

  const CalcFactorVector &LowerEdgeFactors(void) const { return itsLowerEdgeFactors; }
  const CalcFactorVector &UpperEdgeFactors(void) const { return itsUpperEdgeFactors; }
  const CalcFactorVector &MiddleAreaFactors(void) const { return itsMiddleAreaFactors; }

 private:
  static CalcFactorVector itsLowerEdgeFactors;
  static CalcFactorVector itsUpperEdgeFactors;
  static CalcFactorVector itsMiddleAreaFactors;
  static bool fCalcFactorVectorsInitialized;

};  // class NFmiInfoAreaMaskLaplace

class _FMI_DLL NFmiInfoAreaMaskRotor : public NFmiInfoAreaMaskGrad
{
 public:
  ~NFmiInfoAreaMaskRotor(void);
  NFmiInfoAreaMaskRotor(const NFmiCalculationCondition &theOperation,
                        Type theMaskType,
                        NFmiInfoData::Type theDataType,
                        const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                        bool thePeekAlongTudes,
                        MetFunctionDirection theMetFuncDirection,
                        BinaryOperator thePostBinaryOperator = kNoValue);
  NFmiInfoAreaMaskRotor(const NFmiInfoAreaMaskRotor &theOther);
  NFmiAreaMask *Clone(void) const;

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);
};

// Tämän luokan avulla voidaan iterointi halutessa lopettaa 
// NFmiInfoAreaMaskVertFunc::IterateLevelsFromGroundUpward metodissa.
// Tähän voi tallettaa arvon tai indeksin, jonka em. metodia kutsuva taho voi sitten palauttaa.
// Jos tätä dataa ei haluta käyttää, niin iterointi tehdään aina alusta loppuun ja paluu arvot 
// voi palauttaa muita keinoja käyttämällä.
class VerticalIterationBreakingData
{
public:
    VerticalIterationBreakingData(bool useData = false)
        :useBreakingData(useData)
        , stopIteration(false)
        , value(kFloatMissing)
        , index(gMissingIndex)
    {}

    bool BreakIteration() const
    {
        return useBreakingData && stopIteration;
    }

    bool useBreakingData; // Käytetäänkö loopissa tätä datapakettia
    bool stopIteration;
    float value; // arvo joka halutaan loopituksen lopuksi palauttaa
    unsigned long index; // indeksi joka halutaan loppituksen lopuksi palauttaa
};

class _FMI_DLL NFmiInfoAreaMaskVertFunc : public NFmiInfoAreaMaskMetFuncBase
{
 public:
  ~NFmiInfoAreaMaskVertFunc(void);
  NFmiInfoAreaMaskVertFunc(const NFmiCalculationCondition &theOperation,
                           Type theMaskType,
                           NFmiInfoData::Type theDataType,
                           const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                           NFmiAreaMask::FunctionType thePrimaryFunc,
                           NFmiAreaMask::FunctionType theSecondaryFunc,
                           int theArgumentCount);
  NFmiInfoAreaMaskVertFunc(const NFmiInfoAreaMaskVertFunc &theOther);
  NFmiAreaMask *Clone(void) const;
  NFmiInfoAreaMaskVertFunc &operator=(const NFmiInfoAreaMaskVertFunc &theMask) = delete;
  void Initialize(void);
  void SetArguments(std::vector<float> &theArgumentVector);

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);

 protected:
  // Nämä virtuaali funktiot on toteutettava, vaikka niillä ei ole taas käyttöä täällä
  void SetDividers(void) {}
  void InitCalcFactorVectors(void) {}
  const CalcFactorVector &LowerEdgeFactors(void) const
  {
    static CalcFactorVector dummy;
    return dummy;
  }
  const CalcFactorVector &UpperEdgeFactors(void) const
  {
    static CalcFactorVector dummy;
    return dummy;
  }
  const CalcFactorVector &MiddleAreaFactors(void) const
  {
    static CalcFactorVector dummy;
    return dummy;
  }

  void FindCalculatedLeves(const NFmiLocationCache &theLocationCache);
  void SearchLevels(const NFmiLocationCache &theLocationCache);
  void SetLevelValues(void);
  float DoGetFunction(const NFmiLocationCache &theLocationCache,
                      const NFmiCalculationParams &theCalculationParams,
                      float theLevelValue);
  float DoVerticalGrad(const NFmiLocationCache &theLocationCache,
                       const NFmiCalculationParams &theCalculationParams);
  float DoFindFunction(const NFmiLocationCache &theLocationCache, const NFmiCalculationParams &theCalculationParams);
  float FindHeightForSimpleCondition(const NFmiLocationCache &theLocationCache, const NFmiCalculationParams &theCalculationParams);
  void SetLevelIndexies(float theHeightValue);
  unsigned long GetNonMissingStartLevelIndex(const NFmiLocationCache &theLocationCache, const NFmiCalculationParams &theCalculationParams);
  float GetLevelHeightValue(const NFmiLocationCache &theLocationCache);
  float DoNormalFunction(const NFmiLocationCache &theLocationCache, const NFmiCalculationParams &theCalculationParams);

  // Vertikaali levelien läpikäynti oikeassa järjestyksessä. Tehty template funktioksi
  // jotta sitä voidaan käyttää erilaisten lambda funktioiden avulla eri paikoissa.
  template<typename FunctionObject>
  void IterateLevelsFromGroundUpward(FunctionObject &functionObject, VerticalIterationBreakingData &iterationBreakingData, int usedStartLevelIndex, const NFmiCalculationParams &theCalculationParams)
  {
      for(auto levelIndex = usedStartLevelIndex;
          fReverseLevels ? levelIndex >= static_cast<int>(itsEndLevelIndex)
          : levelIndex <= static_cast<int>(itsEndLevelIndex);
          levelIndex += itsLevelIncrement)
      {
          if(itsInfo->LevelIndex(levelIndex))  // pitäisi olla aina totta
          {
              if(IgnoreSimpleConditionWhileIteratingLevels() || VertFuncSimpleconditionCheck(theCalculationParams))
                functionObject();
              if(iterationBreakingData.BreakIteration())
                  break;
          }
      }
  }

  bool VertFuncSimpleconditionCheck(const NFmiCalculationParams &theCalculationParams);
  bool IgnoreSimpleConditionWhileIteratingLevels() const;

  NFmiAreaMask::FunctionType itsPrimaryFunc;    // esim. Avg, Max, Get, Find, jne.
  NFmiAreaMask::FunctionType itsSecondaryFunc;  // esim. VertP, VertZ, jne.
  std::vector<float> itsArgumentVector;

  boost::shared_ptr<NFmiDataModifier>
      itsFunctionModifier;   // tämä luodaan itsPrimaryFunc-dataosan mukaan
  float itsStartLevelValue;  // tähän otetaan annetusta argumentti listasta aloitus korkeus (missä
                             // yksikössä onkaan)
  float itsEndLevelValue;  // tähän otetaan annetusta argumentti listasta lopetus korkeus (missä
  // yksikössä onkaan), PAITSI, jos kyse on get-funktiosta, jolloin tämä on
  // puuttuva
  unsigned long itsStartLevelIndex;  // dataa käydään läpi alkaen tästä levelistä
  unsigned long itsEndLevelIndex;    // dataa käydään läpi tähän leveliin asti

  FmiParameterName itsUsedHeightParId;
  bool fReturnHeightValue;
  int itsLevelIncrement;  // kun ollaan päätelty mihin suuntaan leveldata menee (ylös maanpinnasta
                          // vai alas avaruudesta)
  // tähän on tarkoitus laskea for-looppeja varten level incrementti (joko 1 tai -1)
  // Jos datan levelien suunta on maanpinnasta ylöspäin, on incrementti 1 ja käydään levelit
  // normaali järjestyksessä läpi.
  // Jos datan levelien suuntä on avaruudesta maanpintaa kohden, on incrementti -1 ja levelit
  // käydään käänteisessä järjestyksessä.
  bool fReverseLevels;  // Jos itsLevelIncrement on -1, tämä on true, jolloin for-loopitus tehdään
                        // käänteisessä järjestyksessä

};

class _FMI_DLL NFmiInfoAreaMaskVertConditionalFunc : public NFmiInfoAreaMaskVertFunc
{
 public:
  ~NFmiInfoAreaMaskVertConditionalFunc(void);
  NFmiInfoAreaMaskVertConditionalFunc(const NFmiCalculationCondition &theOperation,
                                      Type theMaskType,
                                      NFmiInfoData::Type theDataType,
                                      const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                                      NFmiAreaMask::FunctionType thePrimaryFunc,
                                      NFmiAreaMask::FunctionType theSecondaryFunc,
                                      int theArgumentCount);
  NFmiInfoAreaMaskVertConditionalFunc(const NFmiInfoAreaMaskVertConditionalFunc &theOther);
  NFmiAreaMask *Clone(void) const;
  NFmiInfoAreaMaskVertConditionalFunc &operator=(const NFmiInfoAreaMaskVertConditionalFunc &theMask) = delete;
  void Initialize(void);

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);

 protected:
  bool InitializeFromArguments(void);
  bool CheckProbabilityCondition(double value);
  float DoFindConditionalFunction(const NFmiLocationCache &theLocationCache, const NFmiCalculationParams &theCalculationParams);

  double itsLimit1;
  double itsLimit2;

};

class NFmiInfoAreaMaskTimeVertFunc : public NFmiInfoAreaMaskVertFunc
{
 public:
  ~NFmiInfoAreaMaskTimeVertFunc(void);
  NFmiInfoAreaMaskTimeVertFunc(const NFmiCalculationCondition &theOperation,
                               Type theMaskType,
                               NFmiInfoData::Type theDataType,
                               const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                               NFmiAreaMask::FunctionType thePrimaryFunc,
                               NFmiAreaMask::FunctionType theSecondaryFunc,
                               int theArgumentCount);
  NFmiInfoAreaMaskTimeVertFunc(const NFmiInfoAreaMaskTimeVertFunc &theOther);
  NFmiAreaMask *Clone(void) const;
  NFmiInfoAreaMaskTimeVertFunc &operator=(const NFmiInfoAreaMaskTimeVertFunc &theMask) = delete;

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);

 protected:
  void SetRangeValuesFromArguments(void);

  double itsStartTimeOffsetInHours;  // kuinka monta tuntia aikaiteroinnin alkuaika poikkeaa
                                     // current-timesta
  double itsEndTimeOffsetInHours;    // kuinka monta tuntia aikaiteroinnin loppuaika poikkeaa
                                     // current-timesta

};

class _FMI_DLL NFmiInfoAreaMaskProbFunc : public NFmiInfoAreaMask
{
 public:
  ~NFmiInfoAreaMaskProbFunc(void);
  NFmiInfoAreaMaskProbFunc(const NFmiCalculationCondition &theOperation,
                           Type theMaskType,
                           NFmiInfoData::Type theDataType,
                           const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                           NFmiAreaMask::FunctionType thePrimaryFunc,
                           NFmiAreaMask::FunctionType theSecondaryFunc,
                           int theArgumentCount);
  NFmiInfoAreaMaskProbFunc(const NFmiInfoAreaMaskProbFunc &theOther);
  NFmiAreaMask *Clone(void) const;
  NFmiInfoAreaMaskProbFunc &operator=(const NFmiInfoAreaMaskProbFunc &theMask) = delete;
  void SetArguments(std::vector<float> &theArgumentVector);

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);
  static NFmiMetTime CalcTimeLoopLimits(boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                                        const NFmiCalculationParams &theCalculationParams,
                                        double theStartTimeOffsetInHours,
                                        double theEndTimeOffsetInHours,
                                        unsigned long *theStartTimeIndexOut,
                                        unsigned long *theEndTimeIndexOut,
                                        bool *doSpecialCalculation,
                                        bool allowInterpolation);
  static bool CheckTimeIndicesForLoopBreak(unsigned long theStartTimeIndex, unsigned long theEndTimeIndex);

 protected:
  void InitializeFromArguments(void);
  bool CheckProbabilityCondition(double value);
  bool IsCalculationPointInsideCircle(const NFmiLocation &theCalculationPointLocation, int theOffsetX, int theOffsetY);
  void DoSubgridCalculations(const NFmiLocation &theCalculationPointLocation, int leftSubGridOffset, int rightSubGridOffset, int bottomSubGridOffset, int topSubGridOffset, const NFmiMetTime &theInterpolationTime, bool useInterpolatedTime, NFmiCalculationParams &theSimpleConditionCalculationPointParams);

  // Seuraavat virtuaali funktiot liittyvät Simplecondition laskuihin ja ne on tarkoitus siirtää myöhemmin 
  // ylemmäs perintäpuussa esim. juureen eli NFmiAreaMask interface luokkaan asti.
  void DoIntegrationCalculations(float value) override;
  void InitializeIntegrationValues() override;
  float CalculationPointValue(int theOffsetX, int theOffsetY, const NFmiMetTime &theInterpolationTime, bool useInterpolatedTime) override;

  // Esim. Over, Under, Between, Equal
  NFmiAreaMask::FunctionType itsPrimaryFunc;    
  // esim. ProbRect, ProbCircle
  NFmiAreaMask::FunctionType itsSecondaryFunc;  
  
  // tähän lasketaan lennossa laskuissa tarvittavat argumentit (alueen säde ja raja(t))
  std::vector<float> itsArgumentVector;         

  // minkä säteisen alueen halutaan käyvän läpi originaali datassa
  double itsSearchRangeInKM;  
  double itsLimit1;
  double itsLimit2;
  // kuinka monta tuntia aikaiteroinnin alkuaika poikkeaa current-timesta
  double itsStartTimeOffsetInHours;  
  
  // kuinka monta tuntia aikaiteroinnin loppuaika poikkeaa current-timesta
  double itsEndTimeOffsetInHours;    
  // käydään läpi ali hila, joka on tämän kokoinen x-suunnassa (min 2)
  int itsGridPointRectSizeX;  
  // käydään läpi ali hila, joka on tämän kokoinen y-suunnassa (min 2)
  int itsGridPointRectSizeY;  
  // Kaikki hilapisteet, missä todennäköisyys ehto piti paikkaansa (tämä otetaan talteen Value -metodissa,
  // jotta NFmiInfoAreaMaskOccurrance -lapsiluokka voi käyttää tämän luokan laskuja hyväkseen hiladatan tapauksessa)
  int itsConditionFullfilledGridPointCount;  
  // Kaikkien laskuissaa läpikäytyjen hilapisteiden määrä, aika- ja area-loopeissa yhteensä,
  // joissa oli ei-puuttuva arvo.
  int itsTotalCalculatedGridPoints;

};

class _FMI_DLL NFmiInfoTimeIntegrator : public NFmiInfoAreaMaskMetFuncBase
{
 public:
  ~NFmiInfoTimeIntegrator(void);
  NFmiInfoTimeIntegrator(const NFmiCalculationCondition &theOperation,
                         Type theMaskType,
                         NFmiInfoData::Type theDataType,
                         const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                         NFmiAreaMask::FunctionType theIntegrationFunc,
                         int theStartTimeOffset,
                         int theEndTimeOffset);
  NFmiInfoTimeIntegrator(const NFmiInfoTimeIntegrator &theOther);
  NFmiAreaMask *Clone(void) const;

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);

 protected:
  // Nämä virtuaali funktiot on toteutettava, vaikka niillä ei ole taas käyttöä täällä
  void SetDividers(void) {}
  void InitCalcFactorVectors(void) {}
  const CalcFactorVector &LowerEdgeFactors(void) const
  {
    static CalcFactorVector dummy;
    return dummy;
  }
  const CalcFactorVector &UpperEdgeFactors(void) const
  {
    static CalcFactorVector dummy;
    return dummy;
  }
  const CalcFactorVector &MiddleAreaFactors(void) const
  {
    static CalcFactorVector dummy;
    return dummy;
  }

  NFmiAreaMask::FunctionType itsIntegrationFunc;  // esim. Avg, Max, Min, Sum, jne.

  boost::shared_ptr<NFmiDataModifier>
      itsFunctionModifier;  // tämä luodaan itsIntegrationFunc-dataosan mukaan
  int itsStartTimeOffset;   // dataa käydään läpi alkaen tästä suhteellisesta ajasta
  int itsEndTimeOffset;     // dataa käydään läpi päätyen tähän suhteelliseen aikaan
};

// Luokka tekee yksinkertaisen laatikko integraation halutulla funktiolla.
// Tarkoittaen esim. MIN(T -1 -1 1 1) tyyppisiä funktioita, joissa käydään läpi
// originaali dataa vakio hilalaatikon puitteissa.
class _FMI_DLL NFmiInfoRectAreaIntegrator : public NFmiInfoAreaMaskMetFuncBase
{
 public:
  ~NFmiInfoRectAreaIntegrator(void);
  NFmiInfoRectAreaIntegrator(const NFmiCalculationCondition &theOperation,
                             Type theMaskType,
                             NFmiInfoData::Type theDataType,
                             const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                             NFmiAreaMask::FunctionType theIntegrationFunc,
                             int theStartXOffset,
                             int theEndXOffset,
                             int theStartYOffset,
                             int theEndYOffset);
  NFmiInfoRectAreaIntegrator(const NFmiInfoRectAreaIntegrator &theOther);
  NFmiAreaMask *Clone(void) const;

  // tätä kaytetaan smarttool-modifierin yhteydessä
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);

 protected:
  // Nämä virtuaali funktiot on toteutettava, vaikka niillä ei ole taas käyttöä täällä
  void SetDividers(void) {}
  void InitCalcFactorVectors(void) {}
  const CalcFactorVector &LowerEdgeFactors(void) const
  {
    static CalcFactorVector dummy;
    return dummy;
  }
  const CalcFactorVector &UpperEdgeFactors(void) const
  {
    static CalcFactorVector dummy;
    return dummy;
  }
  const CalcFactorVector &MiddleAreaFactors(void) const
  {
    static CalcFactorVector dummy;
    return dummy;
  }

  NFmiAreaMask::FunctionType itsIntegrationFunc;  // esim. Avg, Max, Min, Sum, jne.

  boost::shared_ptr<NFmiDataModifier>
      itsFunctionModifier;  // tämä luodaan itsIntegrationFunc-dataosan mukaan
  int itsStartXOffset;
  int itsEndXOffset;
  int itsStartYOffset;
  int itsEndYOffset;
};

// Luokka tekee alueellisia integraatio laskuja halutulla funktiolla.
// Laskenta-alue on halutun säteinen [km] ympyrä kulloisen laskenta pisteen
// ympärillä datan originaali hilassa.
class _FMI_DLL NFmiInfoAreaIntegrationFunc : public NFmiInfoAreaMaskProbFunc
{
public:
    ~NFmiInfoAreaIntegrationFunc(void);
    NFmiInfoAreaIntegrationFunc(const NFmiCalculationCondition &theOperation,
        Type theMaskType,
        NFmiInfoData::Type theDataType,
        const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
        NFmiAreaMask::FunctionType thePrimaryFunc,
        NFmiAreaMask::FunctionType theSecondaryFunc,
        int theArgumentCount);
    NFmiInfoAreaIntegrationFunc(const NFmiInfoAreaIntegrationFunc &theOther);
    NFmiAreaMask *Clone(void) const override;
    NFmiInfoAreaIntegrationFunc &operator=(const NFmiInfoAreaIntegrationFunc &theMask) = delete;

    // tätä kaytetaan smarttool-modifierin yhteydessä
    double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways) override;

protected:
    void DoIntegrationCalculations(float value) override;
    void InitializeIntegrationValues() override;

    // Esim. Avg, Max, Min, Sum, Med, jne.
    NFmiAreaMask::FunctionType itsIntegrationFunc;  
    // Tämä luodaan itsIntegrationFunc-dataosan mukaan
    boost::shared_ptr<NFmiDataModifier> itsFunctionModifier;  

};

// Luokka joka laskee todennäköisyyksiä datan omassa hilassa ynmpyrä alueen sisällä olevien
// pisteiden kanssa, MUTTA niin että laskuissa käytetään simple-condition laskuja.
class _FMI_DLL NFmiInfoAreaMaskAreaProbFunc : public NFmiInfoAreaMaskProbFunc
{
public:
    ~NFmiInfoAreaMaskAreaProbFunc(void);
    NFmiInfoAreaMaskAreaProbFunc(const NFmiCalculationCondition &theOperation,
        Type theMaskType,
        NFmiInfoData::Type theDataType,
        const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
        NFmiAreaMask::FunctionType thePrimaryFunc,
        NFmiAreaMask::FunctionType theSecondaryFunc,
        int theArgumentCount);
    NFmiInfoAreaMaskAreaProbFunc(const NFmiInfoAreaIntegrationFunc &theOther);
    NFmiAreaMask *Clone(void) const override;
    NFmiInfoAreaMaskAreaProbFunc &operator=(const NFmiInfoAreaMaskAreaProbFunc &theMask) = delete;

protected:
    void DoIntegrationCalculations(float value) override;
    float CalculationPointValue(int theOffsetX, int theOffsetY, const NFmiMetTime &theInterpolationTime, bool useInterpolatedTime) override;

};

// ======================================================================
