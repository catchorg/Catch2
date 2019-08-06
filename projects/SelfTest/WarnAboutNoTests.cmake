# Workaround for a peculiarity where CTest disregards the return code from a
# test command if a PASS_REGULAR_EXPRESSION is also set
execute_process(
    COMMAND ${CMAKE_ARGV3} -w NoTests "___nonexistent_test___"
    RESULT_VARIABLE ret
    OUTPUT_VARIABLE out
)

message("${out}")

if(NOT ${ret} MATCHES "^[0-9]+$")
    message(FATAL_ERROR "${ret}")
endif()

if(${ret} EQUAL 0)
    message(FATAL_ERROR "Expected nonzero return code")
elseif(${out} MATCHES "Helper failed with")
    message(FATAL_ERROR "Helper failed")
endif()
