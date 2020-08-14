import datetime

from ecl.util.util import BoolVector
from ecl.util.test import TestAreaContext
from tests import ResTest
from res.enkf import ObsBlock



class ObsBlockTest(ResTest):


    def test_create(self):
        block = ObsBlock("OBS" , 1000)
        self.assertTrue( isinstance( block , ObsBlock ))
        self.assertEqual( 1000 , block.totalSize())
        self.assertEqual( 0 , block.activeSize())


    def test_access(self):
        obs_size = 10
        block = ObsBlock("OBS" , obs_size)

        with self.assertRaises(IndexError):
            block[100] = (1,1)

        with self.assertRaises(IndexError):
            block[-100] = (1,1)

        with self.assertRaises(TypeError):
            block[4] = 10

        with self.assertRaises(TypeError):
            block[4] = (1,1,9)

        #------

        with self.assertRaises(IndexError):
            v = block[100]

        with self.assertRaises(IndexError):
            v = block[-100]

        block[0] = (10,1)
        v = block[0]
        self.assertEqual( v , (10,1))
        self.assertEqual( 1 , block.activeSize())

        block[-1] = (17,19)
        self.assertEqual( block[-1], (17,19))

    def test_is_active(self):
        obs_size = 10
        block = ObsBlock("OBS" , obs_size)
        self.assertEqual(obs_size, block.totalSize())
        self.assertEqual(0, block.activeSize())
        for i in range(block.totalSize()):
            self.assertFalse(block.is_active(i))

        active_indexes = [2, 5, 8, 9]
        for index in active_indexes:
            block[index] = (10*index, index)
        self.assertEqual(obs_size, block.totalSize())
        self.assertEqual(len(active_indexes), block.activeSize())

        for i in active_indexes:
            self.assertTrue(block.is_active(i))

        for i in set(range(block.totalSize())) - set(active_indexes):
            self.assertFalse(block.is_active(i))


    def test_obs_key(self):
        obs_size = 10
        block = ObsBlock("OBS" , obs_size)
        self.assertEqual("OBS", block.get_obs_key())
