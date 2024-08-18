rm -rf video-sentinel
git clone git@github.com:mpohl100/video-sentinel-cpp.git temp-clone
cp -r temp-clone/src/ video-sentinel/
rm -rf temp-clone