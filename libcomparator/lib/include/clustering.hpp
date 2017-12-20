#ifndef CLUSTERING_HPP
    #define CLUSTERING_HPP
    #include "libcomparator.hpp"

    
    struct IndexTransitionCluster: public IndexTransition
    {
    public:
        IndexTransitionCluster( IndexTransition indexTransition ): IndexTransition( indexTransition ), clusterNumber{0}{}
        IndexTransitionCluster( IndexTransition indexTransition, uint clusterNumber ): IndexTransition( indexTransition ), clusterNumber{clusterNumber}{}
        IndexTransitionCluster( uint row, uint col, Transition transition, uint clusterNumber ): IndexTransition{ row, col, transition}, clusterNumber{clusterNumber}{}
        #ifdef WITH_TESTS
            #ifdef TEST_PRIVATE_PART
            uint getCluster();
            bool operator ==(const IndexTransitionCluster &itc) const
            {
                if( static_cast<IndexTransition const &>(*this) == static_cast<IndexTransition const &>(itc) && this->clusterNumber == itc.clusterNumber )
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            #endif
        #endif
    private:
        uint clusterNumber;
    };

    class Clustering
    {
    public:
        Clustering( std::vector<IndexTransition> vIndexTransition, double (*distance_function)( IndexTransitionCluster const &, IndexTransitionCluster const & pixelA ), double eps, uint minPts);
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