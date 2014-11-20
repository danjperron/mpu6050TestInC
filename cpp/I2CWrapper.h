class I2CWrapper
{
 public:
            I2CWrapper();
            ~I2CWrapper();
            int handle;
            int FailedFlag;
            int Open(int BUS,int SlaveAddress);
            void Close(void);
            int SetSlaveAddress(int SlaveAddress);
            int ReadBlock(unsigned char cmd, unsigned char size, void * array);
            unsigned char  ReadByte(unsigned char cmd);
            unsigned short ReadWord(unsigned char cmd);
            int WriteByte(unsigned char cmd, unsigned char value);
            int WriteWord(unsigned char cmd, unsigned short value);
};

