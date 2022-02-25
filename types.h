#include <ac_fixed.h>

// dimensions of input volume
int X=416;
int Y=416;

// total layers=34
int LN=33; // number of convolutional layers
int PN=1; // number of pooling layers
int NumOfBlocks=16;

string kfn[LN]={"kernels01.txt","kernels02.txt","kernels03.txt","kernels04.txt","kernels05.txt","kernels06.txt","kernels07.txt","kernels08.txt","kernels09.txt","kernels10.txt","kernels11.txt","kernels12.txt","kernels13.txt","kernels14.txt","kernels15.txt","kernels16.txt","kernels17.txt","kernels18.txt","kernels19.txt","kernels20.txt","kernels21.txt","kernels22.txt","kernels23.txt","kernels24.txt","kernels25.txt","kernels26.txt","kernels27.txt","kernels28.txt","kernels29.txt","kernels30.txt","kernels31.txt","kernels32.txt","kernels33.txt"};
int kd[LN]={7,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
int kdepth[LN]={3,64,64,64,64,64,64,64,128,128,128,128,128,128,128,128,256,256,256,256,256,256,256,256,256,256,256,256,512,512,512,512,512};
int kn[LN]={64,64,64,64,64,64,64,128,128,128,128,128,128,128,128,256,256,256,256,256,256,256,256,256,256,256,256,512,512,512,512,512,512};
