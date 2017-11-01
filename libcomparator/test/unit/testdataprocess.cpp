#include <opencv2/ts/ts.hpp>
#include "dataprocess.hpp"

class DataProcessTestMethods : public cvtest::BaseTest
{
public:
  DataProcessTestMethods(){}
protected:
  void compareVecIndexTransition(std::vector<IndexTransition> expectedVec, std::vector<IndexTransition> actualVec)
  {
    ASSERT_EQ(expectedVec.size(), actualVec.size());
    for(int i = 0; i < expectedVec.size(); ++i)
    {
      ASSERT_EQ(expectedVec[i].index, actualVec[i].index);
      ASSERT_EQ(expectedVec[i].transition, actualVec[i].transition);
    }
  }
};

class ConcatenateHV : public DataProcessTestMethods
{
public:
    ConcatenateHV() : shuffleNumber{100}{}
    ConcatenateHV(int shuffleNumber) : shuffleNumber{shuffleNumber}{}
protected:
  void run(int)
  { 
    DataProcess dp;     
    
    std::vector<IndexTransition> expectedVec;
    std::vector<IndexTransition> inputVec;

    expectedVec = { IndexTransition{12, all} };
    inputVec = { IndexTransition{12, lToR}, IndexTransition{12, rToL},
                 IndexTransition{12, upToDw}, IndexTransition{12, dwToUp}};
    shuffleAndTest(expectedVec, inputVec);

    expectedVec = { IndexTransition{12, biRUp}, IndexTransition{13, biLDw} };
    inputVec = { IndexTransition{13, lToR}, IndexTransition{12, rToL},
                 IndexTransition{12, upToDw}, IndexTransition{13, dwToUp}};
    shuffleAndTest(expectedVec, inputVec);
    
    expectedVec = { IndexTransition{12, biRUp}, IndexTransition{13, dwToUp}, IndexTransition{114, lToR} };
    inputVec = { IndexTransition{114, lToR}, IndexTransition{12, rToL},
                 IndexTransition{12, upToDw}, IndexTransition{13, dwToUp}};
    shuffleAndTest(expectedVec, inputVec);
    
    expectedVec = { IndexTransition{11, lToR}, IndexTransition{12, biRUp}, IndexTransition{13, dwToUp} };
    inputVec = { IndexTransition{11, lToR}, IndexTransition{12, rToL},
                 IndexTransition{12, upToDw}, IndexTransition{13, dwToUp}};
    shuffleAndTest(expectedVec, inputVec);

    expectedVec = { IndexTransition{11, lToR}, IndexTransition{12, rToL}, IndexTransition{13, dwToUp}, IndexTransition{14, upToDw} };
    inputVec = { IndexTransition{11, lToR}, IndexTransition{12, rToL},
                 IndexTransition{14, upToDw}, IndexTransition{13, dwToUp}};
    shuffleAndTest(expectedVec, inputVec);
  }
private:
  int shuffleNumber;
  void shuffleAndTest(std::vector<IndexTransition>& expectedVec, std::vector<IndexTransition>& inputVec)
  {
    for(int i = 0; i < shuffleNumber; ++i)
    { 
      std::random_shuffle (inputVec.begin(), inputVec.end());
      DataProcess::concatenate_HV(inputVec);
      compareVecIndexTransition(expectedVec, inputVec);
    }
  }
};
TEST(ComparatorLibDataProcessSuite, ConcatenateHV)
{
  ConcatenateHV ConcatenateHV;
  ConcatenateHV.safe_run();
}
