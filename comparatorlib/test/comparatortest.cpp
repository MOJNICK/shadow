#include <opencv2/ts/ts.hpp>
#include <comparatorlib.hpp>

class ComparatorTestClass : public cvtest::BaseTest
{
public:
    ComparatorTestClass(){}
protected:
    
    void run(int) {
      ts->set_failed_test_info(cvtest::TS::OK);

      double balance[] = {1.0, 1.0, 1.0};
      Classifier<int> classifier(1.0, 1.0, balance);

      int pix0[] = {100, 100, 100};
      int pix1[] = {100, 100, 100};
      classifier.copy_pix(pix0, pix1);
      if (Transition::no != classifier.f_classifier())
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);


      std::fill( pix1, pix1 + sizeof( pix1 ), 101 );
      classifier.copy_pix(pix0, pix1);
      if (Transition::forward != classifier.f_classifier())
          ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_OUTPUT);
      //check that all values are between 1 and 1 (and not Nan)
      // if (0 != cvtest::check(, 1, 1, 0) )
      //     ts->set_failed_test_info(cvtest::TS::FAIL_INVALID_TEST_DATA);

  }
};

TEST(ComparatorLibSuite, ATestThatPasses) {
  ComparatorTestClass comparatorLibTestClass;
  comparatorLibTestClass.safe_run();
}

// TEST(ComparatorLibSuite, ATestThatFails) {
//   bool mybool = false;
//   ASSERT_TRUE(mybool);
// }
