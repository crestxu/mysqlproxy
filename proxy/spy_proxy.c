#include <spy_config.h>
#include <spy_core.h>
#include <spy_proxy.h>

spy_int_t spy_proxy_init(spy_global_t *global) {

	spy_uint_t i;
	spy_core_conf_t *core_cf;
	struct sockaddr_in *serv_addr;
	spy_listening_t **ls;
	core_cf = spy_get_core_conf(global);

	ls = NULL;

	global->connection_n = core_cf->worker_connections;

	serv_addr = spy_alloc(sizeof(struct sockaddr_in), global->log);

	spy_memzero(serv_addr, sizeof(struct sockaddr_in));
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_port = htons(core_cf->port);

	for (i = 0; i < core_cf->listen_n; i++) {
		ls[i] = spy_alloc(sizeof(spy_listening_t), global->log);

		if (!inet_pton(AF_INET, (char *) core_cf->listen[i],
				&(serv_addr->sin_addr)))
			return SPY_ERROR;
		ls[i] = spy_create_listening(global, serv_addr, sizeof(*serv_addr));
		ls[i]->handler = spy_proxy_init_packet;
	}
	return SPY_OK;
}
