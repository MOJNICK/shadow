#include <opencv2/ts/ts.hpp>
#include "dataprocess.hpp"

uint const shuffleNumber = 100;

class DataProcessTestMethods : public cvtest::BaseTest
{
public:
  DataProcessTestMethods(): shuffleNumber{::shuffleNumber}{}
  DataProcessTestMethods( int shuffleNumber ): shuffleNumber{shuffleNumber}{}
protected:
  int shuffleNumber;
  
  void compareVecIndexTransition(std::vector<IndexTransition> expectedVec, std::vector<IndexTransition> actualVec)
  {
    ASSERT_EQ(expectedVec.size(), actualVec.size());
    for(int i = 0; i < expectedVec.size(); ++i)
    {
      ASSERT_TRUE( expectedVec[i].same_position( actualVec[i] ) );
      ASSERT_EQ( expectedVec[i].transition, actualVec[i].transition );
    }
  }
  
  void shuffleAndTest(std::vector<IndexTransition>& expectedVec, std::vector<IndexTransition>& inputVec)
  {
    for(int i = 0; i < shuffleNumber; ++i)
    { 
      std::random_shuffle( inputVec.begin(), inputVec.end() );
      IterateProcess<TYPE>::concatenate_HV( inputVec );
      compareVecIndexTransition(expectedVec, inputVec);
    }
  }

  std::vector<IndexTransition> transitionVec_to_indexTransitionVec( std::vector<Transition> trans)
  {
    std::vector<IndexTransition> result; 
    for_each(trans.begin(), trans.end(), [&result](auto tr){
      result.push_back(IndexTransition{0, 0, tr});
    });
    return result;
  }
};

class ConcatenateHV : public DataProcessTestMethods
{
public:
    ConcatenateHV() : DataProcessTestMethods( ::shuffleNumber ){}
    ConcatenateHV( int shuffleNumber ) : DataProcessTestMethods( shuffleNumber ){}
protected:
  void run(int)
  { 
    DataProcess dp;     
    
    std::vector<IndexTransition> expectedVec;
    std::vector<IndexTransition> inputVec;

    expectedVec = { IndexTransition{ 1, 3, all } };
    inputVec = { IndexTransition{ 1, 3, lToR }, IndexTransition{ 1, 3, rToL },
                 IndexTransition{ 1, 3, upToDw}, IndexTransition{ 1, 3, dwToUp }};
    shuffleAndTest(expectedVec, inputVec);

    expectedVec = { IndexTransition{ 1, 3, biRUp }, IndexTransition{ 1, 4, biLDw } };
    inputVec = { IndexTransition{ 1, 4, lToR }, IndexTransition{ 1, 3, rToL },
                 IndexTransition{ 1, 3, upToDw }, IndexTransition{ 1, 4, dwToUp }};
    shuffleAndTest(expectedVec, inputVec);
    
    expectedVec = { IndexTransition{ 1, 3, biRUp}, IndexTransition{ 1, 4, dwToUp }, IndexTransition{ 2, 30, lToR} };
    inputVec = { IndexTransition{ 2, 30, lToR }, IndexTransition{ 1, 3, rToL },
                 IndexTransition{ 1, 3, upToDw}, IndexTransition{ 1, 4, dwToUp }};
    shuffleAndTest(expectedVec, inputVec);
    
    expectedVec = { IndexTransition{ 1, 2, lToR}, IndexTransition{ 1, 3, biRUp}, IndexTransition{ 1, 4, dwToUp} };
    inputVec = { IndexTransition{ 1, 2, lToR}, IndexTransition{ 1, 3, rToL},
                 IndexTransition{ 1, 3, upToDw}, IndexTransition{ 1, 4, dwToUp}};
    shuffleAndTest(expectedVec, inputVec);

    expectedVec = { IndexTransition{ 1, 2, lToR}, IndexTransition{ 1, 3, rToL}, IndexTransition{ 1, 4, dwToUp}, IndexTransition{ 1, 5, upToDw} };
    inputVec = { IndexTransition{ 1, 2, lToR}, IndexTransition{ 1, 3, rToL},
                 IndexTransition{ 1, 5, upToDw}, IndexTransition{ 1, 4, dwToUp}};
    shuffleAndTest(expectedVec, inputVec);
  }
};
TEST(ComparatorLibDataProcessSuite, ConcatenateHV)
{
  ConcatenateHV ConcatenateHV;
  ConcatenateHV.safe_run();
}

