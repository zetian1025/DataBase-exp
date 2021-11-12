#include "utils.h"

int count = 0;

int cmp(int a, int b, int c, int d) {
    if (a == b && c == d) {
        return -1;
    }
    return (a < b) || ((a == b) && (c < d));
}

void Union(Buffer *buf, unsigned int src_r, unsigned int src_s, unsigned int dst) {
    unsigned char * blk = GetNewBlockInBuffer(buf);
    blk_data data_s, data_r, data_w;
    blk = ReadDataFromDisk(blk, &data_s, src_s, buf);
    blk = ReadDataFromDisk(blk, &data_r, src_r, buf);
    src_s = data_s.addr;
    src_r = data_r.addr;

    int idx_s, idx_r, idx_w;
    idx_s = idx_r = idx_w = 0;

    while (idx_s < 7 && idx_r < 7) {
        int res = cmp(data_r.x[idx_r], data_s.x[idx_s], data_r.y[idx_r], data_s.y[idx_s]);
        if (res == -1) {
            data_w.x[idx_w] = data_r.x[idx_r];
            data_w.y[idx_w] = data_r.y[idx_r];
            idx_w++;
            idx_r++;
            idx_s++;
            count++;
        }
        else if (res == 0) {
            data_w.x[idx_w] = data_s.x[idx_s];
            data_w.y[idx_w] = data_s.y[idx_s];
            idx_w++;
            idx_s++;
            count++;
        }
        else {
            data_w.x[idx_w] = data_r.x[idx_r];
            data_w.y[idx_w] = data_r.y[idx_r];
            idx_w++;
            idx_r++;
            count++;
        }

        if (idx_s == 7 && src_s != 0) {
            blk = ReadDataFromDisk(blk, &data_s, src_s, buf);
            src_s = data_s.addr;
            idx_s = 0;
        }
        if (idx_r == 7 && src_r != 0) {
            blk = ReadDataFromDisk(blk, &data_r, src_r, buf);
            src_r = data_r.addr;
            idx_r = 0;
        }
        if (idx_w == 7) {
            data_w.addr = dst+1;
            blk = WriteDataToDisk(blk, &data_w, dst++, buf);
            idx_w = 0;
        }
    }
    while (idx_s < 7) {
        data_w.x[idx_w] = data_s.x[idx_s];
        data_w.y[idx_w] = data_s.y[idx_s];
        idx_s++;
        idx_w++;
        count++;
        if (idx_s == 7 && src_s != 0) {
            blk = ReadDataFromDisk(blk, &data_s, src_s, buf);
            src_s = data_s.addr;
            idx_s = 0;
        }
        if (idx_w == 7) {
            data_w.addr = dst+1;
            blk = WriteDataToDisk(blk, &data_w, dst++, buf);
            idx_w = 0;
        }
    }
    
    while (idx_r < 7) {
        data_w.x[idx_w] = data_r.x[idx_r];
        data_w.y[idx_w] = data_r.y[idx_r];
        idx_r++;
        idx_w++;
        count++;
        if (idx_r == 7 && src_r != 0) {
            blk = ReadDataFromDisk(blk, &data_r, src_r, buf);
            src_r = data_r.addr;
            idx_r = 0;
        }
        if (idx_w == 7) {
            data_w.addr = dst+1;
            blk = WriteDataToDisk(blk, &data_w, dst++, buf);
            idx_w = 0;
        }
    }

    if (idx_w != 0) {
        data_w.addr = 0;
        blk = WriteDataToDisk(blk, &data_w, dst, buf);
    }
    else {
        blk = ReadDataFromDisk(blk, &data_w, dst-1, buf);
        data_w.addr = 0;
        blk = WriteDataToDisk(blk, &data_w, dst-1, buf);
    }

    FreeBlockInBuffer(blk, buf);
}

int main(){
    Buffer buf;
    InitBuffer(BUFSIZE, BLKSIZE, &buf);
    Union(&buf, 301, 317, 801);
    printf("[COUNT] %d\n", count);
}