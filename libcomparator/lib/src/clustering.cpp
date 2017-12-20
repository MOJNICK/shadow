#include "clustering.hpp"

IndexTransitionClusterVec::IndexTransitionClusterVec( std::vector<IndexTransition> vIndexTransition )
{

    std::for_each(vIndexTransition.begin(), vIndexTransition.end(), [ this ]( IndexTransition const & indexTransition ){
        vPointCluster.push_back(IndexTransitionCluster(indexTransition, 0));
    });
}

Clustering::Clustering( std::vector<IndexTransition> vIndexTransition, double (*distance_function)(cv::Point, cv::Point), double eps, uint minPts):
clusters(vIndexTransition), eps{eps}, minPts{minPts}, nowClusterNumber{0}
{
    //""npArray with only possitive detection points npArray=[[pixelX, pixelY]]
    //dist_function must be a vector norm
    //eps - maximum point to point distance to belong to cluster
    //minPts minimum points amount to make new cluster
    //npArray = npArray[npArray[:, 0].argsort()];  // sort by first column
    std::sort(clusters.begin(), clusters.end(), []( IndexTransitionCluster const & p1, IndexTransitionCluster const & p2 ){
        if( p1.row < p2.row )
        {
            return true;
        }
        else
        {
            return false;
        }
    });
}

uint Clustering::create_new_cluster()
{
    nowClusterNumber += 1;
    return nowClusterNumber;
}

uint Clustering::get_cluster_number()
{
    return nowClusterNumber;
}


void Clustering::points_clustering( void (*check_point_zone_function)(int) )
//    #   returns npArray: [pixelX, pixelY, clusterNumber]
{
//    if (self.clusters.shape[1] != 3)
//        throw ValueError("Passed array is not the right shape")

    for( int indexX = 0; indexX < vIndexTransitionCluster.size(); ++indexX )// in xrange(self.clusters.shape[0]):
    {
        check_point_zone_function(indexX);
    }

    remove_small_clusters_and_noise();
}

void Clustering::check_point_zone_linear( int indexX )
{
    uint currentClusterNumber = 0;
    IndexTransitionCluster point = vIndexTransitionCluster[indexX];
    if( point.clusterNumber == 0 )
    {
        currentClusterNumber = create_new_cluster();
        point.clusterNumber = currentClusterNumber;
    }
    else
    {
        currentClusterNumber = point.clusterNumber;
    }

    double minX = point.row - self.eps;
    double maxX = point.row + self.eps;

    for( int i = 0; i < vIndexTransitionCluster.size(); ++i )//:#all elements
    {
        if( cluster[i].row < minX)
            continue;
        else
        {
            if( maxX < vIndexTransitionCluster[i].row )
            {
                break;
            }
            else
            {
                if (distance_function(point, vIndexTransitionCluster[i]) <= self.eps )
                {
                    vIndexTransitionCluster[i].clusterNumber = currentClusterNumber;
                }
            }
        }
    }
}

void Clustering::remove_small_clusters_and_noise()
{
    maxClusterNumber = std::max_element( vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), []( IndexTransitionCluster const & itc1, IndexTransitionCluster const & itc2){
        if(itc1.clusterNumber < itc2.clusterNumber )
        {
            return true;
        }
        else
        {
            return false;
        }
    });//clusters[:,2])

    std::vector<uint>clusterOccurences(maxClusterNumber + 1, 0);
    for_each( vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), [&clusterOccurences]( IndexTransitionCluster point){
        if( point.clusterNumber == 0 )
            continue;
        else
            clusterOccurences[point.clusterNumber] += 1;
    });
    
    vIndexTransitionCluster.erase(std::remove_if(vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), [](IndexTransitionCluster const & itc){//remove small clusters
        if( clusterOccurences[itc.clusterNumber] < minPts )
        {
            return true;
        }
        else
        {
            return false;
        }
    }), vIndexTransitionCluster.end());
}

static double distance_fast( IndexTransitionCluster const & pixelA, IndexTransitionCluster const & pixelB )
{
    return abs( pixelA.row - pixelB.row ) + abs( pixelA.col - pixelB.col );
}


static double distance_slow( IndexTransitionCluster const & pixelA, IndexTransitionCluster const & pixelB )
{
    return sqrt( pow( ( pixelA.row - pixelBrow ), 2.0 ) + pow( ( pixelA.col - pixelB.col ), 2.0 ) );
}
