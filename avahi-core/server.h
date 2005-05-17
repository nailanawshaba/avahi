#ifndef fooserverhfoo
#define fooserverhfoo

/* $Id$ */

/***
  This file is part of avahi.
 
  avahi is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.
 
  avahi is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
  Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with avahi; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#include "core.h"
#include "iface.h"
#include "prioq.h"
#include "llist.h"
#include "timeeventq.h"
#include "announce.h"
#include "subscribe.h"
#include "dns.h"
#include "rrlist.h"

struct AvahiEntry {
    AvahiServer *server;
    AvahiEntryGroup *group;

    gboolean dead;
    
    AvahiEntryFlags flags;
    AvahiRecord *record;
    gint interface;
    guchar protocol;

    AVAHI_LLIST_FIELDS(AvahiEntry, entries);
    AVAHI_LLIST_FIELDS(AvahiEntry, by_key);
    AVAHI_LLIST_FIELDS(AvahiEntry, by_group);
    
    AVAHI_LLIST_HEAD(AvahiAnnouncement, announcements);
};

struct AvahiEntryGroup {
    AvahiServer *server;
    gboolean dead;

    AvahiEntryGroupState state;
    gpointer userdata;
    AvahiEntryGroupCallback callback;

    guint n_probing;
    
    AVAHI_LLIST_FIELDS(AvahiEntryGroup, groups);
    AVAHI_LLIST_HEAD(AvahiEntry, entries);
};

struct AvahiServer {
    GMainContext *context;
    AvahiInterfaceMonitor *monitor;

    AVAHI_LLIST_HEAD(AvahiEntry, entries);
    GHashTable *entries_by_key;

    AVAHI_LLIST_HEAD(AvahiEntryGroup, groups);
    
    AVAHI_LLIST_HEAD(AvahiSubscription, subscriptions);
    GHashTable *subscription_hashtable;

    gboolean need_entry_cleanup, need_group_cleanup;
    
    AvahiTimeEventQueue *time_event_queue;
    
    gchar *host_name, *host_name_fqdn, *domain;

    gint fd_ipv4, fd_ipv6;

    GPollFD pollfd_ipv4, pollfd_ipv6;
    GSource *source;

    gboolean ignore_bad_ttl;

    /* Used for assembling responses */
    AvahiRecordList *record_list;
};

gboolean avahi_server_entry_match_interface(AvahiEntry *e, AvahiInterface *i);

void avahi_server_post_query(AvahiServer *s, gint interface, guchar protocol, AvahiKey *key);

void avahi_server_prepare_response(AvahiServer *s, AvahiInterface *i, AvahiEntry *e, gboolean unicast_response, gboolean auxiliary);
void avahi_server_prepare_matching_responses(AvahiServer *s, AvahiInterface *i, AvahiKey *k, gboolean unicast_response);
void avahi_server_generate_response(AvahiServer *s, AvahiInterface *i, AvahiDnsPacket *p, const AvahiAddress *a, guint16 port, gboolean legacy_unicast);

void avahi_entry_group_change_state(AvahiEntryGroup *g, AvahiEntryGroupState state);

gboolean avahi_entry_commited(AvahiEntry *e);

void avahi_server_enumerate_aux_records(AvahiServer *s, AvahiInterface *i, AvahiRecord *r, void (*callback)(AvahiServer *s, AvahiRecord *r, gboolean flush_cache, gpointer userdata), gpointer userdata);

#endif