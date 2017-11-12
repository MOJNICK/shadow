#include <opencv2/ts/ts.hpp>
#include "dataprocess.hpp"

uint const shuffleNumber = 1;

class DataProcessTestMethods : public cvtest::BaseTest
{
public:
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
      DataProcess::concatenate_HV( inputVec );
      compareVecIndexTransition(expectedVec, inputVec);
    }
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
  }
};


#ifdef TEST_PRIVATE_PART
  class ColorBalancePrivateTestMethods : public cvtest::BaseTest
  {
  public:
  protected:
    void compareColorBalance( std::vector< double > const & expected, double const * const actual )
    {
      for( int i = 0; i < channels; ++i )
      {
        ASSERT_DOUBLE_EQ( expected[i], actual[i] );
      }
    }
  };
  
  class ColorBalanceElementBalance : public ColorBalancePrivateTestMethods
  {
  public:
  protected:
    void run(int)
    {
      cv::Mat_<TYPE> mat( 3, 3 * channels, 100 );

      TYPE acceptanceLevel = 10;
      uint distance = 1;
      ColorBalance colorBalance( mat, acceptanceLevel, distance );

      IndexTransition indexTransition{ 1, 3, rToL };
      mat( 1, 3 ) = 10; mat( 1, 4 ) = 25; mat( 1, 5 ) = 50;
      mat( 1, 6 ) = 50; mat( 1, 7 ) = 50; mat( 1, 8 ) = 50;
      std::vector< double > expectedBalance = {5.0, 2.0, 1.0};
      colorBalance.element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.colorBalance );

      colorBalance.clear_balance();
      indexTransition = IndexTransition{ 1, 6, lToR };
      mat( 1, 3 ) = 10; mat( 1, 4 ) = 25; mat( 1, 5 ) = 50;
      mat( 1, 6 ) = 50; mat( 1, 7 ) = 50; mat( 1, 8 ) = 50;
      expectedBalance = { 1/5.0, 1/2.0, 1/1.0};
      colorBalance.element_balance( indexTransition );
      compareColorBalance( expectedBalance, colorBalance.colorBalance );
    }
  };
  TEST(ColorBalancePrivateSuite, ColorBalanceElementBalance)
  {
    ColorBalanceElementBalance colorBalanceElementBalance;
    colorBalanceElementBalance.safe_run();
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