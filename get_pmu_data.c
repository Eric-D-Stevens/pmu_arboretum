#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>
#include <perfmon/pfmlib.h>
#include <perfmon/perf_event.h>
#include <perfmon/perf_event.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <err.h>
#include "include.h"

#define MAX_EVENTS 50

// setup to abort main processes when child process returns
int global_sigchld_trip = 0;
void sighandler(int signum);
void launch_child(char *command_to_execute);

int main(int argc, char *argv[]){

    //register SIGCHLD
    signal(SIGCHLD, sighandler);

    // create arrays of structs used by libpfm4
    pfm_perf_encode_arg_t pfm_events[MAX_EVENTS]; 
    struct perf_event_attr pes[MAX_EVENTS];
    char *fstrs[MAX_EVENTS];
    
    // create arrays for gathering PMU data
    int fd_arr[MAX_EVENTS];
    long long counts[MAX_EVENTS];


    // describe program and events to monitor
    printf("PREPARING TO RUN:\n\t%s\n", argv[1]);
    printf("MONITORING EVENTS: \n");
    for(int i=2; i<argc; i++){
        get_pfm_encoding(argv[i], &pfm_events[i-2], &pes[i-2], &fstrs[i-2]);
        printf("\t%s\n", fstrs[i-2]);
    }
    printf("\n");


    // for child
    pid_t proc = fork();
    // if child, run program
    if(proc<0){err(-1, "failed to fork");}

    // CHILD PROCESS
    if(proc==0){
        printf("ENTERED CILD PROCESS: %s\n\n", argv[1]);
        launch_child(argv[1]);
    }

    // PARENT PROCESS
    else{
        // decalre data file and remove exising contents
        FILE *data_output_file;
        data_output_file = fopen("current_data.csv","w");
        fclose(data_output_file);
        printf("monitoring child process: %d\n\n", proc);

        // create perf_event FDs
        pid_t pid = -1; //This measures the calling process/thread 
        int cpu = 0; // >=0: specified CPU -1: any cup
        int group_fd = -1; // no groups (all event leaders)
        int flags = 0;

        // create file descriptors for perf_events API
        int num_events = argc-2;
        for(int i=0; i<(num_events); i++){
            fd_arr[i] = perf_event_open(&pes[i],pid,cpu,group_fd,flags);
            if (fd_arr[i] == -1){
			    fprintf(stderr, "Error opening %s\n", fstrs[i]);
			    exit(EXIT_FAILURE);
		    }
        }

        // vars to watch record time
        struct timeval start_time, end_time;
        double run_time;
        


        // LOOP MONITORING, EXIT WHEN CHILD EXITS
        while(!global_sigchld_trip){

            // reset, start timer, then enable counters
            for(int i=0; i<(num_events); i++){ioctl(fd_arr[i], PERF_EVENT_IOC_RESET,0);}
            gettimeofday(&start_time, NULL);
            for(int i=0; i<(num_events); i++){ioctl(fd_arr[i], PERF_EVENT_IOC_ENABLE,0);}

            // let the events be counted and time it
            sleep(2);

            // disable, end timer, then get counts
            for(int i=0; i<(num_events); i++){ioctl(fd_arr[i], PERF_EVENT_IOC_DISABLE,0);}
            gettimeofday(&end_time, NULL);
            for(int i=0; i<(num_events); i++){read(fd_arr[i], &counts[i], sizeof(long long));}

            // calculate runtime
            run_time = ((double) (end_time.tv_sec - start_time.tv_sec)) + ((double) (end_time.tv_usec - start_time.tv_usec))*1e-6;


            // print results
            printf("%f\t", run_time);
            for(int i=0; i<(num_events); i++){printf("%lld\t", counts[i]);}
            printf("\n");

            data_output_file = fopen("current_data.csv","a");
                fprintf(data_output_file, "%f,\t", run_time);
                for(int i=0; i<(num_events); i++){fprintf(data_output_file, "%lld,\t", counts[i]);}
                fprintf(data_output_file,"\n");
            fclose(data_output_file);


        }
    }
}

void sighandler(int signum){
    char *sigstr;
    sigstr = strsignal(signum);
    printf("Caught signal %s ...", sigstr);
    global_sigchld_trip = 1;
}


void launch_child(char *command_to_execute){
    system(command_to_execute);
    printf("\n\n");
}



// NOTES 

    //get_pfm_encoding("skl::INT_MISC:RECOVERY_CYCLES:e=0:i=0:c=0:t=0:intx=0:intxcp=0:u=1:k=1:mg=0:mh=1", &pfm_events[0], &pes[0], &fstrs[0]);
    //get_pfm_encoding("skl::CPU_CLK_THREAD_UNHALTED:REF_XCLK:e=0:i=0:c=0:t=1:intx=0:intxcp=0:u=1:k=1:mg=0:mh=1", &pfm_events[1], &pes[1], &fstrs[1]);