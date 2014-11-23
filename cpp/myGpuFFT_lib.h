extern int  OpenMyGpuFFT(int SetNumberOfDataPointLog2,int jobs);
extern void CloseMyGpuFFT(void);
extern int  DoMyGpuFFT(float ** input, float ** output, int  DoTheSquareRoot);
extern int NumberOfDataPoint;
extern int jobs;
