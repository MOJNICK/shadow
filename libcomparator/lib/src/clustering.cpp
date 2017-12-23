#include "clustering.hpp"

Clustering::Clustering( std::vector<IndexTransition> const & vIndexTransition, double (*distance_function)(IndexTransitionCluster const &, IndexTransitionCluster const &), double eps, uint minPts):
eps{eps}, minPts{minPts}, nowClusterNumber{0}, distance_function{distance_function}
{
    std::for_each(vIndexTransition.begin(), vIndexTransition.end(), [ this ]( IndexTransition const & indexTransition ){
        vIndexTransitionCluster.push_back( IndexTransitionCluster( indexTransition ) );
    });

    std::sort( vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), []( IndexTransitionCluster const & p1, IndexTransitionCluster const & p2 ){
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


void Clustering::points_clustering( void (Clustering::*check_point_zone_function)(int) )
{
    for( int indexX = 0; indexX < vIndexTransitionCluster.size(); ++indexX )
    {
        (this->*check_point_zone_function)(indexX);
    }

    remove_small_clusters_and_noise();
}

void Clustering::check_point_zone_linear( int indexX )
{
    std::vector<uint> linkedClusters;
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
    linkedClusters.push_back(currentClusterNumber);

    double minX = point.row - eps;
    double maxX = point.row + eps;
    for( int i = 0; i < vIndexTransitionCluster.size(); ++i )
    {
        if( vIndexTransitionCluster[i].row < minX)
            continue;
        else
        {
            if( maxX < vIndexTransitionCluster[i].row )
            {
                break;
            }
            else
            {
                if (distance_function(point, vIndexTransitionCluster[i]) <= eps )
                {
                    if( vIndexTransitionCluster[i].clusterNumber == 0 )
                    {
                        vIndexTransitionCluster[i].clusterNumber = currentClusterNumber;
                    }
                    else
                    {
                        linkedClusters.push_back( vIndexTransitionCluster[i].clusterNumber );
                    }
                }
            }
        }
    }
    std::sort(linkedClusters.begin(),linkedClusters.end());
    linkedClusters.erase(std::unique(linkedClusters.begin(), linkedClusters.end()), linkedClusters.end());
    uint minClusterNumber = linkedClusters.front();

    linkedTransform.resize(std::max(linkedClusters.back() + 1, (uint)(linkedTransform.size())), 0);

    for_each(linkedClusters.begin(), linkedClusters.end(), [this, minClusterNumber]( uint cl){
        linkedTransform[cl] = std::min(linkedTransform[cl], minClusterNumber);
    });
        
    
    return;
}

void Clustering::remove_small_clusters_and_noise()
{
    auto maxClusterNumberIterator = std::max_element( vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), []( IndexTransitionCluster const & itc1, IndexTransitionCluster const & itc2){
        if(itc1.clusterNumber < itc2.clusterNumber )
        {
            return true;
        }
        else
        {
            return false;
        }
    });
    uint maxClusterNumber = maxClusterNumberIterator->clusterNumber;

    std::vector<uint>clusterOccurences(maxClusterNumber + 1, 0);
    for_each( vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), [&clusterOccurences]( IndexTransitionCluster point){
        if( point.clusterNumber != 0 )// new point without cluster occurrrence
        {
            clusterOccurences[point.clusterNumber] += 1;
        }
    });
    
    vIndexTransitionCluster.erase(std::remove_if(vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), [this, &clusterOccurences](IndexTransitionCluster const & itc){//remove small clusters
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

double Distance::distance_fast( IndexTransitionCluster const & pixelA, IndexTransitionCluster const & pixelB )
{
    return abs( pixelA.row - pixelB.row ) + abs( pixelA.col - pixelB.col );
}


double Distance::distance_slow( IndexTransitionCluster const & pixelA, IndexTransitionCluster const & pixelB )
{
    return sqrt( pow( ( pixelA.row - pixelB.row ), 2.0 ) + pow( ( pixelA.col - pixelB.col ), 2.0 ) );
}
