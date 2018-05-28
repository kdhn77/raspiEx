#define BUF_SIZE 1024
#define SHM_SIZE  BUF_SIZE+8
    //[0] : flag
   // [1:1024] : data
  // [1025:1027] : empty
  // [1028:1031] : copy_size (integer)
#define SHM_KEY 0x1234
#define READ_FLAG 1
#define WRITE_FLAG 2
#define BREAK_FLAG 3
