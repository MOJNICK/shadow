#include <opencv2/ts/ts.hpp>
#include <comparatorlib.hpp>

class ComparatorTestClass : public cvtest::BaseTest
{
public:
    ComparatorTestClass():{}
protected:
    
    void run(int) {
      ts->set_failed_test_info(cvtest::TS::OK);

      Classifier classifier(1.0, 1.0, (double[]){1.0, 1.0, 1.0});

      int pix0 = {100, 100, 100};
      int pix1 = {100, 100, 100};

      classifier.copy_pix(pix0, pix1);
      //check that all values are between 1 and 1 (and not Nan)
      if (0 != cvtest::check(, 1, 1, 0) )
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_TEST_DATA);

      
      
      //check that all values are between 0 and 0 (and not Nan)
      if (0 != cvtest::check(classifier.f_classifier(), 0, 0, 0) )
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);
  }
};

TEST(ComparatorLibSuite, ATestThatPasses) {
  ComparatorTestClass comparatorLibTestClass;
  comparatorLibTestClass.safe_run();
}

TEST(ComparatorLibSuite, ATestThatFails) {
  bool mybool = false;
  ASSERT_TRUE(mybool);
}
