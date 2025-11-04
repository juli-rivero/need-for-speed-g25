#pragma once
#include "client/connexion/connexion.h"

struct WaitingRequester : Requester {
    using Requester::Requester;
};

struct MockWaitingRequester : MockRequester {
    using MockRequester::MockRequester;
};
