#include <opencv2/ts/ts.hpp>
#include "libcomparator.hpp"

class IterateTestMethods : public cvtest::BaseTest
{
public:
  IterateTestMethods(){}
protected:
  void compareIndexTransition(IndexTransition& expected, IndexTransition& actual)
  {
    ASSERT_TRUE( expected == actual );
    //ASSERT_EQ(expected.transition, actual.transition);
  }
};



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
    TYPE acceptanceLevel = 1;
    double balance[] = {1.0, 1.0, 1.0};
    double lightThreshold = 1.0;
    double colorThreshold = 1.0;
    IterateProcess<TYPE> iterateProcess(mat, acceptanceLevel, lightThreshold, colorThreshold, balance);
    
    std::vector<IndexTransition> result = iterateProcess.iterate_HV();
    std::vector<IndexTransition> expected{ };
    ASSERT_EQ( expected, result );
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
      TYPE acceptanceLevel = 1;
      double balance[] = {1.0, 1.0, 1.0};
      double lightThreshold = 1.0;
      double colorThreshold = 1.0;

      IterateProcess<TYPE> iterateProcess(mat, acceptanceLevel, lightThreshold, colorThreshold, balance);

      std::vector<IndexTransition> result = iterateProcess.iterate_H();

      std::vector<IndexTransition> expected{ { 1, 1, lToR }, { 1, 1, rToL } };
      ASSERT_EQ( expected, result );
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
      TYPE acceptanceLevel = 1;
      double balance[] = {1.0, 1.0, 1.0};
      double lightThreshold = 3.0;
      double colorThreshold = 1.0;

      IterateProcess<TYPE> iterateProcess(mat, acceptanceLevel, lightThreshold, colorThreshold, balance);

      std::vector<IndexTransition> result = iterateProcess.iterate_V();

      std::vector<IndexTransition> expected{ { 1, 1, upToDw }, { 1, 1, dwToUp } };
      ASSERT_EQ( expected, result );

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
    TYPE acceptanceLevel = 1;
    double balance[] = {1.0, 1.0, 1.0};
    double lightThreshold = 1.0;
    double colorThreshold = 1.0;

    Classifier<TYPE> classifier( acceptanceLevel, lightThreshold, colorThreshold, balance );

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

    TYPE acceptanceLevel = 1;
    double balance[] = {2.0, 1.0, 1.0};
    double lightThreshold = 1.0;
    double colorThreshold = 1.0;

    Classifier<TYPE> classifier( acceptanceLevel, lightThreshold, colorThreshold, balance);


    TYPE pix0[] = {3, 3, 3};
    TYPE pix1[] = {3, 3, 3};
    
    classifier.copy_pix(pix0, pix1);
    if (Transition::no != classifier.f_classifier())
        ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);

    std::fill( pix0, pix0 + channels, 6);
    std::fill( pix1, pix1 + channels, 14);
    pix0[0] = 3;      
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::back, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 6);
    std::fill( pix1, pix1 + channels, 12);
    pix0[0] = 3;      
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 10);
    std::fill( pix1, pix1 + channels, 3);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());

    
    balance[0] = 0.9;
    lightThreshold = 0.00001;
    colorThreshold = 0.05;
    classifier.set_parameters( acceptanceLevel, lightThreshold, colorThreshold, balance);

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
    lightThreshold = 0.0001;
    colorThreshold = 10.0;
    classifier.set_parameters( acceptanceLevel, lightThreshold, colorThreshold, balance);

    std::fill( pix0, pix0 + channels, 33);
    std::fill( pix1, pix1 + channels, 33);
    pix1[0] = 30;
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());


    balance[0] = 1.0;
    lightThreshold = 1.00001;
    colorThreshold = 200.0;
    classifier.set_parameters( acceptanceLevel, lightThreshold, colorThreshold, balance);

    std::fill( pix0, pix0 + channels, 50);
    std::fill( pix1, pix1 + channels, 100);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 100);
    std::fill( pix1, pix1 + channels, 50);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::no, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 50);
    std::fill( pix1, pix1 + channels, 101);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::back, classifier.f_classifier());

    std::fill( pix0, pix0 + channels, 101);
    std::fill( pix1, pix1 + channels, 50);
    classifier.copy_pix(pix0, pix1);
    ASSERT_EQ(Transition::fwd, classifier.f_classifier());


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
      TYPE acceptanceLevel = 1;
      double balance[] = {1.0, 1.0, 1.0};
      double lightThreshold = 0.0;
      double colorThreshold = 1.0;

      Classifier<TYPE> classifier( acceptanceLevel, lightThreshold, colorThreshold, balance);


      TYPE pix0[] = {8, 8, 8};
      TYPE pix1[] = {16, 16, 16};
      
      classifier.copy_pix(pix0, pix1);
      ASSERT_DOUBLE_EQ(2, classifier.light_distance());
      ASSERT_DOUBLE_EQ(0, classifier.color_distance());
      
      pix1[0] = 12;
      classifier.copy_pix(pix0, pix1);
      ASSERT_DOUBLE_EQ(44/24.0, classifier.light_distance());
      ASSERT_DOUBLE_EQ(2/12.0, classifier.color_distance());

      std::fill( pix0, pix0 + channels, 128);
      std::fill( pix1, pix1 + channels, 128);
      pix0[0] = 32;
      classifier.copy_pix(pix0, pix1);
      ASSERT_DOUBLE_EQ(6.0, classifier.color_distance());
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