#include <opencv2/ts/ts.hpp>
#include <comparatorlib.hpp>


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
      if (Transition::no != classifier.f_classifier())
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);

      std::fill( pix0, pix0 + channels, 3);
      std::fill( pix1, pix1 + channels, 10);
      classifier.copy_pix(pix0, pix1);
      if (Transition::forward != classifier.f_classifier())
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);

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
      if (21 != classifier.light_distance())
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);
      if (0 != classifier.color_distance())
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);
      
      pix1[0] = 13;
      classifier.copy_pix(pix0, pix1);
      if (24 != classifier.light_distance())
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);
      if (6 != classifier.color_distance())
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);



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
