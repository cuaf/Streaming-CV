# Compilation (add flags as needed)
CXXFLAGS    += `pkg-config opencv --cflags`

# Linking (add flags as needed)
LDFLAGS     += `pkg-config opencv --libs`

# Name your target executables here
netcv: netcvc netcvs
# udpClient udpServer
clean: 
	$(RM) netcvc netcvs udpClient udpServer
