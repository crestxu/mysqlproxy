#include <spy_core.h>
#include <spy_config.h>

static spy_log_t spy_log;
static spy_open_file_t spy_log_file;
spy_uint_t spy_use_stderr = 1;
u_char *SPY_LOG_ERR_PATH = (u_char *) "spy.log";

static spy_str_t err_levels[] = { spy_null_string, spy_string("emerg"),
		spy_string("alert"), spy_string("crit"), spy_string("error"),
		spy_string("warn"), spy_string("notice"), spy_string("info"),
		spy_string("debug") };

void spy_log_error_core(spy_uint_t level, spy_log_t *log, spy_err_t err,
		const char *fmt, va_list args) {

	u_char *p, *last;
	u_char errstr[SPY_MAX_ERROR_STR];

	if (log->file->fd == SPY_INVALID_FILE) {
		return;
	}

	last = errstr + SPY_MAX_ERROR_STR;

	spy_memcpy(errstr, spy_cached_err_log_time.data,
			spy_cached_err_log_time.len);

	p = errstr + spy_cached_err_log_time.len; // 添加時間

	p = spy_slprintf(p, last, " [%S] ", err_levels[level]); // 添加Debug Lv

	p = spy_vslprintf(p, last, fmt, args); // 自定義字符串

	if (err) {
		p = spy_log_errno(p, last, err); // 添加errno
	}

	if (p > last - SPY_LINEFEED_SIZE) {
		p = last - SPY_LINEFEED_SIZE;
	}

	*p++ = LF;

	(void) spy_write_fd(log->file->fd, errstr, p - errstr);

	if (!spy_use_stderr || log->file->fd == spy_stderr) {
		return;
	}

	(void) spy_console_write(spy_stderr, errstr, p - errstr);
}

void spy_log_debug_core(spy_log_t *log, spy_err_t err, const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	spy_log_error_core(SPY_LOG_DEBUG, log, err, fmt, args);
	va_end(args);
}

void spy_cdecl
spy_log_error(spy_uint_t level, spy_log_t *log, spy_err_t err, const char *fmt,
		...) {
	va_list args;

	if (log->log_level >= level) {
		va_start(args, fmt);
		spy_log_error_core(level, log, err, fmt, args);
		va_end(args);
	}
}

void spy_cdecl
spy_log_debug(spy_uint_t level, spy_log_t *log, spy_err_t err, const char* fmt,
		...) {

	if ((log)->log_level & level) {
		spy_log_debug_core(log, err, fmt);
	}
}

void spy_cdecl
spy_log_stderr(spy_err_t err, const char *fmt, ...) {
	u_char *p, *last;
	va_list args;
	u_char errstr[SPY_MAX_ERROR_STR];

	last = errstr + SPY_MAX_ERROR_STR;
	p = errstr + 5;

	spy_memcpy(errstr, "spy: ", 5);

	va_start(args, fmt);
	p = spy_vslprintf(p, last, fmt, args);
	va_end(args);

	if (err) {
		p = spy_log_errno(p, last, err);
	}

	if (p > (last - SPY_LINEFEED_SIZE)) {
		p = last - SPY_LINEFEED_SIZE;
	}
	*p++ = LF;

	spy_write_fd(spy_stderr, (void *) errstr, (p - errstr));
}

void spy_cdecl
spy_log_stdout(const char *fmt, ...) {
	u_char *p, *last;
	va_list args;
	u_char errstr[SPY_MAX_ERROR_STR];

	last = errstr + SPY_MAX_ERROR_STR;
	p = errstr + 5;

	spy_memcpy(errstr, "spy: ", 5);

	va_start(args, fmt);
	p = spy_vslprintf(p, last, fmt, args);
	va_end(args);

	if (p > (last - SPY_LINEFEED_SIZE)) {
		p = last - SPY_LINEFEED_SIZE;
	}
	*p++ = LF;

	spy_write_fd(spy_stdout, (void *) errstr, (p - errstr));
}

spy_log_t *
spy_log_init(u_char *prefix) {

	u_char *p, *name;
	size_t nlen, plen;

	spy_log.file = &spy_log_file;
	spy_log.log_level = SPY_LOG_ERR;

	name = SPY_LOG_ERR_PATH;

	// 空名字直接打拼到标准错误输出
	nlen = spy_strlen(name);
	if (nlen == 0) {
		spy_log_file.fd = spy_stderr;
		return &spy_log;
	}

	p = NULL;
	// 非绝对路径，使用SPY_LOG_PREFIX
	if (name[0] != '/') {

		if (prefix) {
			plen = spy_strlen(prefix);
		} else {
#ifdef SPY_PREFIX
			prefix = (u_char *) SPY_PREFIX;
			plen = spy_strlen(prefix);
#else
			plen = 0;
#endif
		}

		// 填充新名
		if (plen) {
			name = malloc(plen + nlen + 2);
			if (name == NULL) {
				return NULL;
			}

			p = spy_cpymem(name, prefix, plen);

			if (!spy_path_separator(*(p - 1))) {
				*p++ = '/';
			}

			spy_cpystrn(p, name, nlen + 1);

			p = name;
		}

	}

	// 打开文件
	spy_log.file->fd
			= spy_open_file(name, SPY_FILE_APPEND, SPY_FILE_CREATE_OR_OPEN,
					SPY_FILE_DEFAULT_ACCESS);
	if (spy_log_file.fd == SPY_INVALID_FILE) {
		spy_log_stderr(spy_errno, "[error] could not open error log file: "
		spy_open_file_n " \"%s\" failed", name);

		spy_log_file.fd = spy_stderr;
	}

	if (p) {
		spy_free(p);
	}

	return &spy_log;
}

u_char *spy_log_errno(u_char *buf, u_char *last, spy_err_t err) {

	if (buf > last - 50) {

		/* leave a space for an error code */

		buf = last - 50;
		*buf++ = '.';
		*buf++ = '.';
		*buf++ = '.';
	}

	buf = spy_slprintf(buf, last, " (%d: ", err);

	buf = spy_strerror(err, buf, last - buf);

	if (buf < last) {
		*buf++ = ')';
	}

	return buf;
}

#ifdef _SPY_LOG_UNIT_TEST_

int main()
{

	exit(EXIT_SUCCESS);
}

#endif

