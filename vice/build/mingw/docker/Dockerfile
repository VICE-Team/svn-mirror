FROM fedora:latest
WORKDIR /vice
COPY build-vice.sh /
RUN yum install gcc -y && \
    	yum install yasm -y && \
    	yum install gcc-c++ -y && \
    	yum install automake -y && \
    	yum install svn -y && \
    	yum install flex -y && \
    	yum install bison -y && \
    	yum install xa -y && \
    	yum install texinfo -y && \
    	yum install texinfo-tex -y && \
	yum install diffutils -y && \
	yum install dos2unix -y && \
	yum install mingw32-binutils -y && \
	yum install mingw32-gcc -y && \
	yum install mingw32-gcc-c++ -y && \
	yum install mingw32-gtk3  -y && \
	yum install mingw32-gtkmm30 -y && \
	yum install mingw32-glew -y && \
	yum install mingw32-xz -y && \
	yum install mingw32-librsvg2 -y && \
 	yum install mingw32-curl -y && \
	yum install mingw32-flac -y && \
	yum install mingw32-portaudio -y && \
	yum install mingw32-giflib.noarch -y && \
	yum install mingw32-libvorbis.noarch -y && \
	yum install mingw64-binutils -y && \
	yum install mingw64-gcc -y && \
	yum install mingw64-gcc-c++ -y && \
	yum install mingw64-gtk3  -y && \
	yum install mingw64-gtkmm30 -y && \
	yum install mingw64-glew -y && \
	yum install mingw64-xz -y && \
	yum install mingw64-librsvg2 -y && \
 	yum install mingw64-curl -y && \
	yum install mingw64-flac -y && \
	yum install mingw64-portaudio -y && \
	yum install mingw64-giflib.noarch -y && \
	yum install mingw64-libvorbis.noarch -y && \
	yum install icoutils -y && \
	yum install cmake -y && \
	yum install make -y && \
	yum install git -y && \
	yum install zip -y && \
	yum install glib2-devel -y && \
	chmod +x /build-vice.sh
#USER nobody
