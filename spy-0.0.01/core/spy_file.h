


#ifndef _SPY_FILE_H_INCLUDED_
#define _SPY_FILE_H_INCLUDED_

#include <spy_core.h>
#include <spy_config.h>


struct spy_log_file_s {
    spy_fd_t                   fd;
    spy_str_t                  name;
};

#endif