#ifndef CLUSTERING_HPP
    #define CLUSTERING_HPP
    #include "libcomparator.hpp"

    
    struct IndexTransitionCluster: public IndexTransition
    {
        IndexTransitionCluster( IndexTransition indexTransition ): IndexTransition( indexTransition ), clusterNumber{0}{}
        uint getCluster();
    private:
        uint clusterNumber;
    }

    struct IndexTransitionClusterVec
    {
        IndexTransitionClusterVec( std::vector<IndexTransition> vIndexTransition );
        std::vector<PointCluster> vIndexTransitionCluster;
    }

    class Clustering
    {
    public:
        Clustering( npArray, distance_function, eps, minPts );
        int create_new_cluster();
        int get_cluster_number();
        void points_clustering( void (*check_point_zone_function)(int) );
        void check_point_zone_linear( int indexX );
        void remove_small_clusters_and_noise();
    private:
        uint nowClusterNumber;
        double eps;
        uint minPts;
        IndexTransitionClusterVec clusters;
        double (*distance_function)( IndexTransitionCluster const & , IndexTransitionCluster const & );
    }


    class Distance
    {
    public:
        static double distance_fast(IndexTransitionCluster const & pixelA, IndexTransitionCluster const & pixelB);
        static double distance_slow(IndexTransitionCluster const & pixelA, IndexTransitionCluster const & pixelB);
    }
#endif