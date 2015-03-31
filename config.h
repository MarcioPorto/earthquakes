#ifndef config_h
#define config_h

#include <string>


// This variable isn't used outside this file, but makes specifying the other
// paths simpler.
const G3D::String DATA_DIR = "/Users/mporto/Documents/Graphics/EarthquakeStudent/data/";

// The file to load for the earth texture.
const G3D::String TEXTURE_PATH = DATA_DIR + "earth-2k.jpg";

// The earthquake database file
const G3D::String DATA_PATH = DATA_DIR + "earthquakes.txt";


#endif