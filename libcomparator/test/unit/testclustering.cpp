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
  void compareIndexTransitionCluster( IndexTransitionCluster const & expected, IndexTransitionCluster const & actual )
  {
    ASSERT_EQ(expected.row, actual.row);
    ASSERT_EQ(expected.col, actual.col);
    ASSERT_EQ(expected.transition, actual.transition);
    ASSERT_EQ(expected.clusterNumber, actual.clusterNumber);
  }
  
  void compareIndexTransitionClusterVec( std::vector<IndexTransitionCluster> const & expected, std::vector<IndexTransitionCluster> const & actual )
  {
      ASSERT_EQ( expected, actual );
  }
};

class TestClustering : public TestClusteringMethods
{
public:
        int siz0;
        int siz1;
        double eps;
        uint minPts;
        std::vector<IndexTransition> const vIndexTransition;
        Clustering objectClustering;
    TestClustering(): siz0{10}, siz1{2}, eps{2.0}, minPts{3}, vIndexTransition{ {1, 2, lToR}, {4, 2, lToR}, {11, 2, lToR},
                      {12, 2, lToR}, {9, 2, lToR}, {6, 8, rToL}, {8, 8, rToL}, {2, 3, rToL}, {3, 4, rToL}, {5, 7, rToL} },
                      objectClustering( vIndexTransition, Distance::distance_fast, eps, minPts)
    {
        
    }
    void SetUp()
    {
        siz0 = 10;
        siz1 =2;
        eps = 2.0;
        minPts = 3;     
        objectClustering = Clustering( vIndexTransition, Distance::distance_fast, eps, minPts);
    }
};

class ClusteringConstructor : public TestClustering
{
public:
protected:
    void run(int)
    {
        std::vector<IndexTransitionCluster>& actual = objectClustering.vIndexTransitionCluster;
        std::vector<IndexTransitionCluster> expected{   {1, 2, lToR, 0}, {2, 3, rToL, 0}, {3, 4, rToL, 0},
                                                        {4, 2, lToR, 0}, {5, 7, rToL, 0}, {6, 8, rToL, 0},
                                                        {8, 8, rToL, 0}, {9, 2, lToR, 0}, {11, 2, lToR, 0}, {12, 2, lToR, 0}  };

        ASSERT_EQ(expected, actual);
    }
};
TEST(ClusteringSuite, ClusteringConstructor)
{
  ClusteringConstructor clusteringConstructor;
  clusteringConstructor.safe_run();
}

class ClusteringSmallAndNoise : public TestClustering
{
public:
protected:
    void run(int)
    {
        SetUp();
        std::vector<IndexTransitionCluster>& actual = objectClustering.vIndexTransitionCluster;
        for(int i = 1; i < 4; ++ i)
        {
            actual[i].clusterNumber = 1;
        }
        for(int i = 6; i < 9; ++ i)
        {
            actual[i].clusterNumber = 6;
        }
        objectClustering.remove_small_clusters_and_noise();

        std::vector<IndexTransitionCluster> expected{   {2, 3, rToL, 1}, {3, 4, rToL, 1}, {4, 2, lToR, 1},
                                                        {8, 8, rToL, 6}, {9, 2, lToR, 6}, {11, 2, lToR, 6}  };
        ASSERT_EQ(expected, actual);

        
        SetUp();
        for(int i = 1; i < 4; ++ i)
        {
            actual[i].clusterNumber = 1;
        }
        for(int i = 7; i < 9; ++ i)
        {
            actual[i].clusterNumber = 6;
        }
        objectClustering.remove_small_clusters_and_noise();

        expected = {   {2, 3, rToL, 1}, {3, 4, rToL, 1}, {4, 2, lToR, 1}   };
        ASSERT_EQ(expected, actual);
    }
};
TEST(ClusteringSuite, ClusteringSmallAndNoise)
{
  ClusteringSmallAndNoise clusteringSmallAndNoise;
  clusteringSmallAndNoise.safe_run();
}

/*



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

/*    def setUp(self):
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
*/