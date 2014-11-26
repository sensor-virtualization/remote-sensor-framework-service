
#include <syslog.h> 
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <common.h>
#include <dlog.h>
#include <stdarg.h>

#ifndef EXTAPI
#define EXTAPI __attribute__((visibility("default")))
#endif

#define RSF_SERVER_MSG_LOG_FILE		"/var/log/messages"
#define FILE_LENGTH 255

static int rsf_debug_file_fd;
static char rsf_debug_file_buf[FILE_LENGTH];

EXTAPI void rsf_log(int type , int priority , const char *tag , const char *fmt , ...)
{
	va_list ap;
	va_start(ap, fmt);
	
	switch (type) {
		case RSF_LOG_PRINT_FILE:
			rsf_debug_file_fd = open(RSF_SERVER_MSG_LOG_FILE, O_WRONLY|O_CREAT|O_APPEND, 0644);
			if (rsf_debug_file_fd != -1) {
				vsnprintf(rsf_debug_file_buf,255, fmt , ap );
				write(rsf_debug_file_fd, rsf_debug_file_buf, strlen(rsf_debug_file_buf));
				close(rsf_debug_file_fd);
			}
			break;

		case RSF_LOG_SYSLOG:
			int syslog_prio;
			switch (priority) {
				case RSF_LOG_ERR:
					syslog_prio = LOG_ERR|LOG_DAEMON;
					break;
					
				case RSF_LOG_DBG:
					syslog_prio = LOG_DEBUG|LOG_DAEMON;
					break;

				case RSF_LOG_INFO:
					syslog_prio = LOG_INFO|LOG_DAEMON;
					break;
					
				default:
					syslog_prio = priority;
					break;
			}
			
			vsyslog(syslog_prio, fmt, ap);
			break;

		case RSF_LOG_DLOG:
			if (tag) {
				switch (priority) {
					case RSF_LOG_ERR:
						SLOG_VA(LOG_ERROR, tag ? tag : "NULL" , fmt ? fmt : "NULL" , ap);
						break;

					case RSF_LOG_DBG:
						SLOG_VA(LOG_DEBUG, tag ? tag : "NULL", fmt ? fmt : "NULL" , ap);
						break;

					case RSF_LOG_INFO:
						SLOG_VA(LOG_INFO, tag ? tag : "NULL" , fmt ? fmt : "NULL" , ap);
						break;
				}
			}
			break;
	}

	va_end(ap);
}
