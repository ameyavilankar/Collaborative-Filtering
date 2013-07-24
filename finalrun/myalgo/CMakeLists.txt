project(GraphLab)
# include(CheckCXXSourceCompiles)

# Build als
add_graphlab_executable(finalrun finalrun.cpp)
requires_eigen(finalrun) # build and attach eigen
