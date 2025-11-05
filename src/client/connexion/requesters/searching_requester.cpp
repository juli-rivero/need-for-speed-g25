#include "client/connexion/requesters/searching_requester.h"

using dto_search::JoinResponse;
using dto_search::SearchResponse;

void MockSearchingRequester::request_all_sessions() const {
    response_listener.recv(SearchResponse{{
        SessionInfo{
            .name = "Los menos capos",
            .maxPlayers = 2,
            .raceCount = 3,
            .city = "New York",
            .currentPlayers = 1,
            .status = SessionStatus::Waiting,
        },
        SessionInfo{
            .name = "Los mas capos",
            .maxPlayers = 3,
            .raceCount = 3,
            .city = "Buenos Aires",
            .currentPlayers = 3,
            .status = SessionStatus::Full,
        },
        SessionInfo{
            .name = "Partida de Pablo",
            .maxPlayers = 5,
            .raceCount = 2,
            .city = "Rosario",
            .currentPlayers = 5,
            .status = SessionStatus::Playing,
        },
    }});
}
void MockSearchingRequester::request_join(const std::string& session_id) const {
    response_listener.recv(JoinResponse{session_id});
}
