#pragma once
#include "client/connexion/connexion.h"

struct CreatingRequester : Requester {
    using Requester::Requester;
};

struct MockCreatingRequester : MockRequester {
    using MockRequester::MockRequester;
};
