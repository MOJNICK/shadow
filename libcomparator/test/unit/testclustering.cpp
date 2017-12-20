#include <opencv2/ts/ts.hpp>
#include "clustering.hpp"

class TestClusteringMethods : public cvtest::BaseTest
{
public:
  TestClusteringMethods(){}
protected:
  void compareIndexTransition(IndexTransition& expected, IndexTransition& actual)
  {
    ASSERT_TRUE( expected.same_position( actual ) );
    ASSERT_EQ(expected.transition, actual.transition);
  }
};
/*
class ColorStructsaturation : public cvtest::BaseTest
{
public:
protected:
    void run(int)
    {
        ColorStruct::baseLevel = 0;
        ColorStruct cs{ 0, 0, 0};
        ASSERT_DOUBLE_EQ( 0.0, cs.saturation() );
    }
};
TEST(ColorStructSuite, ColorStructsaturation)
{
  ColorStructsaturation colorStructsaturation;
  colorStructsaturation.safe_run();
}
*/
class TestClustering : public TestClusteringMethods
{
    TestClustering()
    {
        int siz0 = 10;
        int siz1 = 2;
        std::vector<IndexTransition> vIndexTransition{ {1, 2, lToR}, {4, 2, lToR}, {11, 2, lToR}, {12, 2, lToR}, {9, 2, lToR}, 
                                                        {6, 8, rToL}, {8, 8, rToL}, {2, 3, rToL}, {3, 4, rToL}, {5, 7, rToL} };

        //self.npArray = np.array([[1, 2], [4, 2], [11, 2], [12, 2], [9, 2], [6, 8], [8, 8], [2, 3], [3, 4], [5, 7]],
          //                      dtype=np.int)
        double eps = 2;
        uint minPts = 3;
        Clustering objectClustering( vIndexTransition, Distance::distance_fast, eps, minPts);
    }
};
/*
    def setUp(self):
        self.siz0 = 10
        self.siz1 = 2
        self.npArray = np.array([[1, 2], [4, 2], [11, 2], [12, 2], [9, 2], [6, 8], [8, 8], [2, 3], [3, 4], [5, 7]],
                                dtype=np.int)
        self.eps = 2
        self.minPts = 3
        self.objectClustering = clustering.Clustering(self.npArray, clustering.Distance.distance_fast, self.eps, self.minPts)


    def test_init_clustering(self):
        self.assertEqual(self.objectClustering.clusters.shape, (self.siz0, self.siz1 + 1), 'missed size')
        np.testing.assert_equal(self.objectClustering.clusters, np.array([[1, 2, 0], [2, 3, 0], [3, 4, 0], [4, 2, 0], [5, 7, 0],
                                                                          [6, 8, 0], [8, 8, 0], [9, 2, 0], [11, 2, 0], [12, 2, 0]], dtype=np.int), 'not sorted or missing cluster number')


    def test_remove_small_clusters_and_noise_first(self):
        self.objectClustering.clusters[1:4,2] = 1
        self.objectClustering.clusters[6:9, 2] = 6
        self.objectClustering.remove_small_clusters_and_noise()
        np.testing.assert_equal(self.objectClustering.clusters, np.array([[2, 3, 1], [3, 4, 1], [4, 2, 1],
                                                                         [8, 8, 6], [9, 2, 6], [11, 2, 6]], dtype=np.int), 'not removing correctly')


    def test_remove_small_clusters_and_noise_second(self):
        self.objectClustering.clusters[1:4,2] = 1
        self.objectClustering.clusters[7:9, 2] = 6
        self.objectClustering.remove_small_clusters_and_noise()
        np.testing.assert_equal(self.objectClustering.clusters, np.array([[2, 3, 1], [3, 4, 1], [4, 2, 1]], dtype=np.int), 'not removing correctly')


    def test_remove_small_clusters_and_noise_third(self):
        self.objectClustering.minPts = 12
        self.objectClustering.clusters[1:4,2] = 1
        self.objectClustering.clusters[6:9, 2] = 6
        self.objectClustering.remove_small_clusters_and_noise()
        np.testing.assert_equal(self.objectClustering.clusters, np.empty([0, 3], dtype=np.int), 'problem with no big cluster')


    def test_remove_small_clusters_and_noise_fourth(self):
        self.objectClustering.minPts = 12
        self.objectClustering.remove_small_clusters_and_noise()
        np.testing.assert_equal(self.objectClustering.clusters, np.empty([0, 3], dtype=np.int), 'problem with no cluster')


    def test_point_zone_linear_first(self):
        self.objectClustering.check_point_zone_linear(2)
        np.testing.assert_equal(self.objectClustering.clusters, np.array([[1, 2, 0], [2, 3, 1], [3, 4, 1], [4, 2, 0], [5, 7, 0],
                                                                          [6, 8, 0], [8, 8, 0], [9, 2, 0], [11, 2, 0], [12, 2, 0]], dtype=np.int), '')

    def test_point_zone_linear_second(self):
        self.objectClustering.eps = 5
        self.objectClustering.check_point_zone_linear(2)
        np.testing.assert_equal(self.objectClustering.clusters, np.array([[1, 2, 1], [2, 3, 1], [3, 4, 1], [4, 2, 1], [5, 7, 1],
                                                                          [6, 8, 0], [8, 8, 0], [9, 2, 0], [11, 2, 0], [12, 2, 0]], dtype=np.int), '')


    def test_distance_fast(self):
        pixelA = np.array([1, 3, 7])
        pixelB = np.array([1, 3, 7])
        self.assertEqual(clustering.Distance.distance_fast(pixelA, pixelB), 0, 'fast distance function problem')

        pixelB = np.array([0, 1, 7])
        self.assertEqual(clustering.Distance.distance_fast(pixelA, pixelB), 3, 'fast distance function problem')

        pixelB = np.array([0, 10, 2])
        self.assertEqual(clustering.Distance.distance_fast(pixelA, pixelB), 8, 'fast distance function problem')

        pixelA = np.array([3, 4, 1])
        pixelB = np.array(self.objectClustering.clusters[3])
        self.assertEqual(clustering.Distance.distance_fast(pixelA, pixelB), 3, 'fast distance function problem')

    def test_points_clustering_first(self):
        self.objectClustering.eps = 3
        self.objectClustering.points_clustering(self.objectClustering.check_point_zone_linear)
        np.testing.assert_equal(self.objectClustering.clusters, np.array([[1, 2, 1], [2, 3, 1], [3, 4, 1], [4, 2, 1], [5, 7, 2],
                                                                          [6, 8, 2], [8, 8, 2], [9, 2, 3], [11, 2, 3], [12, 2, 3]], dtype=np.int), '')

    def test_points_clustering_second(self):
        self.objectClustering.eps = 3
        self.objectClustering.minPts = 4
        self.objectClustering.points_clustering(self.objectClustering.check_point_zone_linear)
        np.testing.assert_equal(self.objectClustering.clusters, np.array([[1, 2, 1], [2, 3, 1], [3, 4, 1], [4, 2, 1]], dtype=np.int), '')
*/