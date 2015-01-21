# Compilation (add flags as needed)
CXXFLAGS    += `pkg-config opencv --cflags`
CXXFLAGS +=  -I/usr/local/include/raspicam/

# Linking (add flags as needed)
LDFLAGS     += `pkg-config opencv --libs`
LDFLAGS += -L/usr/local/lib/
LDFLAGS += -lraspicam -lraspicam_cv

# Name your target executables here
netcv: netcvc netcvs piSender
# udpClient udpServer
clean:
	$(RM) netcvc netcvs udpClient udpServer
