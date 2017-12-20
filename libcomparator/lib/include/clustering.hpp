#ifndef CLUSTERING_HPP
    #define CLUSTERING_HPP
    #include "libcomparator.hpp"

    
    struct IndexTransitionCluster: public IndexTransition
    {
        IndexTransitionCluster( IndexTransition indexTransition ): IndexTransition( indexTransition ), clusterNumber{0}{}
        uint getCluster();
    private:
        uint clusterNumber;
    };

    class Clustering
    {
    public:
        Clustering( std::vector<IndexTransition> vIndexTransition, double (*distance_function)(cv::Point, cv::Point), double eps, uint minPts);
        uint create_new_cluster();
        uint get_cluster_number();
        void points_clustering( void (*check_point_zone_function)(int) );
        void check_point_zone_linear( int indexX );
        void remove_small_clusters_and_noise();
    private:
        uint nowClusterNumber;
        double eps;
        uint minPts;
        std::vector<IndexTransitionCluster>  vIndexTransitionCluster;
        double (*distance_function)( IndexTransitionCluster const & , IndexTransitionCluster const & );
    };


    class Distance
    {
    public:
        static double distance_fast(IndexTransitionCluster const & pixelA, IndexTransitionCluster const & pixelB);
        static double distance_slow(IndexTransitionCluster const & pixelA, IndexTransitionCluster const & pixelB);
    };
#endif