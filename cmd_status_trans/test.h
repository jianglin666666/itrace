#ifndef TEST_H
#define TEST_H

void cmd_and_status_channel_rw_loop_test();

void cmd_and_status_channel_loop_write();
void cmd_and_status_channel_loop_read();
void cmd_status_trans_rw_test();

int check_data_inc(char start_val, int start_val_vaild, char *data, int length);



#endif // TEST_H
