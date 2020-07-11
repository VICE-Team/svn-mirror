FROM fedora:33
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
	yum install mingw64-gcc -y && \
	yum install mingw64-gcc-c++ -y && \
	yum install mingw64-gtk3  -y && \
	yum install mingw64-gtkmm30 -y && \
	yum install mingw64-glew -y && \
	yum install mingw64-xz -y && \
	yum install mingw64-librsvg2 -y && \
	yum install cmake -y && \
	yum install make -y && \
	yum install git -y && \
	yum install zip -y && \
	yum install glib2-devel -y && \
	chmod +x /build-vice.sh
#USER nobody
