#include "utils.h"

void create_index(Buffer *buf, unsigned int src, unsigned int dst){
    unsigned char *blk_r;
    unsigned char *blk_w = GetNewBlockInBuffer(buf);

    blk_data data_r;
    blk_data data_w;
    int curr_x = 0, curr_index = 0;
    int addr = src;
    while (addr){
        blk_r = ReadBlockFromDisk(addr, buf);
        GetDataFromBlk(blk_r, &data_r);
        if (curr_x != data_r.x[0]) {
            curr_x = data_r.x[0];
            data_w.x[curr_index] = data_r.x[0];
            data_w.y[curr_index] = addr;
            curr_index++;
            if (curr_index == 7) {
                curr_index = 0;
                data_w.addr = dst+1;
                WriteDataToDisk(blk_w, &data_w, dst, buf);
                CleanData(&data_w);
                dst++;
            }
        }
        addr = data_r.addr;
        FreeBlockInBuffer(blk_r, buf);
    }
    if (curr_index > 0) {
        WriteDataToDisk(blk_w, &data_w, dst, buf);
    }
}

void SearchInLinear(Buffer *buf, unsigned int addr_r, unsigned int addr_w){
    unsigned char *blk_r; /* A pointer to a block */
    unsigned char *blk_w; /* A pointer to a block */
    blk_data data_r;
    blk_data data_w;
    CleanData(&data_w);

    blk_w = GetNewBlockInBuffer(buf);

    int idx = 0;
    while (addr_r) {
        blk_r = ReadDataFromDisk(blk_r, &data_r, addr_r, buf);
        for (int i=0; i<7; i++) {
            if(data_r.x[i] == 130) {
                data_w.x[idx] = data_r.x[i];
                data_w.y[idx] = data_r.y[i];
                if (++idx == 7) {
                    data_w.addr = addr_w+1;
                    blk_w = WriteDataToDisk(blk_w, &data_w, addr_w, buf);
                    addr_w++;
                    idx = 0;
                }
            }
        }
        addr_r = data_r.addr;
        FreeBlockInBuffer(blk_r, buf);

        if (data_r.x[6] > 130) {
            break;
        }
    }
    if (idx != 0) {
        data_w.addr = 0;
        blk_w = WriteDataToDisk(blk_w, &data_w, addr_w, buf);
    }
    else {
        blk_r = ReadDataFromDisk(blk_r, &data_w, addr_w-1, buf);
        data_w.addr = 0;
        blk_w = WriteDataToDisk(blk_w, &data_w, addr_w-1, buf);
    }
    FreeBlockInBuffer(blk_w, buf);
}

void SearchByIndex(Buffer *buf, unsigned int src, unsigned int dst, int num) {
    unsigned char * blk = GetNewBlockInBuffer(buf);
    blk_data data_index;

    int addr = src;
    int addr_prev = 0;
    while (addr) {
        blk = ReadDataFromDisk(blk, &data_index, addr, buf);
        if (data_index.x[6] < num) {
            addr_prev = data_index.y[6];
        }
        else {
            if (data_index.x[0] >= num && addr_prev != 0) {
                SearchInLinear(buf, addr_prev, dst);
                break;
            }
            else if (data_index.x[0] > num) {
                break;
            }
            else {
                int index = 0;
                for (int i=0; i<7; i++) {
                    if (data_index.x[i] < num) {
                        index = i;
                    }
                }
                SearchInLinear(buf, data_index.y[index], dst);
                break;
            }
        }

        addr = data_index.addr;
        FreeBlockInBuffer(blk, buf);
    }
}

int main(){
    Buffer buf; /* A buffer */
    InitBuffer(BUFSIZE, BLKSIZE, &buf);

    create_index(&buf, 301, 401);
    create_index(&buf, 317, 417);
    ListBlockFromDisk(401, &buf);
    ListBlockFromDisk(417, &buf);

    InitBuffer(BUFSIZE, BLKSIZE, &buf);
    SearchByIndex(&buf, 417, 517, 130);
    printf("IO's is %d\n", buf.numIO); /* Check the number of IO's */
    ListBlockFromDisk(517, &buf);
}