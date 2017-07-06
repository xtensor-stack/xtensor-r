xtensor_version="0.10.6"

# From build directory
rm -rf build
mkdir build
cd build

# Move the R-package assets
cp -r ../R-package ./

# Build and install xtensor
git clone https://github.com/QuantStack/xtensor.git
cd xtensor
git checkout $xtensor_version
cd ..
mkdir build_xtensor
cd build_xtensor
cmake -D CMAKE_INSTALL_PREFIX=../R-package/inst ../xtensor
make install
cd ..

# Build and install xtensor-r
mkdir build_xtensor_r
cd build_xtensor_r
cmake -D CMAKE_INSTALL_PREFIX=../R-package/inst ../..
make install
cd ..

