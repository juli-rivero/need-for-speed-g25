target_sources(taller_client
        PRIVATE
        # .cpp files
        creating_requester.cpp
        searching_requester.cpp
        selecting_requester.cpp
        waiting_requester.cpp
        PUBLIC
        # .h files
        creating_requester.h
        searching_requester.h
        selecting_requester.h
        waiting_requester.h
)
