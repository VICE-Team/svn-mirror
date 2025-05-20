# VICE GitHub Mirror - TheC64 Joystick Fix for macOS

This is a fork of the official VICE git mirror of the VICE subversion repo, with specific fixes for TheC64 joystick on macOS.

## Bug Fix
Added support for [THEC64 Joystick](https://retrogames.biz/products/thejoystick/) on macOS, fixing [bug #1785](https://sourceforge.net/p/vice-emu/bugs/1785/). The joystick now works properly with both directional controls and fire buttons on macOS systems.

## TL;DR Instructions for Gamers

While waiting for VICE to be officially patched, here's what you can do to get your TheC64 joystick working on macOS:

### Step 1: Clone the repository
```
git clone -b thec64-joystick-macos-fix https://github.com/alrighdee/VICE-C64-svn-mirror.git
cd VICE-C64-svn-mirror
```

### Step 2: Build it
```
# Install required dependencies
brew install automake autoconf pkgconfig gtk+3 glew sdl2 giflib lame libpng libjpeg readline flac

# Build VICE
cd vice
./autogen.sh
mkdir -p build-gtk3
cd build-gtk3
../configure --enable-native-gtk3ui --disable-sdlui --enable-macosx-bundle
make -j$(sysctl -n hw.ncpu)
```

### Step 3: Run it
```
cd src
./x64sc
```

That's it! TheC64 joystick should now work perfectly with VICE on your Mac.
Making a test change
