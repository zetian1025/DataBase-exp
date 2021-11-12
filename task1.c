#include "utils.h"

void SearchInLinear(Buffer *buf, unsigned int addr_w){
    unsigned char *blk_r; /* A pointer to a block */
    unsigned char *blk_w; /* A pointer to a block */
    blk_data data;

    blk_w = GetNewBlockInBuffer(buf);

    int addr_r = 17;
    int idx = 0;
    while (addr_r < 49) {
        blk_r = ReadBlockFromDisk(addr_r, buf);
        GetDataFromBlk(blk_r, &data);
        for (int i=0; i<7; i++) {
            if(data.x[i] == 130) {
                PutDataToBlk(blk_w, idx, data.x[i], data.y[i]);
                if (++idx == 7) {
                    PutDataToBlk(blk_w, idx, addr_w+1, 0);
                    WriteBlockToDisk(blk_w, addr_w, buf);
                    addr_w++;
                    idx = 0;
                }
            }
        }
        addr_r = data.addr;
        FreeBlockInBuffer(blk_r, buf);
    }
    if (idx > 0) {
        WriteBlockToDisk(blk_w, addr_w, buf);
    }
}

int main(){
    Buffer buf; /* A buffer */
    InitBuffer(BUFSIZE, BLKSIZE, &buf);

    SearchInLinear(&buf, 8888);
    
    printf("IO's is %d\n", buf.numIO); /* Check the number of IO's */
    ListBlockFromDisk(8888, &buf);
    return 0;
}