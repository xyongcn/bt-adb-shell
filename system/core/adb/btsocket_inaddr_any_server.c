#include <cutils/sockets.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>

#ifndef HAVE_WINSOCK
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>


#define LISTEN_BACKLOG 4

int btsocket_inaddr_any_server(int port, int type)
{
    struct sockaddr_l2 addr;
    int s;
    int n;

    memset(&addr, 0, sizeof(addr));

    s = socket(PF_BLUETOOTH, type, BTPROTO_L2CAP);  
    if(s < 0) return -1;
    addr.l2_family = PF_BLUETOOTH;
    addr.l2_psm = htobs(port);
    bacpy(&addr.l2_bdaddr, BDADDR_ANY);
	
    n = 1;
    //setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&n, sizeof(n));

    if(bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        close(s);
        return -1;
    }

    int ret;
	ret = listen(s, LISTEN_BACKLOG);
	if (ret < 0) {
		close(s);
		return -1;
	}
    return s;

}

