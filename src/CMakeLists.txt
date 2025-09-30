add_library(HoRGod
    utils/circuit.cpp
    HoRGod/types.cpp
    HoRGod/sharing.cpp
    HoRGod/helpers.cpp
    HoRGod/rand_gen_pool.cpp
    HoRGod/offline_evaluator.cpp
    HoRGod/ijmp.cpp
    HoRGod/online_evaluator.cpp)
target_include_directories(HoRGod PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(HoRGod PUBLIC Boost::system EMPTool EMP_OT NTL GMP)
