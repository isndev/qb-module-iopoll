
if (QB_BUILD_TEST)
    qb_register_module_gtest(
            NAME ${PROJECT_NAME}
            TESTNAME polling
            DEPENDENCIES ${QB_PREFIX}-network
            SOURCES test-polling.cpp
    )
endif()