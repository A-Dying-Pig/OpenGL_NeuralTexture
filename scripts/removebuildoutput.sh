cd ..
if [ -d "build" ]; then
  rm -r build
fi
mkdir build

cd output
if [ -d "uv" ]; then
  rm -r uv
fi
mkdir uv

if [ -d "frame" ]; then
  rm -r frame
fi
mkdir frame

if [ -d "view_normal" ]; then
  rm -r view_normal
fi
mkdir view_normal