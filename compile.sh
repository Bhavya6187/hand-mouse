#g++ live.cpp -lcv -lcvaux -lhighgui -I /usr/include/opencv/

CC="g++ -g"
CCFLAGS="  -O2 "

INCLUDE_PATHS=" -I /usr/include/opencv/"

LD_FLAGS=" -lcv -lcvaux -lhighgui -lm -lX11 -lXtst "


TRACKING_OBJ="trackingCV/trackingCV.h   trackingCV/trackingCV.cpp"


#MOUSE_MOVEMENTS = 


${CC} ${CCFLAGS} -o live live.cpp  ${INCLUDE_PATHS} ${LD_FLAGS}


