add_library(pteros_substructure_search SHARED
    substructure_search.cpp
    ${PROJECT_SOURCE_DIR}/include/pteros/extras/substructure_search.h
    )

target_link_libraries(pteros_substructure_search pteros)

install(TARGETS pteros_substructure_search
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)


