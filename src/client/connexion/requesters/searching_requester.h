#pragma once
#include "client/connexion/connexion.h"

struct SearchingRequester : Requester {
    using Requester::Requester;
};

struct MockSearchingRequester : MockRequester {
    using MockRequester::MockRequester;

    void request_all_sessions() const;
};
