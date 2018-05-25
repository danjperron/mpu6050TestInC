S = /opt/vc/src/hello_pi/hello_fft/hex/shader_256.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_512.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_1k.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_2k.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_4k.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_8k.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_16k.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_32k.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_64k.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_128k.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_256k.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_512k.hex \
    /opt/vc/src/hello_pi/hello_fft/hex/shader_1024k.hex

C = /opt/vc/src/hello_pi/hello_fft/mailbox.c \
    /opt/vc/src/hello_pi/hello_fft/gpu_fft.c \
    /opt/vc/src/hello_pi/hello_fft/gpu_fft_base.c \
    /opt/vc/src/hello_pi/hello_fft/gpu_fft_twiddles.c \
    /opt/vc/src/hello_pi/hello_fft/gpu_fft_shaders.c

CO = mailbox.o gpu_fft.o gpu_fft_base.o gpu_fft_twiddles.o gpu_fft_shaders.o myGpuFFT_lib.o



F = -lbcm_host  -lrt -lm -I /opt/vc/src/hello_pi/hello_fft/ -L/opt/vc/lib/ -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux



all:		MPU6050 



myGpuFFT_lib.o:	myGpuFFT_lib.c
	gcc -c -o myGpuFFT_lib.o $(F) myGpuFFT_lib.c

mailbox.o:	/opt/vc/src/hello_pi/hello_fft/mailbox.c
	gcc -c -o mailbox.o $(F) /opt/vc/src/hello_pi/hello_fft/mailbox.c

gpu_fft.o:	/opt/vc/src/hello_pi/hello_fft/gpu_fft.c
	gcc -c -o gpu_fft.o $(F) /opt/vc/src/hello_pi/hello_fft/gpu_fft.c

gpu_fft_base.o:	/opt/vc/src/hello_pi/hello_fft/gpu_fft_base.c
	gcc -c -o gpu_fft_base.o $(F) /opt/vc/src/hello_pi/hello_fft/gpu_fft_base.c

gpu_fft_twiddles.o:	/opt/vc/src/hello_pi/hello_fft/gpu_fft_twiddles.c
	gcc -c -o gpu_fft_twiddles.o $(F) /opt/vc/src/hello_pi/hello_fft/gpu_fft_twiddles.c

gpu_fft_shaders.o:	/opt/vc/src/hello_pi/hello_fft/gpu_fft_shaders.c
	gcc -c -o gpu_fft_shaders.o $(F) /opt/vc/src/hello_pi/hello_fft/gpu_fft_shaders.c 

myGpuFFT_lib.a:  $(S) $(CO)
	ar rcs myGpuFFT_lib.a  $(CO)



MPU6050:	myGpuFFT_lib.a I2CWrapper.o MPU6050.cpp I2CWrapper.cpp
	g++ -lrt -o MPU6050 MPU6050.cpp I2CWrapper.cpp myGpuFFT_lib.a -ldl


clean:
	rm -f *.bin
	rm -f *.o
	rm -f myGpuFFT_lib.a
	rm -f MPU6050
