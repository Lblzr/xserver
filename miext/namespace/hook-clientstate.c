#define HOOK_NAME "clienstate"

#include <dix-config.h>

#include "dix/registry_priv.h"
#include "os/client_priv.h"
#include "os/auth.h"

#include "namespace.h"
#include "hooks.h"

void hookClientState(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(NewClientInfoRec);

    switch (client->clientState) {
    case ClientStateInitial:
        // better assign *someting* than null -- clients can't do anything yet anyways
        XnamespaceAssignClient(subj, &namespaces[NS_ID_ANONYMOUS]);
        break;

    case ClientStateRunning:
        subj->authId = AuthorizationIDOfClient(client);

        short unsigned int name_len = 0, data_len = 0;
        const char * name = NULL;
        char * data = NULL;
        if (AuthorizationFromID(subj->authId, &name_len, &name, &data_len, &data)) {
            XnamespaceAssignClient(subj, XnsFindByAuth(name_len, name, data_len, data));
        } else {
            XNS_HOOK_LOG("no auth data - assuming anon\n");
        }
        break;

    case ClientStateRetained:
        XnamespaceAssignClient(subj, NULL);
        break;
    case ClientStateGone:
        XnamespaceAssignClient(subj, NULL);
        break;
    default:
        XNS_HOOK_LOG("unknown state =%d\n", client->clientState);
        break;
    }
}
