#pragma once

#include <string>

#include "client/connexion/connexion.h"

struct SearchingRequester : Requester {
    using Requester::Requester;
};

struct MockSearchingRequester : MockRequester {
    using MockRequester::MockRequester;

    void request_all_sessions() const;
    void request_join(const std::string& session_id) const;
};
