#include "client/connexion/requesters/creating_requester.h"

using dto_search::CreateResponse;

void MockCreatingRequester::create_session(const SessionConfig&) const {
    response_listener.recv(CreateResponse{});
}
