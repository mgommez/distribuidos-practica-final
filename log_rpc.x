struct log_data {
    opaque username[256];
    opaque op[20];
    opaque fileName[256];
    opaque date_time[20];
} ;

program LOG {
    version LOGVER {

        int logging                          ( log_data l ) = 1;

    } = 1;
} = 100454255;