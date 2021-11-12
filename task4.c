#include "utils.h"

int count = 0;

void SearchInLinear(Buffer *buf, unsigned int addr_r, unsigned int *addr_w, unsigned int x, unsigned int y){
    unsigned char *blk_r; /* A pointer to a block */
    unsigned char *blk_w; /* A pointer to a block */
    blk_data data_r;
    blk_data data_w;
    blk_w = GetNewBlockInBuffer(buf);

    int idx = 0;
    CleanData(&data_w);
    while (addr_r) {
        blk_r = ReadDataFromDisk(blk_r, &data_r, addr_r, buf);
        for (int i=0; i<7; i++) {
            if(data_r.x[i] == x) {
                count++;
                data_w.x[idx] = x;
                data_w.y[idx] = y;
                data_w.x[idx+1] = data_r.x[i];
                data_w.y[idx+1] = data_r.y[i];
                idx += 2;
                if (idx == 6) {
                    data_w.addr = (*addr_w) + 1;
                    blk_w = WriteDataToDisk(blk_w, &data_w, *addr_w, buf);
                    (*addr_w)++;
                    idx = 0;
                    CleanData(&data_w);
                }
            }
        }
        addr_r = data_r.addr;
        FreeBlockInBuffer(blk_r, buf);

        if (data_r.x[6] > x) {
            break;
        }
    }
    if (idx > 0) {
        data_w.addr = (*addr_w) + 1;
        WriteDataToDisk(blk_w, &data_w, *addr_w, buf);
        (*addr_w)++;
    }
    FreeBlockInBuffer(blk_w, buf);
}

void SearchBySorted(Buffer *buf, unsigned int *src, unsigned int *dst, unsigned int x, unsigned int y) {
    unsigned char * blk = GetNewBlockInBuffer(buf);
    blk_data data;
    int * addr = src;
    while (*addr) {
        blk = ReadDataFromDisk(blk, &data, *addr, buf);
        if (data.x[6] < x) {

        }
        else if (data.x[0] > x) {
            break;
        }
        else {
            SearchInLinear(buf, *addr, dst, x, y);
            break;
        }
        *addr = data.addr;
    }
    FreeBlockInBuffer(blk, buf);
}

void InnerJoin(Buffer *buf, unsigned int src_r, unsigned int src_s, unsigned int dst) {
    unsigned char * blk = GetNewBlockInBuffer(buf);
    blk_data data;
    int addr = src_r;
    while (addr) {
        blk = ReadDataFromDisk(blk, &data, addr, buf);
        for (int i=0; i<7; i++) {
            if (data.x[i] == 0) {
                continue;
            }
            SearchBySorted(buf, &src_s, &dst, data.x[i], data.y[i]);
        }
        addr = data.addr;
    }
    blk = ReadDataFromDisk(blk, &data, dst-1, buf);
    data.addr = 0;
    blk = WriteDataToDisk(blk, &data, dst-1, buf);
    FreeBlockInBuffer(blk, buf);
}

int main(){
    Buffer buf;
    InitBuffer(BUFSIZE, BLKSIZE, &buf);
    InnerJoin(&buf, 301, 317, 601);
    printf("[COUNT] %d\n", count);
    // ListBlockFromDisk(601, &buf);
}