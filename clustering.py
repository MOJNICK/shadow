import cv2
import numpy as np
import math

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
        return ++self.nowClusterNumber

    def get_cluster_number(self):
        return self.nowClusterNumber

    def binary_clustering(self, binaryImage):
        # ""binaryImage==npArray with 1 as positive detection, 0 negative
        #   returns npArray: [clusterNumber, [pixelX, pixelY]]
        # ""
        pass

    def points_clustering(self, check_point_zone_function):
        #   returns npArray: [pixelX, pixelY, clusterNumber]
        if self.clusters.shape[1] != 3:
            raise ValueError("Passed array is not of the right shape")

        for indexX in xrange(self.clusters.shape[0]):
            check_point_zone_function(self, indexX)

        self.remove_small_clusters()



    def check_point_zone_linear(self, indexX):
        point = self.clusters[indexX]
        if point[2] == 0:
            currentClusterNumber = self.create_new_cluster()
            point[2] = currentClusterNumber
        else:
            currentClusterNumber = point[2]

        minX = point[0] - self.eps
        maxX = point[0] + self.eps
        minY = point[1] - self.eps
        maxY = point[1] + self.eps
        iStart = 0
        for i in xrange(iStart, self.clusters.shape[0]):#all elements
            if self.clusters[i, 0] > maxX | self.clusters[i, 0] < minX:
                break
            else:
                if self.distance_function(point, self.clusters[i]) < self.eps:
                    point[2] = currentClusterNumber

    def remove_small_clusters(self):
        lastClusterNumber = 0
        for point in self.clusters:
            if point[2] == 0:
                continue
            else:
                if last

class Distance:
    @staticmethod
    def distance_fast(self, pixelA, pixelB):
        # """"fast sum, sqrt error""""
        return abs(pixelA[0] - pixelB[0]) + abs(pixelA[1] - pixelB[1])

    @staticmethod
    def distance_slow(self, pixelA, pixelB):
        """euclidean"""
        return math.sqrt(math.pow((pixelA[0] - pixelB[0]), 2) + math.pow((pixelA[1] - pixelB[1]), 2))
