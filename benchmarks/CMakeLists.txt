add_executable(Benchmarks catch_benchmarks.cpp)
target_link_libraries(Benchmarks PRIVATE Catch2WithMain)
target_compile_features(Benchmarks PUBLIC cxx_std_14)

list(APPEND CATCH_TEST_TARGETS Benchmarks)
set(CATCH_TEST_TARGETS ${CATCH_TEST_TARGETS} PARENT_SCOPE)
