# This script builds space-invaders, and then runs it on the scene with the
# given name. Any `*.xml` file in the `scenes/` directory can be used
# for this puropose.

cd src
mkdir -p build
cd build
cmake ..
make -j2
cd ../..

if test "1" -eq "$#"; then
    cd scenes
    ../src/build/space-invaders $@ && echo "Thanks for playing!"
    cd ..
else
    echo "Expected exactly one input argument. Sorry :/"
    echo
    echo "This script builds space-invaders, and then runs it on the scene with the given name. Any '.xml' file in the 'scenes/' directory will do."
    echo "For example, you could give '$0 space-invaders.xml' a try."
fi