class RemoveNoise : public DataProcessTestMethods
{
public:
    RemoveNoise(){}
protected:
  void run(int)
  { 
    DataProcess dp;
    std::vector<uint> expectedSize{1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0};
    for(uint i = 0; i < 0x10; ++i )
    {
      Transition tr = (Transition)( i << directionBit );

      std::vector<IndexTransition> testValue{ { 0, 0, tr } };
      IterateProcess<TYPE>::remove_noise_matches( testValue );
      EXPECT_EQ( expectedSize[ i ], testValue.size() );
    }

    std::vector<Transition> transVector{ (Transition)(0), upToDw, lToR, biLUp, dwToUp, biLDw, rToL, biRUp, biRDw};
    std::vector<IndexTransition> expectedVector = transitionVec_to_indexTransitionVec( transVector );
    std::vector<IndexTransition> testVector;
    for(uint i = 0; i < 0x10; ++i )
    {
      Transition tr = (Transition)( i << directionBit );
      testVector.push_back( IndexTransition{ 0, 0, tr } );
    }
    IterateProcess<TYPE>::remove_noise_matches( testVector );
    EXPECT_EQ( expectedVector, testVector );

  }
};
TEST(ComparatorLibDataProcessSuite, RemoveNoise)
{
  RemoveNoise RemoveNoise;
  RemoveNoise.safe_run();
}

class ColorStructsaturation : public cvtest::BaseTest
{
public:
protected:
  void run(int)
  {
    //ColorStruct::baseLevel = 0;
    ColorStruct cs{ 0, 0, 0};
    ASSERT_DOUBLE_EQ( 0.0, cs.saturation() );

    cs = { 10, 10, 10};
    ASSERT_DOUBLE_EQ( 0.0, cs.saturation() );

    cs = { 20, 0, 0};
    ASSERT_DOUBLE_EQ( 1, cs.saturation() );

    cs = { 0, 13, 0};
    ASSERT_DOUBLE_EQ( 1, cs.saturation() );

    cs = { 0, 0, 10};
    ASSERT_DOUBLE_EQ( 1, cs.saturation() );

    cs = { 0, 10, 10};
    ASSERT_DOUBLE_EQ( 1, cs.saturation() );

    cs = { 5, 10, 10};
    ASSERT_DOUBLE_EQ( 0.5, cs.saturation() );
  }
};
TEST(ColorStructSuite, ColorStructsaturation)
{
  ColorStructsaturation colorStructsaturation;
  colorStructsaturation.safe_run();
}

class ColorStructHUE : public cvtest::BaseTest
{
public:
protected:
  void run(int)
  {
//    ColorStruct::baseLevel = 0;
    ColorStruct cs{ 0, 0, 0};
    ASSERT_DOUBLE_EQ( 0.0, cs.HUE() );

    cs = { 10, 10, 10};
    ASSERT_DOUBLE_EQ( 0.0, cs.HUE() );

    cs = { 20, 0, 0};
    ASSERT_DOUBLE_EQ( -120.0, cs.HUE() );

    cs = { 0, 13, 0};
    ASSERT_DOUBLE_EQ( 120.0, cs.HUE() );

    cs = { 0, 0, 10};
    ASSERT_DOUBLE_EQ( 0.0, cs.HUE() );

    cs = { 0, 10, 10};
    ASSERT_DOUBLE_EQ( 60, cs.HUE() );

    cs = { 5, 10, 10};
    ASSERT_DOUBLE_EQ( 60, cs.HUE() );
  }
};
TEST(ColorStructSuite, ColorStructHUE)
{
  ColorStructHUE colorStructHUE;
  colorStructHUE.safe_run();
}

class ColorBalanceTestMethods : public cvtest::BaseTest
{
public:
protected:
  void compareColorBalance( ColorStruct const & expected, ColorStruct const & actual )
  {
    for( int i = 0; i < channels; ++i )
    {
      ASSERT_DOUBLE_EQ( expected.get_color(i), actual.get_color(i) );
    }
  }
};

class ColorBalanceBalance : public cvtest::BaseTest
{
public:
protected:
  void run(int)
  {
    cv::Mat_<TYPE> mat(3, 3 * channels, 100);
    mat(1,3)=0; mat(1,4)=0; mat(1,5)=0;
    TYPE acceptanceLevel = 1;
    uint distance = 1;
    ColorBalance colorBalance(cv::Mat(), acceptanceLevel, distance);
    //balance TBD
  }
};
TEST(ColorBalanceSuite, ColorBalanceBalance)
{
  ColorBalanceBalance colorBalanceBalance;
  colorBalanceBalance.safe_run();
}

