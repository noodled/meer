
#ifdef HAVE_CONFIG_H
#include "config.h"             /* From autoconf */
#endif

#ifdef HAVE_LIBJSON_C
#include <json-c/json.h>
#endif

#ifndef HAVE_LIBJSON_C
libjson-c is required for Meer to function!
#endif



typedef struct _DecodeAlert _DecodeAlert;
struct _DecodeAlert
{

char *timestamp;
char *flowid;
char *in_iface;
char *event_type;
char *src_ip;
char *src_port;
char *dest_ip;
char *dest_port;
char *proto;
char *app_proto;
char *payload;
char *stream;
char *packet;

char alert_action[16];
    char alert_gid[5];
    char alert_signature_id[16];
    char alert_rev[5];
    char alert_signature[512];
    char alert_catagory[16];
    char alert_severity[5];

    char packet_info_link[32];

};


struct _DecodeAlert *Decode_JSON_Alert( struct json_object *json_obj );

