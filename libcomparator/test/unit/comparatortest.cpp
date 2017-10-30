#include <opencv2/ts/ts.hpp>
#include <libcomparator.hpp>

class IterateTestMethods : public cvtest::BaseTest
{
public:
  IterateTestMethods(){}
protected:
  void compareIndexTransition(IndexTransition& expected, IndexTransition& actual)
  {
    ASSERT_EQ(expected.index, actual.index);
    ASSERT_EQ(expected.transition, actual.transition);
  }

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

class ConcatenateHV : public IterateTestMethods
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
TEST(ComparatorLibSuite, ConcatenateHV)
{
  ConcatenateHV ConcatenateHV;
  ConcatenateHV.safe_run();
}


class IterateHV : public IterateTestMethods
{
public:
  IterateHV(){}
protected:
  void run(int)
  {
    cv::Mat_<TYPE> mat(3, 3 * channels, 100);
    // cv::Vec<TYPE, channels> subPix;
    // subPix = static_cast<cv::Vec<TYPE, channels>>(cv::Matx<TYPE, channels, 1>::zeros());
    // mat.at<cv::Vec<TYPE, channels>>(cv::Point(3,3)) = subPix;
    mat(1,3)=0; mat(1,4)=0; mat(1,5)=0;
    double balance[] = {1.0, 1.0, 1.0};
    double lightThreshold = 2.0;
    double colorThreshold = 1.0;

    IterateProcess<TYPE> iterateProcess(mat, lightThreshold, colorThreshold, balance);

    std::vector<IndexTransition> result = iterateProcess.iterate_HV();

    ASSERT_EQ(result.size(), 4);
    auto expected = IndexTransition{12, lToR};
    compareIndexTransition(result[0], expected);
    expected = IndexTransition{12, rToL};
    compareIndexTransition(result[1], expected);
    expected = IndexTransition{12, upToDw};
    compareIndexTransition(result[2], expected);
    expected = IndexTransition{12, dwToUp};
    compareIndexTransition(result[3], expected);
  }
};
TEST(ComparatorLibSuite, IterateHV)
{
  IterateHV IterateHV;
  IterateHV.safe_run();
}


#ifdef TEST_PRIVATE_PART
  class IterateH : public IterateTestMethods
  {
  public:
    IterateH(){}
  protected:
    void run(int)
    {
      cv::Mat_<TYPE> mat(3, 3 * channels, 100);
      // cv::Vec<TYPE, channels> subPix;
      // subPix = static_cast<cv::Vec<TYPE, channels>>(cv::Matx<TYPE, channels, 1>::zeros());
      // mat.at<cv::Vec<TYPE, channels>>(cv::Point(3,3)) = subPix;
      mat(1,3)=0; mat(1,4)=0; mat(1,5)=0;
      double balance[] = {1.0, 1.0, 1.0};
      double lightThreshold = 2.0;
      double colorThreshold = 1.0;

      IterateProcess<TYPE> iterateProcess(mat, lightThreshold, colorThreshold, balance);

      std::vector<IndexTransition> result = iterateProcess.iterate_H();

      ASSERT_EQ(result.size(), 2);
      auto expected = IndexTransition{12, lToR};
      compareIndexTransition(expected, result[0]);
      expected = IndexTransition{12, rToL};
      compareIndexTransition(expected, result[1]);
    }
  };
  TEST(ComparatorPrivateLibSuite, IterateH)
  {
    IterateH iterateH;
    iterateH.safe_run();
  }


  class IterateV : public IterateTestMethods
  {
  public:
    IterateV(){}
  protected:
    void run(int)
    {
      cv::Mat_<TYPE> mat(3, 3 * channels, 100);
      // cv::Vec<TYPE, channels> subPix;
      // subPix = static_cast<cv::Vec<TYPE, channels>>(cv::Matx<TYPE, channels, 1>::zeros());
      // mat.at<cv::Vec<TYPE, channels>>(cv::Point(3,3)) = subPix;
      mat(1,3)=0; mat(1,4)=0; mat(1,5)=0;
      double balance[] = {1.0, 1.0, 1.0};
      double lightThreshold = 4.0;
      double colorThreshold = 1.0;

      IterateProcess<TYPE> iterateProcess(mat, lightThreshold, colorThreshold, balance);

      std::vector<IndexTransition> result = iterateProcess.iterate_V();

      ASSERT_EQ(result.size(), 2);
      auto expected = IndexTransition{12, upToDw};
      compareIndexTransition(expected, result[0]);
      expected = IndexTransition{12, dwToUp};
      compareIndexTransition(expected, result[1]);
    }
  };
  TEST(ComparatorPrivateLibSuite, IterateV)
  {
    IterateV IterateV;
    IterateV.safe_run();
  }
#endif


class Fclassifier : public cvtest::BaseTest
{
public:
  Fclassifier(){}
protected:
  void run(int)
  {
    double balance[] = {1.0, 1.0, 1.0};
    double lightThreshold = 2.0;
    double colorThreshold = 1.0;

    Classifier<TYPE> classifier(lightThreshold, colorThreshold, balance);

    TYPE pix0[] = {3, 3, 3};
    TYPE pix1[] = {3, 3, 3};
    
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 3);
    std::fill( pix1, pix1 + channels, 10);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::back, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 10);
    std::fill( pix1, pix1 + channels, 3);
    classifier.copy_pix(pix0, pix1);
    if (Transition::fwd != classifier.f_classifier())
        ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);
    //check that all values are between 1 and 1 (and not Nan)
    // if (0 != cvtest::check(, 1, 1, 0) )
    //     ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_TEST_DATA);
  }
};
TEST(ComparatorLibSuite, f_classifier)
{
  Fclassifier fclassifeir;
  fclassifeir.safe_run();
}


