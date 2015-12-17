cd ../../libs/boost/boost_1_55_0
echo "Building boost..."
./bootstrap.sh --with-libraries=system,filesystem
./b2
echo "Done building boost"
cd ../../../src/build
