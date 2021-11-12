#ifndef UTILS_H_
#define UTILS_H_

#define BUFSIZE (512)
#define BLKSIZE (64)

#include <stdlib.h>
#include <stdio.h>
#include "extmem.h"
#include "string.h"

typedef struct blk_data
{
    int x[7];
    int y[7];
    int addr;
}blk_data;

void CleanData(blk_data *data) {
    memset(data->x, 0, sizeof(data->x));
    memset(data->y, 0, sizeof(data->y));
    data->addr = 0;
}

unsigned char * ReadBlockFromDisk(unsigned int addr, Buffer *buf) {
    unsigned char *blk;
    if ((blk = readBlockFromDisk(addr, buf)) == NULL)
    {
        perror("Reading Block Failed!\n");
        exit(-1);
    }
    printf("[ReadBlockFromDisk] %d\n", addr);
    return blk;
}

void FreeBlockInBuffer(unsigned char *blk, Buffer *buf){
    return freeBlockInBuffer(blk, buf);
}

void ListData(blk_data *data) {
    for (int i=0; i<7; i++) {
        printf("[blk_data] %d %d\n", data->x[i], data->y[i]);
    }
    printf("[blk_data] addr: %d\n", data->addr);
}

void PutDataToBlk(unsigned char *blk, int idx, int x, int y){
    int a, b, c, d;
    a = (x%10000)/1000;
    b = (x%1000)/100;
    c = (x%100)/10;
    d = (x%10);
    *(blk + 8*idx + 0) = '0'+a;
    *(blk + 8*idx + 1) = '0'+b;
    *(blk + 8*idx + 2) = '0'+c;
    *(blk + 8*idx + 3) = '0'+d;
    a = (y%10000)/1000;
    b = (y%1000)/100;
    c = (y%100)/10;
    d = (y%10);
    *(blk + 8*idx + 4) = '0'+a;
    *(blk + 8*idx + 5) = '0'+b;
    *(blk + 8*idx + 6) = '0'+c;
    *(blk + 8*idx + 7) = '0'+d;
}

void GetDataFromBlk(unsigned char *blk, blk_data *data){
    char str[5];
    int i;
    for (i = 0; i < 7; i++)
    {
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk + i*8 + k);
        }
        (*data).x[i] = atoi(str);
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk + i*8 + 4 + k);
        }
        (*data).y[i] = atoi(str);
    }
    for (int k = 0; k < 4; k++)
    {
        str[k] = *(blk + i*8 + k);
    }
    (*data).addr = atoi(str);
}

unsigned char * GetNewBlockInBuffer(Buffer *buf)
{
    return getNewBlockInBuffer(buf);
}

void WriteBlockToDisk(unsigned char *blkPtr, unsigned int addr, Buffer *buf) {
    if (writeBlockToDisk(blkPtr, addr, buf) != 0)
    {
        perror("Writing Block Failed!\n");
        exit(-1);
    }
    printf("[WriteBlockToDisk] %d\n", addr);
}

unsigned char * WriteDataToDisk(unsigned char *blk, blk_data *data, unsigned int dst, Buffer *buf){
    blk_data temp_data;
    GetDataFromBlk(blk, &temp_data);
    for (int i=0; i<7; i++) {
        PutDataToBlk(blk, i, data->x[i], data->y[i]);
    }
    PutDataToBlk(blk, 7, data->addr, 0);
    WriteBlockToDisk(blk, dst, buf);
    blk = GetNewBlockInBuffer(buf);
    for (int i=0; i<7; i++) {
        PutDataToBlk(blk, i, temp_data.x[i], temp_data.y[i]);
    }
    PutDataToBlk(blk, 7, temp_data.addr, 0);
    CleanData(data);
    return blk;
}

unsigned char * ReadDataFromDisk(unsigned char *blk, blk_data *data, unsigned int src, Buffer *buf){
    FreeBlockInBuffer(blk, buf);    
    CleanData(data);
    blk = ReadBlockFromDisk(src, buf);
    GetDataFromBlk(blk, data);
    return blk;
}

void InitBuffer(size_t bufSize, size_t blkSize, Buffer *buf){
    if (!initBuffer(520, 64, buf))
    {
        perror("Buffer Initialization Failed!\n");
        exit(-1);
    }
}

void ListBlockFromDisk(unsigned int addr, Buffer *buf) {
    unsigned char * blk;
    blk_data data;
    while (addr) {
        blk = ReadBlockFromDisk(addr, buf);
        GetDataFromBlk(blk, &data);
        ListData(&data);
        addr = data.addr;
        FreeBlockInBuffer(blk, buf);
    }
}

void DropBlocksOnDisk(unsigned int addr, unsigned int range) {
    for (int i=0; i<range; i++) {
        dropBlockOnDisk(addr+i);
    }
}

#endif