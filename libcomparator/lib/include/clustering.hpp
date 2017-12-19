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
        int points_clustering( check_point_zone_function);
        void check_point_zone_linear( indexX );
        void remove_small_clusters_and_noise();
    private:
        int nowClusterNumber;
        double eps;
        uint minPts;
        double (*distance_function)(cv::Point_<uint>, cv::Point_<uint>);
    }


    class Distance
    {
    public:
        static double distance_fast(cv::Point pixelA, cv::Point pixelB);        
        static double distance_slow(cv::Point pixelA, cv::Point pixelB);
    }
#endif