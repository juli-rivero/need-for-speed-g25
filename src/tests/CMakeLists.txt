target_sources(taller_tests
        PRIVATE
        # .cpp files
        protocol_core.cpp
        protocol_variants.cpp
        protocol_search.cpp
        timer_interval_tests.cpp
        PUBLIC
        # .h files
        protocol_mock_class.h
)