#ifdef TEST_PRIVATE_PART
  
  class ColorBalanceElementBalance : public ColorBalanceTestMethods
  {
  public:
  protected:
    void run(int)
    {
      cv::Mat_<TYPE> mat( 3, 3 * channels, (TYPE)0 );

      TYPE acceptanceLevel = 10;
      uint distance = 1;
      ColorBalance colorBalance( mat, acceptanceLevel, distance );

      colorBalance.clear_balance();
      IndexTransition indexTransition{ 1, 1, rToL };
      mat( 1, 3 ) = 9; mat( 1, 4 ) = 25; mat( 1, 5 ) = 50;//acceptanceLevel
      mat( 1, 6 ) = 50; mat( 1, 7 ) = 50; mat( 1, 8 ) = 50;
      ColorStruct expectedBalance = {.0, .0, .0};
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance() );

      colorBalance.clear_balance();
      indexTransition = IndexTransition{ 1, 1, rToL };
      mat( 1, 3 ) = 10; mat( 1, 4 ) = 25; mat( 1, 5 ) = 50;
      mat( 1, 6 ) = 50; mat( 1, 7 ) = 50; mat( 1, 8 ) = 50;
      expectedBalance = {5.0, 2.0, 1.0};
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance() );

      colorBalance.clear_balance();
      indexTransition = IndexTransition{ 1, 2, lToR };
      mat( 1, 3 ) = 10; mat( 1, 4 ) = 25; mat( 1, 5 ) = 50;
      mat( 1, 6 ) = 50; mat( 1, 7 ) = 50; mat( 1, 8 ) = 50;
      expectedBalance = { 1/5.0, 1/2.0, 1/1.0};
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance() );

      colorBalance.clear_balance();
      indexTransition = IndexTransition{ 0, 0, biRDw };
      mat( 0, 0 ) = 30; mat( 0, 1 ) = 10; mat( 0, 2 ) = 20;
      mat( 1, 3 ) = 120; mat( 1, 4 ) = 120; mat( 1, 5 ) = 120;
      expectedBalance = { 4.0, 12.0, 6.0 };
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance() );

      colorBalance.clear_balance();
      indexTransition = IndexTransition{ 1, 1, biLUp };
      mat( 0, 0 ) = 30; mat( 0, 1 ) = 10; mat( 0, 2 ) = 20;
      mat( 1, 3 ) = 120; mat( 1, 4 ) = 120; mat( 1, 5 ) = 120;
      expectedBalance = { 1/4.0, 1/12.0, 1/6.0 };
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance() );

      indexTransition = IndexTransition{ 3, 2, biLUp };
      mat( 3, 6 ) = 30; mat( 3, 7 ) = 10; mat( 3, 8 ) = 20;
      mat( 2, 3 ) = 120; mat( 2, 4 ) = 120; mat( 2, 5 ) = 120;
      colorBalance.push_element_balance( indexTransition );
      expectedBalance = { 1/4.0, 1/12.0, 1/6.0 };
      compareColorBalance( expectedBalance, colorBalance.getColorBalance( 0 ) );
      expectedBalance = { 4.0, 12.0, 6.0 };
      compareColorBalance( expectedBalance, colorBalance.getColorBalance( 1 ) );

      indexTransition = IndexTransition{ 0, 0, biRDw };
      mat( 0, 0 ) = 30; mat( 0, 1 ) = 10; mat( 0, 2 ) = 20;
      mat( 1, 3 ) = 120; mat( 1, 4 ) = 120; mat( 1, 5 ) = 120;
      colorBalance.push_element_balance( indexTransition );
      expectedBalance = { 1/4.0, 1/12.0, 1/6.0 };
      compareColorBalance( expectedBalance, colorBalance.getColorBalance( 0 ) );
      expectedBalance = { 4.0, 12.0, 6.0 };
      compareColorBalance( expectedBalance, colorBalance.getColorBalance( 1 ) );
      expectedBalance = { 4.0, 12.0, 6.0};
      compareColorBalance( expectedBalance, colorBalance.getColorBalance( 2 ) );

      colorBalance.clear_balance();
      indexTransition = IndexTransition{ 0, 0, biRDw };
      mat( 0, 0 ) = 30; mat( 0, 1 ) = 10; mat( 0, 2 ) = 20;
      mat( 1, 3 ) = 120; mat( 1, 4 ) = 120; mat( 1, 5 ) = 120;
      expectedBalance = { 4.0, 12.0, 6.0};
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance( 0 ) );
    }
  };
  TEST(ColorBalancePrivateSuite, ColorBalanceElementBalance)
  {
    ColorBalanceElementBalance colorBalanceElementBalance;
    colorBalanceElementBalance.safe_run();
  }

  class ColorBalanceElementBalanceCorner : public ColorBalanceTestMethods
  {
  public:
  protected:
    void run(int)
    {
      cv::Mat_<TYPE> mat( 3, 3 * channels, (TYPE)0 );

      TYPE acceptanceLevel = 10;
      uint distance = 1;
      ColorBalance colorBalance( mat, acceptanceLevel, distance );

      colorBalance.clear_balance();
      IndexTransition indexTransition{ 0, 1, lToR };
      mat( 0, 3 ) = 10; mat( 0, 4 ) = 20; mat( 0, 5 ) = 30;
      mat( 0, 0 ) = 120; mat( 0, 1 ) = 120; mat( 0, 2 ) = 120;
      ColorStruct expectedBalance = { 12.0, 6.0, 4.0 };
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance() );

/*
      colorBalance.clear_balance();
      indexTransition = IndexTransition{ 0, 2, lToR };//slightly out of range
      expectedBalance = { .0, .0, .0 };
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance() );//expected no balance modification
*/
      colorBalance.clear_balance();
      indexTransition = IndexTransition{ 0, 2, rToL };//out of range
      expectedBalance = { .0, .0, .0 };
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance() );//expected no balance modification
/*
      colorBalance.clear_balance();
      indexTransition = IndexTransition{ 0, 5, rToL };//out of range, but missed check as first pixel of subpixel inside
      mat( 0, 5 ) = 10; mat( 0, 6 ) = 20; mat( 0, 7 ) = 30;
      mat( 0, 8 ) = 120; mat( 1, 0 ) = 120; mat( 1, 1 ) = 120;
      expectedBalance = { 12.0, 6.0, 4.0 };
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance() );
*/
      colorBalance.clear_balance();
      indexTransition = IndexTransition{ 0, 0, biRDw };
      mat( 0, 0 ) = 30; mat( 0, 1 ) = 10; mat( 0, 2 ) = 20;
      mat( 1, 3 ) = 120; mat( 1, 4 ) = 9; mat( 1, 5 ) = 120;//9 < acceptance
      expectedBalance = { .0, .0, .0};
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance() );

      colorBalance.clear_balance();
      indexTransition = IndexTransition{ 0, 0, biRDw };
      mat( 0, 0 ) = 30; mat( 0, 1 ) = 9; mat( 0, 2 ) = 20;//9 < acceptance
      mat( 1, 3 ) = 120; mat( 1, 4 ) = 120; mat( 1, 5 ) = 120;
      expectedBalance = { .0, .0, .0};
      colorBalance.push_element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.getColorBalance() );
    }
  };
  TEST(ColorBalancePrivateSuite, ColorBalanceElementBalanceCorner)
  {
    ColorBalanceElementBalanceCorner colorBalanceElementBalanceCorner;
    colorBalanceElementBalanceCorner.safe_run();
  }

  class ColorBalanceIsValid : public cvtest::BaseTest
  {
  public:
    ColorBalanceIsValid(){}
  protected:
    void run(int)
    {
      Transition transition;

      transition = Transition::lToR;
      ASSERT_TRUE( ColorBalance::is_valid( transition ) );

      transition = Transition::rToL;
      ASSERT_TRUE( ColorBalance::is_valid( transition ) );

      transition = Transition::biLDw;
      ASSERT_TRUE( ColorBalance::is_valid( transition ) );

      transition = Transition::biRUp;
      ASSERT_TRUE( ColorBalance::is_valid( transition ) );

      transition = Transition::biLR;
      ASSERT_FALSE( ColorBalance::is_valid( transition ) );

      transition = Transition::all;
      ASSERT_FALSE( ColorBalance::is_valid( transition ) );

      transition = Transition::back;
      ASSERT_FALSE( ColorBalance::is_valid( transition ) );

      transition = Transition::fwd;
      ASSERT_FALSE( ColorBalance::is_valid( transition ) );

      transition = Transition::back | Transition::upToDw;
      ASSERT_FALSE( ColorBalance::is_valid( transition ) );

      transition = Transition::biRDw | Transition::upToDw;
      ASSERT_FALSE( ColorBalance::is_valid( transition ) );
    }
  };
  TEST(ColorBalancePrivateSuite, ColorBalanceIsValid)
  {
    ColorBalanceIsValid colorBalanceIsValid;
    colorBalanceIsValid.safe_run();
  }
#endif