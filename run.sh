# This script builds space-invaders, and then runs it on the scene with the
# given name. Any `*.xml` file in the `scenes/` directory can be used
# for this puropose.

if test "1" -eq "$#"; then
    cd src
    mkdir build -p
    cd build
    cmake ..
    make -j2
    cd ../../scenes
    ../src/build/space-invaders $@
    cd ..
    echo "Thanks for playing!"
else
    echo "Expected exactly one input argument. Sorry :/"
    echo
    echo "This script builds space-invaders, and then runs it on the scene with the given name. Any '.xml' file in the 'scenes/' directory will do."
    echo "For example, you could give '$0 namespace-invaders.xml' a try."
fi