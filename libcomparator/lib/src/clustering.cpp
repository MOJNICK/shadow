#include "clustering.hpp"

IndexTransitionClusterVec::IndexTransitionClusterVec( std::vector<IndexTransition> vIndexTransition )
{

    std::for_each(vIndexTransition.begin(), vIndexTransition.end(), [ this ]( IndexTransition const & indexTransition ){
        vPointCluster.push_back(IndexTransitionCluster(indexTransition, 0));
    });
}

Clustering::Clustering( std::vector<IndexTransition> vIndexTransition, double (*distance_function)(cv::Point_<uint>, cv::Point_<uint>), double eps, uint minPts) :
clusters(vIndexTransition), eps{eps}, minPts{minPts}, nowClusterNumber{0}
{
    //""npArray with only possitive detection points npArray=[[pixelX, pixelY]]
    //dist_function must be a vector norm
    //eps - maximum point to point distance to belong to cluster
    //minPts minimum points amount to make new cluster
    //npArray = npArray[npArray[:, 0].argsort()];  // sort by first column
}

Clustering::create_new_cluster()
{
    nowClusterNumber += 1;
    return nowClusterNumber;
}

Clustering::get_cluster_number()
{
    return nowClusterNumber;
}


Clustering::points_clustering( check_point_zone_function )
    #   returns npArray: [pixelX, pixelY, clusterNumber]
{
//    if (self.clusters.shape[1] != 3)
//        throw ValueError("Passed array is not the right shape")

    for indexX in xrange(self.clusters.shape[0]):
        check_point_zone_function(indexX)

    self.remove_small_clusters_and_noise()
}

Clustering::check_point_zone_linear(self, indexX)
{
    point = self.clusters[indexX]
    if point[2] == 0:
        currentClusterNumber = self.create_new_cluster()
        point[2] = currentClusterNumber
    else:
        currentClusterNumber = point[2]

    minX = point[0] - self.eps
    maxX = point[0] + self.eps

    for i in xrange(0, self.clusters.shape[0]):#all elements
        if self.clusters[i, 0] < minX:
            continue
        else:
            if maxX < self.clusters[i, 0]:
                break
            else:
                if self.distance_function(point, self.clusters[i]) <= self.eps:
                    self.clusters[i, 2] = currentClusterNumber
}

Clustering::remove_small_clusters_and_noise(self)
{
    maxClusterNumber = np.max(self.clusters[:,2])
    clusterOccurences = np.zeros(maxClusterNumber + 1, dtype=np.int)

    for( point in self.clusters )
        if( point[2] == 0 )
            continue;
        else
            clusterOccurences[point[2]] += 1;

    for i in xrange(clusterOccurences.shape[0])
    {
        if( clusterOccurences[i] < self.minPts )
            self.clusters = self.clusters[(self.clusters[:, 2]!=i)];
    }
}

static double distance_fast(pixelA, pixelB)
{
    return abs(pixelA[0] - pixelB[0]) + abs(pixelA[1] - pixelB[1])
}


static double distance_slow(pixelA, pixelB)
{
    return math.sqrt(math.pow((pixelA[0] - pixelB[0]), 2) + math.pow((pixelA[1] - pixelB[1]), 2))
}
