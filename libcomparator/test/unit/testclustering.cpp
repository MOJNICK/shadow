#include <opencv2/ts/ts.hpp>
#include "clustering.hpp"


class TestClustering : public cvtest::BaseTest
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
        SetUp();
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


        SetUp();
        objectClustering.minPts = 12;
        for(int i = 1; i < 4; ++ i)
        {
            actual[i].clusterNumber = 1;
        }
        for(int i = 6; i < 9; ++ i)
        {
            actual[i].clusterNumber = 6;
        }
        objectClustering.remove_small_clusters_and_noise();

        expected = { };
        ASSERT_EQ(expected, actual);

        SetUp();
        objectClustering.minPts = 12;
        objectClustering.remove_small_clusters_and_noise();
        expected = { };
        ASSERT_EQ(expected, actual);
    }
};
TEST(ClusteringSuite, ClusteringSmallAndNoise)
{
  ClusteringSmallAndNoise clusteringSmallAndNoise;
  clusteringSmallAndNoise.safe_run();
}

class ClusteringPointZoneLinear: public TestClustering
{
public:
protected:
    void run(int)
    {
        SetUp();
        std::vector<IndexTransitionCluster>& actual = objectClustering.vIndexTransitionCluster;
        objectClustering.check_point_zone_linear(2);

        std::vector<IndexTransitionCluster> expected{   {1, 2, lToR, 0}, {2, 3, rToL, 1}, {3, 4, rToL, 1},
                                                        {4, 2, lToR, 0}, {5, 7, rToL, 0}, {6, 8, rToL, 0},
                                                        {8, 8, rToL, 0}, {9, 2, lToR, 0}, {11, 2, lToR, 0}, {12, 2, lToR, 0}  };
        ASSERT_EQ(expected, actual);

        SetUp();
        objectClustering.eps = 5.0;
        objectClustering.check_point_zone_linear(2);

        expected = {    {1, 2, lToR, 1}, {2, 3, rToL, 1}, {3, 4, rToL, 1}, {4, 2, lToR, 1}, {5, 7, rToL, 1},
                        {6, 8, rToL, 0}, {8, 8, rToL, 0}, {9, 2, lToR, 0}, {11, 2, lToR, 0}, {12, 2, lToR, 0}  };
        ASSERT_EQ(expected, actual);
    }
};
TEST(ClusteringSuite, ClusteringPointZoneLinear)
{
  ClusteringPointZoneLinear clusteringPointZoneLinear;
  clusteringPointZoneLinear.safe_run();
}

class ClusteringDistanceFast: public TestClustering
{
public:
protected:
    void run(int)
    {
        SetUp();
        IndexTransitionCluster pixelA{ 1, 3, lToR, 7 };
        IndexTransitionCluster pixelB{ 1, 3, lToR, 7 };
        ASSERT_DOUBLE_EQ(0.0, Distance::distance_fast(pixelA, pixelB));

        pixelB = { 0, 1, lToR, 7 };
        ASSERT_DOUBLE_EQ(3.0, Distance::distance_fast(pixelA, pixelB));

        pixelB = { 0, 10, lToR, 2 };
        ASSERT_DOUBLE_EQ(8.0, Distance::distance_fast(pixelA, pixelB));

        pixelA = { 3, 4, lToR, 1 };
        pixelB = objectClustering.vIndexTransitionCluster[3];
        ASSERT_DOUBLE_EQ(3.0, Distance::distance_fast(pixelA, pixelB));
    }
};
TEST(ClusteringSuite, ClusteringDistanceFast)
{
  ClusteringDistanceFast clusteringDistanceFast;
  clusteringDistanceFast.safe_run();
}

class ClusteringPointsClustering: public TestClustering
{
public:
protected:
    void run(int)
    {
        SetUp();
        std::vector<IndexTransitionCluster>& actual = objectClustering.vIndexTransitionCluster;
        objectClustering.eps = 3.0;
        objectClustering.points_clustering(&Clustering::check_point_zone_linear);
        std::vector<IndexTransitionCluster> expected{   {1, 2, lToR, 1}, {2, 3, rToL, 1}, {3, 4, rToL, 1},
                                                        {4, 2, lToR, 1}, {5, 7, rToL, 2}, {6, 8, rToL, 2},
                                                        {8, 8, rToL, 2}, {9, 2, lToR, 3}, {11, 2, lToR, 3}, {12, 2, lToR, 3}  };
        ASSERT_EQ(expected, actual);

        SetUp();
        objectClustering.eps = 3.0;
        objectClustering.minPts = 4;
        objectClustering.points_clustering(&Clustering::check_point_zone_linear);
        expected = {{1, 2, lToR, 1}, {2, 3, rToL, 1}, {3, 4, rToL, 1}, {4, 2, lToR, 1}};
        ASSERT_EQ(expected, actual);
    }
};
TEST(ClusteringSuite, ClusteringPointsClustering)
{
  ClusteringPointsClustering clusteringPointsClustering;
  clusteringPointsClustering.safe_run();
}