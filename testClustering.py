import unittest
import clustering
import numpy as np

class TestClustering(unittest.TestCase):

    def setUp(self):
        self.siz0 = 10
        self.siz1 = 2
        self.npArray = np.array([[1, 2], [4, 2], [11, 2], [12, 2], [9, 2], [6, 8], [8, 8], [2, 3], [3, 4], [5, 7]],
                                dtype=np.int)
        self.eps = 2
        self.minPts = 3
        self.objectClustering = clustering.Clustering(self.npArray, clustering.Distance.distance_fast, self.eps, self.minPts)


    def test_init_clustering(self):
        self.assertEqual(self.objectClustering.clusters.shape, (self.siz0, self.siz1 + 1), 'missed size')
        np.testing.assert_equal(self.objectClustering.clusters, np.array([[1, 2, 0], [2, 3, 0], [3, 4, 0], [4, 2, 0], [5, 7, 0],
                                                                          [6, 8, 0], [8, 8, 0], [9, 2, 0], [11, 2, 0], [12, 2, 0]], dtype=np.int), 'not sorted or missing cluster number')


    def test_remove_small_clusters_and_noise_first(self):
        self.objectClustering.clusters[1:4,2] = 1
        self.objectClustering.clusters[6:9, 2] = 6
        self.objectClustering.remove_small_clusters_and_noise()
        np.testing.assert_equal(self.objectClustering.clusters, np.array([[2, 3, 1], [3, 4, 1], [4, 2, 1],
                                                                         [8, 8, 6], [9, 2, 6], [11, 2, 6]], dtype=np.int), 'not removing correctly')


    def test_remove_small_clusters_and_noise_second(self):
        self.objectClustering.clusters[1:4,2] = 1
        self.objectClustering.clusters[7:9, 2] = 6
        self.objectClustering.remove_small_clusters_and_noise()
        np.testing.assert_equal(self.objectClustering.clusters, np.array([[2, 3, 1], [3, 4, 1], [4, 2, 1]], dtype=np.int), 'not removing correctly')


    def test_remove_small_clusters_and_noise_third(self):
        self.objectClustering.minPts = 12
        self.objectClustering.clusters[1:4,2] = 1
        self.objectClustering.clusters[6:9, 2] = 6
        self.objectClustering.remove_small_clusters_and_noise()
        np.testing.assert_equal(self.objectClustering.clusters, np.empty([0, 3], dtype=np.int), 'problem with no big cluster')


    def test_remove_small_clusters_and_noise_fourth(self):
        self.objectClustering.minPts = 12
        self.objectClustering.remove_small_clusters_and_noise()
        np.testing.assert_equal(self.objectClustering.clusters, np.empty([0, 3], dtype=np.int), 'problem with no cluster')


    def test_point_zone_linear(self):
        self.objectClustering.check_point_zone_linear(2)
        print(self.objectClustering.clusters)
        np.testing.assert_equal(self.objectClustering.clusters, np.array([[1, 2, 0], [2, 3, 1], [3, 4, 1], [4, 2, 0], [5, 7, 0],
                                                                          [6, 8, 0], [8, 8, 0], [9, 2, 0], [11, 2, 0], [12, 2, 0]], dtype=np.int), '')