class FclassifierBT : public cvtest::BaseTest
{
public:
  FclassifierBT(){}
protected:
  void run(int) {
    ts->set_failed_test_info(cvtest::TS::OK);

    double balance[] = {2.0, 1.0, 1.0};
    double lightThreshold = 1.0;
    double colorThreshold = 4.0;

    Classifier<TYPE> classifier(lightThreshold, colorThreshold, balance);


    TYPE pix0[] = {3, 3, 3};
    TYPE pix1[] = {3, 3, 3};
    
    classifier.copy_pix(pix0, pix1);
    if (Transition::no != classifier.f_classifier())
        ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);

    std::fill( pix0, pix0 + channels, 6);
    std::fill( pix1, pix1 + channels, 10);
    pix0[0] = 4;      
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::back, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 10);
    std::fill( pix1, pix1 + channels, 3);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());

    
    balance[0] = 0.9;
    lightThreshold = 0.0;
    colorThreshold = 10.0;
    classifier.set_parameters(lightThreshold, colorThreshold, balance);

    std::fill( pix0, pix0 + channels, 3);
    std::fill( pix1, pix1 + channels, 3);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 30);
    std::fill( pix1, pix1 + channels, 30);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::back, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 30);
    std::fill( pix1, pix1 + channels, 30);
    pix0[0] = 35;
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 33);
    std::fill( pix1, pix1 + channels, 30);
    pix1[0] = 34;
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::fwd, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 33);
    std::fill( pix1, pix1 + channels, 30);
    pix1[0] = 33;
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::fwd, classifier.f_classifier());

    
    balance[0] = 1.1;
    lightThreshold = 0.0;
    colorThreshold = 10.0;
    classifier.set_parameters(lightThreshold, colorThreshold, balance);

    std::fill( pix0, pix0 + channels, 33);
    std::fill( pix1, pix1 + channels, 33);
    pix1[0] = 30;
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());


    balance[0] = 1.0;
    lightThreshold = 90.0;
    colorThreshold = 200.0;
    classifier.set_parameters(lightThreshold, colorThreshold, balance);

    std::fill( pix0, pix0 + channels, 50);
    std::fill( pix1, pix1 + channels, 50 + 29);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 50);
    std::fill( pix1, pix1 + channels, 50 + 30);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 50);
    std::fill( pix1, pix1 + channels, 50 + 31);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::back, classifier.f_classifier());
    // std::fill( pix0, pix0 + channels, 10);
    // std::fill( pix1, pix1 + channels, 3);
    // pix0[0] = 5;
    // classifier.copy_pix(pix0, pix1);
    // if (Transition::back != classifier.f_classifier())
    //     ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);
    //check that all values are between 1 and 1 (and not Nan)
    // if (0 != cvtest::check(, 1, 1, 0) )
    //     ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_TEST_DATA);
  }
};
TEST(ComparatorLibSuite, f_classifierbt)
{
  FclassifierBT fclassifierbt;
  fclassifierbt.safe_run();
}


#ifdef TEST_PRIVATE_PART
  class LightAndColor : public cvtest::BaseTest
  {
  public:
    LightAndColor(){}
  protected:
    void run(int)
    {
      double balance[] = {1.0, 1.0, 1.0};
      double lightThreshold = 1.0;
      double colorThreshold = 1.0;

      Classifier<TYPE> classifier(lightThreshold, colorThreshold, balance);


      TYPE pix0[] = {3, 3, 3};
      TYPE pix1[] = {10, 10, 10};
      
      classifier.copy_pix(pix0, pix1);
      ASSERT_EQ(21, classifier.light_distance());
      ASSERT_EQ(0, classifier.color_distance());
      
      pix1[0] = 13;
      classifier.copy_pix(pix0, pix1);
      ASSERT_EQ(24, classifier.light_distance());
      ASSERT_EQ(6, classifier.color_distance());
    }
  };
  TEST(ComparatorPrivateLibSuite, PrivatePartClassifier)
  {
    LightAndColor lightAndColor;
    lightAndColor.safe_run();
  }
#endif


class DataContainers : public cvtest::BaseTest
{
public:
  DataContainers(){}
protected:
  void run(int)
  {
    Transition trans1 = biUpDw;
    Transition trans2 = biLR;
    trans1|=trans2;
    ASSERT_EQ(trans1, all);

    trans1=lToR;
    trans2=rToL;
    trans1|=trans2;
    ASSERT_EQ(trans1, biLR);
  }
};
TEST(ComparatorLibSuite, DataContainers)
{
  DataContainers DataContainers;
  DataContainers.safe_run();
}
      
// TEST(ComparatorLibSuite, ATestThatFails) {
//   bool mybool = false;
//   ASSERT_TRUE(mybool);
// }
