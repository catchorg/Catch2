include(CTest)

set(SURROGATE_TEST_SOURCES
  surr_catch_benchmark.cpp
  )

add_executable(SurrogateTUsTest ${SURROGATE_TEST_SOURCES})
target_link_libraries(SurrogateTUsTest PRIVATE Catch2WithMain)

add_test(NAME SurrogateTUsTest COMMAND $<TARGET_FILE:SurrogateTUsTest>)
