#pragma once
#include "client/connexion/connexion.h"

struct SelectingRequester : Requester {
    using Requester::Requester;
};

struct MockSelectingRequester : MockRequester {
    using MockRequester::MockRequester;
};
