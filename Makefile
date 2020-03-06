
LIBS = -lopencv_imgcodecs -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lopencv_features2d

.PHONY:all clean

all:camera

clean:
	rm -f *.bin


camera:videocapture_camera.cpp mylib.cpp
	g++ $^ ${LIBS} -o $@.bin
