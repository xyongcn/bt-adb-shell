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
#include <netdb.h>
#endif

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

int btsocket_network_client(char *server_addr, int cid, int type)
{
    int s;
    struct sockaddr_l2 addr;
    	
    memset(&addr, 0, sizeof(struct sockaddr_l2));
    addr.l2_family = PF_BLUETOOTH;
    str2ba(server_addr, &addr.l2_bdaddr);
    addr.l2_psm = htobs(cid);//0x1003);

    s = socket(PF_BLUETOOTH, type, BTPROTO_L2CAP);
    if(s < 0) return -1;

    if(connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        close(s);
        return -1;
    }

    return s;

}

