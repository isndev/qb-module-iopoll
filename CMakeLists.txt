project(iopoll)

if(UNIX)
        qb_register_module(
                NAME ${PROJECT_NAME}
                VERSION 1.0.0
        )

        add_subdirectory(test)
else()
        message(STATUS "qb-module-iopoll is available ony on UNIX systems")
endif()