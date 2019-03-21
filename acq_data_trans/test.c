
#include "test.h"
#include "server.h"



void read_dma_test()
{
    int dma_fd  = -1;
    #ifndef SIMU_MODE
            dma_fd = open_dma_device(DmaDevicePath);
    #else
            dma_fd = 1;
    #endif
            printc((dma_fd > 0) ? MSG_INFO : MSG_ERROR, "open device %s %s\n", DmaDevicePath, (dma_fd > 0) ? "success" : "failed");
            if(dma_fd < 0)
            {
                exit(0);
            }
            printc(MSG_INFO, "success \n");
            // init mem
            printc(MSG_INFO, "init dma buffer list  \n");

            char *dma_fd_mmap_area = NULL;
    #ifndef SIMU_MODE
            dma_fd_mmap_area = (char *) mmap(NULL, 2*MAX_DMA_LENGTH, PROT_READ, MAP_SHARED, dma_fd, 0);
    #else
            dma_fd_mmap_area = (char *)malloc(2*MAX_DMA_LENGTH);
    #endif
            if(dma_fd_mmap_area == NULL)
            {
                printc(MSG_ERROR, "applicate dma_device mem failed, process quit\n");
                close_fd(1, &dma_fd);
                exit(0);
            }





    int read_ret = 0;
    int err_cnt_sum = 0;
    char buf_index = 0;

    int is_first_read = 1;
    char start_val = 0;
    int sum = 0;
    int print_control_cnt = 10 * 1024 * 1024;

    while(1)
    {

         ioctl(dma_fd, CONFIG_ADDROFFSET, buf_index);
         read_ret = read(dma_fd, dma_fd_mmap_area, MAX_DMA_LENGTH);

        if(read_ret < 0)
        {
            printc(MSG_ERROR, "cmd_and_status_channel_loop_read, thread exit\n");
            return;
        }


        sum += read_ret;


        if(is_first_read)
        {
            err_cnt_sum += check_data_inc(start_val, 0,dma_fd_mmap_area, read_ret);

            is_first_read = !is_first_read;
        }else{
            err_cnt_sum += check_data_inc(start_val, 1,dma_fd_mmap_area, read_ret);
        }
        start_val = dma_fd_mmap_area[read_ret - 1];

        printc(MSG_INFO, "the DMA err_cnt_sum is %d\n",err_cnt_sum);
        if(sum > print_control_cnt)
        {
            printc(MSG_INFO, "===================== read status, sum is %d\n", sum);
            print_control_cnt *= 2;

        }

    }
    munmap(dma_fd_mmap_area,2*MAX_DMA_LENGTH);
}







int check_data_inc(char start_val, int start_val_vaild, char *data, int length)
{
    int err_cnt = 0;
    if(start_val_vaild == 1)
    {
        start_val = data[0] - 1;
    }
    for(int i = 0; i < length; i++)
    {
        start_val++;
        if(start_val != data[i])
        {
            start_val = data[i];
            err_cnt++;
        }
    }
    return err_cnt;
}
