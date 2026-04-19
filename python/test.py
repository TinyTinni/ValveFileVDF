import unittest
import vdf

class MyTest(unittest.TestCase):

    def test_readme(self):
        d = vdf.read('"vdf_file"{"key" "value"}')
        self.assertEqual(d["vdf_file"]["key"], "value")

    def test_read_file(self):
        d = vdf.read_file("DST_Manifest.acf")
        d = d["AppState"]
        self.assertEqual(d["appid"], "343050")
        self.assertEqual(d["UserConfig"], {})
        self.assertEqual(len(d["MountedDepots"]), 1)
        self.assertEqual(d["another attribute with fancy space"], "yay")   
        

if __name__ == "__main__":
    unittest.main()