catch_discover_tests(
  tests
  TEST_PREFIX " prefix "
  TEST_SUFFIX " suffix "
  ADD_TAGS_AS_LABELS
  DISCOVERY_MODE PRE_TEST
  USE_RELATIVE_PATHS
  WORKING_DIRECTORY "$<TARGET_FILE_DIR:tests>"
  ${extra_args}
)
