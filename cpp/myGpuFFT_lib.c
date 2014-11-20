/*
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "mailbox.h"
#include "gpu_fft.h"

int mbox_handle;
int   NumberOfDataPoint;
int   NumberOfDataPointLog2;
float fNumberOfDataPoint;
float fHalfDataPoint;

struct GPU_FFT_COMPLEX * base;
struct GPU_FFT *fft;


int OpenMyGpuFFT(int SetNumberOfDataPointLog2)
{
  mbox_handle= mbox_open();
  if(mbox_handle>=0)
   {
  NumberOfDataPointLog2= SetNumberOfDataPointLog2;
  NumberOfDataPoint = 1 << NumberOfDataPointLog2;
  fNumberOfDataPoint = (float) NumberOfDataPoint;
  fHalfDataPoint = fNumberOfDataPoint / 2.0;

  int ret= gpu_fft_prepare(mbox_handle,NumberOfDataPointLog2, GPU_FFT_FWD,1,&fft);
  }
  return mbox_handle;
}

void CloseMyGpuFFT(void)
{
  if(mbox_handle<0) return;
  gpu_fft_release(fft);
  mbox_close(mbox_handle);
}



void DoMyGpuFFT(float * input, float * output, int  DoTheSquareRoot)
{
 int i;
 float ftemp;
 base = fft->in;


 for(i=0;i<NumberOfDataPoint;i++)
  {
   base[i].re =  input[i]; base[i].im =0.0;
  }

 gpu_fft_execute(fft);
 base = fft->out;

 //average value
 ftemp= (base[0].re * base[0].re) + (base[0].im * base[0].im);
 if(DoTheSquareRoot)
   ftemp= sqrt(ftemp) / fNumberOfDataPoint;
 output[0]= ftemp;

 // Get square/Convert frequency domain
 for(i=1;i<NumberOfDataPoint;i++)
   {
     ftemp= (base[i].re * base[i].re) + (base[i].im * base[i].im);
     if(DoTheSquareRoot)
       ftemp= sqrt(ftemp) / fHalfDataPoint;
     output[i]= ftemp;
   }
}

