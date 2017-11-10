import numpy as np
import math

class CApi:
    def __init__(self):
        pass

    # def call_points_clustering( self, eps, minPts, cPointer, length, pointerType ):
    #     npArray = self.uchar_pointer_to_np( cPointer, length, pointerType )
    #     clustering = Clustering( npArray, Distance.distance_fast, eps, minPts )
    #     clustering.points_clustering( clustering.check_point_zone_linear )
    #     clustering.npArray = clustering.npArray.reshape( 1, -1 )
    #     return 

    @staticmethod
    def call_points_clustering( self ):
        # eps = 4
        # minPts = 2
        # clustering = Clustering( npArray, Distance.distance_fast, eps, minPts )
        # clustering.points_clustering( clustering.check_point_zone_linear )
        print ("call_points_clustering====================================")
        return 13

    def uchar_pointer_to_np(self, cPointer, length, pointerType):
        npArray = np.fromiter(cPointer, np.int, length)
        npArray = npArray.reshape(length/2, 2)
        return npArray

capiObject = CApi()

class Clustering:

    def __init__(self, npArray, distance_function, eps, minPts):
        # ""npArray with only possitive detection points npArray=[[pixelX, pixelY]]
        #dist_function must be a vector norm
        #eps - maximum point to point distance to belong to cluster
        #minPts minimum points amount to make new cluster
        npArray = npArray[npArray[:, 0].argsort()]  # sort by first column
        self.nowClusterNumber = 0
        self.clusters = np.append(npArray, np.zeros((npArray.shape[0], 1), dtype=np.int), axis=1)
        self.distance_function = distance_function
        self.eps = eps
        self.minPts = minPts


    def create_new_cluster(self):
        self.nowClusterNumber += 1
        return self.nowClusterNumber


    def get_cluster_number(self):
        return self.nowClusterNumber


    def binary_clustering(self, binaryImage):
        # ""binaryImage==npArray with 1 as positive detection, 0 negative
        #   returns npArray: [clusterNumber, [pixelX, pixelY]]
        # ""
        pass


    def points_clustering(self, check_point_zone_function):
        #   returns npArray: [pixelX, pixelY, clusterNumber]
        # if self.clusters.shape[1] != 3:
        #     raise ValueError("Passed array is not the right shape")

        for indexX in xrange(self.clusters.shape[0]):
            check_point_zone_function(indexX)

        self.remove_small_clusters_and_noise()


    def check_point_zone_linear(self, indexX):
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


    def remove_small_clusters_and_noise(self):
        maxClusterNumber = np.max(self.clusters[:,2])
        clusterOccurences = np.zeros(maxClusterNumber + 1, dtype=np.int)

        for point in self.clusters:
            if point[2] == 0:
                continue
            else:
                clusterOccurences[point[2]] += 1

        for i in xrange(clusterOccurences.shape[0]):
            if clusterOccurences[i] < self.minPts:
                self.clusters = self.clusters[(self.clusters[:, 2]!=i)]



class Distance:
    @staticmethod
    def distance_fast(pixelA, pixelB):
        # """"fast sum, sqrt error""""
        return abs(pixelA[0] - pixelB[0]) + abs(pixelA[1] - pixelB[1])

    @staticmethod
    def distance_slow(pixelA, pixelB):
        """euclidean"""
        return math.sqrt(math.pow((pixelA[0] - pixelB[0]), 2) + math.pow((pixelA[1] - pixelB[1]), 2))
