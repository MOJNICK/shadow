#include <opencv2/ts/ts.hpp>
#include <libcomparator.hpp>


class IterateH : public cvtest::BaseTest
{
public:
    IterateH(){}
protected:
    
    void run(int) {
      cv::Mat_<TYPE> mat(3, 3 * channels, 100);
      // cv::Vec<TYPE, channels> subPix;
      // subPix = static_cast<cv::Vec<TYPE, channels>>(cv::Matx<TYPE, channels, 1>::zeros());
      // mat.at<cv::Vec<TYPE, channels>>(cv::Point(3,3)) = subPix;
      mat(1,3)=0; mat(1,4)=0; mat(1,5)=0;
      double balance[] = {1.0, 1.0, 1.0};
      double lightThreshold = 1.0;
      double colorThreshold = 1.0;

      IterateProcess<TYPE> iterateProcess(mat, lightThreshold, colorThreshold, balance);

      std::vector<IndexTransition> result = iterateProcess.iterate_H();

      ASSERT_EQ(result.size(), 2);
      auto expected = IndexTransition{12, leftToRight};
      compareIndexTransition(result[0], expected);
      expected = IndexTransition{12, rightToLeft};
      compareIndexTransition(result[1], expected);

      // ASSERT_EQ(result[0], IndexTransition{12, rightToLeft});

  }

  void compareIndexTransition(IndexTransition& iT1, IndexTransition& iT2)
  {
    ASSERT_EQ(iT1.index, iT2.index);
    ASSERT_EQ(iT1.transition, iT2.transition);
  }
};
TEST(ComparatorLibSuite, IterateH) {
  IterateH iterateH;
  iterateH.safe_run();
}

class IterateV : public cvtest::BaseTest
{
public:
    IterateV(){}
protected:
    
    void run(int) {
      cv::Mat_<TYPE> mat(3, 3 * channels, 100);
      // cv::Vec<TYPE, channels> subPix;
      // subPix = static_cast<cv::Vec<TYPE, channels>>(cv::Matx<TYPE, channels, 1>::zeros());
      // mat.at<cv::Vec<TYPE, channels>>(cv::Point(3,3)) = subPix;
      mat(1,3)=0; mat(1,4)=0; mat(1,5)=0;
      double balance[] = {1.0, 1.0, 1.0};
      double lightThreshold = 1.0;
      double colorThreshold = 1.0;

      IterateProcess<TYPE> iterateProcess(mat, lightThreshold, colorThreshold, balance);

      std::vector<IndexTransition> result = iterateProcess.iterate_V();

      ASSERT_EQ(result.size(), 2);
      auto expected = IndexTransition{12, upToDown};
      compareIndexTransition(result[0], expected);
      expected = IndexTransition{12, downToUp};
      compareIndexTransition(result[1], expected);

      // ASSERT_EQ(result[0], IndexTransition{12, rightToLeft});

  }

  void compareIndexTransition(IndexTransition& iT1, IndexTransition& iT2)
  {
    ASSERT_EQ(iT1.index, iT2.index);
    ASSERT_EQ(iT1.transition, iT2.transition);
  }
};
TEST(ComparatorLibSuite, IterateV) {
  IterateV IterateV;
  IterateV.safe_run();
}



class Fclassifier : public cvtest::BaseTest
{
public:
    Fclassifier(){}
protected:
    
    void run(int) {
      double balance[] = {1.0, 1.0, 1.0};
      double lightThreshold = 1.0;
      double colorThreshold = 1.0;

      Classifier<TYPE> classifier(lightThreshold, colorThreshold, balance);

      TYPE pix0[] = {3, 3, 3};
      TYPE pix1[] = {3, 3, 3};
      
      classifier.copy_pix(pix0, pix1);
      ASSERT_EQ(Transition::no, classifier.f_classifier());

      std::fill( pix0, pix0 + channels, 3);
      std::fill( pix1, pix1 + channels, 10);
      classifier.copy_pix(pix0, pix1);
      ASSERT_EQ(Transition::backward, classifier.f_classifier());

      std::fill( pix0, pix0 + channels, 10);
      std::fill( pix1, pix1 + channels, 3);
      classifier.copy_pix(pix0, pix1);
      if (Transition::forward != classifier.f_classifier())
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);
      //check that all values are between 1 and 1 (and not Nan)
      // if (0 != cvtest::check(, 1, 1, 0) )
      //     ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_TEST_DATA);

  }
};
TEST(ComparatorLibSuite, f_classifier) {
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
      ASSERT_EQ(Transition::backward, classifier.f_classifier());

      std::fill( pix0, pix0 + channels, 10);
      std::fill( pix1, pix1 + channels, 3);
      classifier.copy_pix(pix0, pix1);
      ASSERT_EQ(Transition::no, classifier.f_classifier());

      // std::fill( pix0, pix0 + channels, 10);
      // std::fill( pix1, pix1 + channels, 3);
      // pix0[0] = 5;
      // classifier.copy_pix(pix0, pix1);
      // if (Transition::backward != classifier.f_classifier())
      //     ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);
      //check that all values are between 1 and 1 (and not Nan)
      // if (0 != cvtest::check(, 1, 1, 0) )
      //     ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_TEST_DATA);

  }
};
TEST(ComparatorLibSuite, f_classifierbt) {
  FclassifierBT fclassifierbt;
  fclassifierbt.safe_run();
}


#ifdef TEST_PRIVATE_PART

class LightAndColor : public cvtest::BaseTest
{
public:
    LightAndColor(){}
protected:
    
    void run(int) {
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
TEST(ComparatorPrivateLibSuite, PrivatePartClassifier) {
  LightAndColor lightAndColor;
  lightAndColor.safe_run();
}
#endif

// TEST(ComparatorLibSuite, ATestThatFails) {
//   bool mybool = false;
//   ASSERT_TRUE(mybool);
// }
