#include "clustering.hpp"

Clustering::Clustering( std::vector<IndexTransition> const & vIndexTransition, double (*distance_function)(IndexTransitionCluster const &, IndexTransitionCluster const &), double eps, uint minPts):
eps{eps}, minPts{minPts}, nowClusterNumber{0}, distance_function{distance_function}
{
    std::for_each(vIndexTransition.begin(), vIndexTransition.end(), [ this ]( IndexTransition const & indexTransition ){
        vIndexTransitionCluster.push_back( IndexTransitionCluster( indexTransition ) );
    });

    std::stable_sort( vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), []( IndexTransitionCluster const & p1, IndexTransitionCluster const & p2 ){
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
    if( linkedTransform.size() == 0 )
        return;

    concatenate_clusters();

    remove_small_clusters_and_noise();
}

void Clustering::check_point_zone_linear( int indexX )
{
    std::vector<uint> linkedClusters;
    uint currentClusterNumber = 0;
    IndexTransitionCluster point = vIndexTransitionCluster[indexX];
    if( point.get_cluster_number() == 0 )
    {
        currentClusterNumber = create_new_cluster();
        point.set_cluster_number( currentClusterNumber );
    }
    else
    {
        currentClusterNumber = point.get_cluster_number();
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
                    if( vIndexTransitionCluster[i].get_cluster_number() == 0 )
                    {
                        vIndexTransitionCluster[i].set_cluster_number(currentClusterNumber);
                    }
                    else
                    {
                        linkedClusters.push_back( vIndexTransitionCluster[i].get_cluster_number() );
                    }
                }
            }
        }
    }
    std::sort(linkedClusters.begin(),linkedClusters.end());
    linkedClusters.erase(std::unique(linkedClusters.begin(), linkedClusters.end()), linkedClusters.end());    

    linkedTransform.resize(std::max(linkedClusters.back() + 1, (uint)(linkedTransform.size())), UINT32_MAX);//UINT32_MAX is one to one map (ex. if linkedTransform[2]==UINT32_MAX then cluster 2 maped to cluster2)

    uint minLinkedTransformByLinkedClusters = linkedClusters.front();//only for start, could be back() or what ever
    std::for_each( linkedClusters.begin(), linkedClusters.end(), [ this, &minLinkedTransformByLinkedClusters ]( uint cl){
        minLinkedTransformByLinkedClusters = std::min( minLinkedTransformByLinkedClusters, linkedTransform[cl] );
    });

    std::for_each( linkedClusters.begin(), linkedClusters.end(), [ this, &minLinkedTransformByLinkedClusters ]( uint cl){
        uint oldValue = linkedTransform[cl];
        std::replace( linkedTransform.begin() + 1, linkedTransform.end(), oldValue, minLinkedTransformByLinkedClusters);// +1 reserved element [0]
    }); 

    return;
}

void Clustering::concatenate_clusters()
{
    for(uint i = 0; i < linkedTransform.size(); ++i)//zeroes is one to one map (ex. if linkedTransform[2]==0 then cluster 2 maped to cluster 2)
    {
        if(linkedTransform[i] == UINT32_MAX)
        {
            linkedTransform[i] = i;
        }
    }

//raw cluster numbers or not code below
    std::vector<uint> distinctClusters(linkedTransform);
    std::sort(distinctClusters.begin(), distinctClusters.end());
    distinctClusters.erase(std::unique(distinctClusters.begin(), distinctClusters.end()), distinctClusters.end());

    std::vector<uint> transLinkedTransform(distinctClusters.back() + 1, 0);
    for(int i = 0; i < distinctClusters.size(); ++i)
    {
        transLinkedTransform[distinctClusters[i]] = i;
    }
    std::for_each(linkedTransform.begin(), linkedTransform.end(), [&transLinkedTransform](auto& lt){
        lt = transLinkedTransform[lt];
    });
    

    std::for_each(vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), [this](auto& itc){
        itc.set_cluster_number(linkedTransform[ itc.get_cluster_number() ]);
    });
}

void Clustering::remove_small_clusters_and_noise()
{
    auto maxClusterNumberIterator = std::max_element( vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), []( IndexTransitionCluster const & itc1, IndexTransitionCluster const & itc2){
        if( itc1.get_cluster_number() < itc2.get_cluster_number() )
        {
            return true;
        }
        else
        {
            return false;
        }
    });
    uint maxClusterNumber = maxClusterNumberIterator->get_cluster_number();

    std::vector<uint>clusterOccurences(maxClusterNumber + 1, 0);
    for_each( vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), [&clusterOccurences]( IndexTransitionCluster const & itc ){
        if( itc.get_cluster_number() != 0 )// new itc without cluster occurrrence
        {
            clusterOccurences[itc.get_cluster_number()] += 1;
        }
    });
    
    vIndexTransitionCluster.erase(std::remove_if(vIndexTransitionCluster.begin(), vIndexTransitionCluster.end(), [this, &clusterOccurences](IndexTransitionCluster const & itc){//remove small clusters
        if( clusterOccurences[itc.get_cluster_number()] < minPts )
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