rm -rf video-sentinel
git clone git@github.com:mpohl100/video-sentinel-cpp.git temp-clone
cp -r temp-clone/src/ video-sentinel/
rm -rf temp-clone
cd video-sentinel
find . -type f -name "CMakeLists.txt" -exec sed -i 's|\${CMAKE_SRC_DIR}/src|\${CMAKE_SRC_DIR}/video-sentinel|g' {} +
cd ..