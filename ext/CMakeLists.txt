cmake_minimum_required(VERSION 3.5)
project(ISToolExt)

aux_source_directory(z3 Z3_SOURCE)
aux_source_directory(composed_semantics COMPOSE_SOURCE)
add_library(ext_lib STATIC ${Z3_SOURCE} ${COMPOSE_SOURCE})

