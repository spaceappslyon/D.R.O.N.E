#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

int main(void)
{
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };
    bdaddr_t target;
    str2ba( "01:23:45:67:89:AB", &target );
    
    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }

    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
            name, 0) < 0)
        strcpy(name, "[unknown]");
        printf("%s  %s\n", addr, name);
	if(strstr(name, "V1C0D3R") != NULL)
	{
		target = (ii+i)->bdaddr;
		printf("Adresse sauvee\n");
	}
    }

	////////////////////////////////////

    uint8_t svc_uuid_int[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0xab, 0xcd };
    uuid_t svc_uuid;
    int err;
    
    sdp_list_t *response_list = NULL, *search_list, *attrid_list;
    sdp_session_t *session = 0;

    // connect to the SDP server running on the remote machine
    session = sdp_connect( BDADDR_ANY, &target, SDP_RETRY_IF_BUSY );

    // specify the UUID of the application we're searching for
    sdp_uuid128_create( &svc_uuid, &svc_uuid_int );
    search_list = sdp_list_append( NULL, &svc_uuid );

    // specify that we want a list of all the matching applications' attributes
    uint32_t range = 0x0000ffff;
    attrid_list = sdp_list_append( NULL, &range );

    // get a list of service records that have UUID 0xabcd
    err = sdp_service_search_attr_req( session, search_list, \
            SDP_ATTR_REQ_RANGE, attrid_list, &response_list);

     sdp_list_t *r = response_list;

    // go through each of the service records
    for (; r; r = r->next ) {
        sdp_record_t *rec = (sdp_record_t*) r->data;
        sdp_list_t *proto_list;
        
        // get a list of the protocol sequences
        if( sdp_get_access_protos( rec, &proto_list ) == 0 ) {
	        sdp_list_t *p = proto_list;
	
	        // go through each protocol sequence
	        for( ; p ; p = p->next ) {
	            sdp_list_t *pds = (sdp_list_t*)p->data;
	
	            // go through each protocol list of the protocol sequence
	            for( ; pds ; pds = pds->next ) {
	
	                // check the protocol attributes
	                sdp_data_t *d = (sdp_data_t*)pds->data;
	                int proto = 0;
	                for( ; d; d = d->next ) {
	                    switch( d->dtd ) { 
	                        case SDP_UUID16:
	                        case SDP_UUID32:
	                        case SDP_UUID128:
	                            proto = sdp_uuid_to_proto( &d->val.uuid );
	                            break;
	                        case SDP_UINT8:
	                            if( proto == RFCOMM_UUID ) {
	                                printf("rfcomm channel: %d\n",d->val.int8);
	                            }
	                            break;
	                    }
	                }
	            }
	            sdp_list_free( (sdp_list_t*)p->data, 0 );
	        }
        	sdp_list_free( proto_list, 0 );
        }

        printf("found service record 0x%x\n", rec->handle);
        sdp_record_free( rec );
    }

    sdp_close(session);
    ///////////////////////////////////////////////

    free( ii );
    close( sock );
    return 0;
}