#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>
#include <perfmon/pfmlib.h>
#include <perfmon/perf_event.h>
#include <perfmon/pfmlib_perf_event.h>
#include <string.h>
#include <signal.h>
#include <err.h>

void get_pfm_encoding(char *event_string, 
                      pfm_perf_encode_arg_t *pfm_event,
                      struct perf_event_attr *pe,
                      char **fstr){

    // init libpfm
    int ret; // error check
    ret = pfm_initialize();
    if (ret != PFM_SUCCESS)
        errx(1, "cannot init lib %s\n", pfm_strerror(ret));



    // ?
    memset(pfm_event, 0, sizeof(pfm_perf_encode_arg_t));
    memset(pe, 0, sizeof(struct perf_event_attr));

    // set pfm attrs
    pfm_event->attr = pe;
    pfm_event->fstr = fstr;
    pfm_event->size = sizeof(pfm_perf_encode_arg_t);

    // set perf attrs
    pe->inherit = 1;
    pe->disabled = 1; // wait to activate

    // get encoding
    
    int ring = PFM_PLM0 | PFM_PLM3;
    pfm_os_t pmu = PFM_OS_PERF_EVENT;

    ret = pfm_get_os_event_encoding(event_string,
                                    ring,
                                    pmu,
                                    pfm_event);
    if (ret != PFM_SUCCESS)
        errx(1, "cannot init lib %s\n", pfm_strerror(ret));
}
