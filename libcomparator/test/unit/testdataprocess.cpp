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
