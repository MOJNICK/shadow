#include <opencv2/ts/ts.hpp>
#include <libcomparator.hpp>


class Fclassifier : public cvtest::BaseTest
{
public:
    Fclassifier(){}
protected:
    
    void run(int) {
      ts->set_failed_test_info(cvtest::TS::OK);

      double balance[] = {1.0, 1.0, 1.0};
      Classifier<int> classifier(1.0, 1.0, balance);

      int pix0[] = {3, 3, 3};
      int pix1[] = {3, 3, 3};
      
      classifier.copy_pix(pix0, pix1);
      ASSERT_EQ(Transition::no, classifier.f_classifier());

      std::fill( pix0, pix0 + channels, 3);
      std::fill( pix1, pix1 + channels, 10);
      classifier.copy_pix(pix0, pix1);
      ASSERT_EQ(Transition::forward, classifier.f_classifier());

      std::fill( pix0, pix0 + channels, 10);
      std::fill( pix1, pix1 + channels, 3);
      classifier.copy_pix(pix0, pix1);
      if (Transition::backward != classifier.f_classifier())
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
      Classifier<int> classifier(1.0, 4.0, balance);

      int pix0[] = {3, 3, 3};
      int pix1[] = {3, 3, 3};
      
      classifier.copy_pix(pix0, pix1);
      if (Transition::no != classifier.f_classifier())
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);

      std::fill( pix0, pix0 + channels, 6);
      std::fill( pix1, pix1 + channels, 10);
      pix0[0] = 4;      
      classifier.copy_pix(pix0, pix1);
      ASSERT_EQ(Transition::forward, classifier.f_classifier());

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
      ts->set_failed_test_info(cvtest::TS::OK);

      double balance[] = {1.0, 1.0, 1.0};
      Classifier<int> classifier(1.0, 1.0, balance);

      int pix0[] = {3, 3, 3};
      int pix1[] = {10, 10, 10};
      
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
