target_sources(typecheck PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/ConstraintPass.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Debug.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FunctionDefinition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FunctionVar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/GenericTypeGenerator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/KnownProtocolKind.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RawType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Type.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/TypeManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/TypeManager+Constraints.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/TypeVar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Constraints.cpp")

add_subdirectory(protocols)
