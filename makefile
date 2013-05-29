INCLUDES=-I /Developer/SDKs/MacOSX10.5.sdk/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers

CXXFLAGS=$(INCLUDES) -m64
OBJS=launch_chk.o launchd_find_socket.o socket_jni.o
project : liblaunchd.jnilib 


liblaunchd.jnilib : $(OBJS)
	g++ -m64 -dynamiclib -o liblaunchd.jnilib $(OBJS)  -framework JavaVM 


