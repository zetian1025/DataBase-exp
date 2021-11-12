#include "utils.h"

int is_smaller(int a, int b, int c, int d) {
    return (a < b) || ((a == b) && (c < d));
}

void swap(blk_data *data, int i, int j) {
    int temp_x, temp_y;
    temp_x = data->x[i];
    temp_y = data->y[i];
    data->x[i] = data->x[j];
    data->y[i] = data->y[j];
    data->x[j] = temp_x;
    data->y[j] = temp_y;
}

void sort_InBlock(blk_data *data){
    for (int i=0; i<7; i++) {
        for (int j=i+1; j<7; j++) {
            if (!is_smaller(data->x[i], data->x[j], data->y[i], data->y[j])) {
                swap(data, i, j);
            }
        }
    }
}

void sort_CrossBlock_1(Buffer * buf, unsigned int src, unsigned int dst){
    unsigned char * blks[8];
    blk_data data[8];
    // Read 8 blocks from disk and sort each of them.
    for (int i=0; i<8; i++) {
        blks[i] = ReadBlockFromDisk(src+i, buf);
        GetDataFromBlk(blks[i], &data[i]);
        sort_InBlock(&data[i]);
    }

    int index[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int offset = 0;
    blk_data data_w;
    int index_w = 0;
    int min_x, min_y, min_index;
    while (1)
    {
        min_x = -1;
        min_y = -1;
        for (int i=0; i<8; i++) {
            if (index[i] == 7) {
                continue;
            }
            min_x = data[i].x[index[i]];
            min_y = data[i].y[index[i]];
            min_index = i;
            break;
        }
        if (min_x == -1 || min_y == -1) {
            break;
        }

        for (int i=0; i<8; i++) {
            if (index[i] == 7) {
                continue;
            }
            if (!is_smaller(min_x, data[i].x[index[i]], min_y, data[i].y[index[i]])){
                min_x = data[i].x[index[i]];
                min_y = data[i].y[index[i]];
                min_index = i;
            }
        }
        data_w.x[index_w] = min_x;
        data_w.y[index_w] = min_y;
        index_w++;
        index[min_index]++;
        if (index_w == 7){
            data_w.addr = dst+offset+1;
            blks[0] = WriteDataToDisk(blks[0], &data_w, dst+offset, buf);
            index_w = 0;
            offset++;
        }
    }

    blks[0] = ReadDataFromDisk(blks[0], &data_w, dst+offset-1, buf);
    data_w.addr = 0;
    blks[0] = WriteDataToDisk(blks[0], &data_w, dst+offset-1, buf);

    for (int i=0; i<8; i++) {
        FreeBlockInBuffer(blks[i], buf);
    }
}

void sort_CrossBlock_2(Buffer * buf, unsigned int src, unsigned int dst, unsigned int stride, unsigned int num_groups){
    unsigned char * blks[8];
    blk_data data[8];

    for (int i=0; i<8/num_groups; i++) {
        for (int j=0; j<num_groups; j++) {
            blks[i*num_groups + j] = ReadBlockFromDisk(src + j*stride + i, buf);
            GetDataFromBlk(blks[i*num_groups + j], &data[i*num_groups + j]);
        }
    }

    int index[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int offset = 0;
    blk_data data_w;
    int index_w = 0;
    int min_x, min_y, min_index;
    while (1)
    {
        min_x = -1;
        min_y = -1;
        for (int i=0; i<num_groups; i++) {
            if (index[i] == 7) {
                continue;
            }
            min_x = data[i].x[index[i]];
            min_y = data[i].y[index[i]];
            min_index = i;
            break;
        }
        if (min_x == -1 || min_y == -1) {
            break;
        }

        for (int i=0; i<num_groups; i++) {
            if (index[i] == 7) {
                continue;
            }
            if (!is_smaller(min_x, data[i].x[index[i]], min_y, data[i].y[index[i]])){
                min_x = data[i].x[index[i]];
                min_y = data[i].y[index[i]];
                min_index = i;
            }
        }
        data_w.x[index_w] = min_x;
        data_w.y[index_w] = min_y;
        index_w++;
        index[min_index]++;
        if (index_w == 7){
            data_w.addr = dst+offset+1;
            blks[0] = WriteDataToDisk(blks[0], &data_w, dst+offset, buf);
            index_w = 0;
            offset++;
        }

        if (index[min_index] == 7) {
            while (min_index+num_groups<8 && data[min_index].addr) {
                for (int i=0; i<7; i++) {
                    data[min_index].x[i] = data[min_index+num_groups].x[i];
                    data[min_index].y[i] = data[min_index+num_groups].y[i];
                }
                data[min_index].addr = data[min_index+num_groups].addr;
                index[min_index] = index[min_index+num_groups];
                min_index += num_groups;
            }

            if (data[min_index].addr) {
                blks[0] = ReadDataFromDisk(blks[0], &data[min_index], data[min_index].addr, buf);
            }
        }
    }
    if (index_w != 0) {
        data_w.addr = 0;
        blks[0] = WriteDataToDisk(blks[0], &data_w, dst+offset-1, buf);
    }
    else {
        blks[0] = ReadDataFromDisk(blks[0], &data_w, dst+offset-1, buf);
        data_w.addr = 0;
        blks[0] = WriteDataToDisk(blks[0], &data_w, dst+offset-1, buf);
    }

    for (int i=0; i<8; i++) {
        FreeBlockInBuffer(blks[i], buf);
    }
}

int main(){
    Buffer buf;
    InitBuffer(BUFSIZE, BLKSIZE, &buf);
    sort_CrossBlock_1(&buf, 1, 49);
    sort_CrossBlock_1(&buf, 1+8, 49+8);
    sort_CrossBlock_2(&buf, 49, 301, 8, 2);
    ListBlockFromDisk(301, &buf);

    printf("===================\n");
    sort_CrossBlock_1(&buf, 17, 65);
    sort_CrossBlock_1(&buf, 17+8, 65+8);
    sort_CrossBlock_1(&buf, 17+2*8, 65+2*8);
    sort_CrossBlock_1(&buf, 17+3*8, 65+3*8);

    sort_CrossBlock_2(&buf, 65, 317, 8, 4);    
    ListBlockFromDisk(317, &buf);

    DropBlocksOnDisk(49, 48);
}