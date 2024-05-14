cmake_minimum_required(VERSION 3.29)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    LPKF91s
    VERSION 0.1.0
    LANGUAGES CXX
    )
  
find_package(GTest REQUIRED)
find_package(Boost REQUIRED)
find_package(wxWidgets REQUIRED)

if (GCC)
    add_compile_options(-pedantic) # Warn on language extensions
    add_compile_options(-Wall) # -Wextra reasonable and standard
    add_compile_options(-Wshadow) # warn the user if a variable declaration shadows one from a parent context
    add_compile_options(-Wnon-virtual-dtor) # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    # use these and consider the following (see descriptions below)
    add_compile_options(-Wold-style-cast) # warn for c-style casts
    add_compile_options(-Wcast-align) # warn for potential performance problem casts
    add_compile_options(-Wunused) # warn on anything being unused
    add_compile_options(-Woverloaded-virtual) # warn if you overload (not override) a virtual function
    add_compile_options(-Wpedantic) # (all versions of GCC, Clang >= 3.2) warn if non-standard C++ is used
    add_compile_options(-Wconversion) # warn on type conversions that may lose data
    add_compile_options(-Wsign-conversion) # (Clang all versions, GCC >= 4.3) warn on sign conversions
    add_compile_options(-Wmisleading-indentation) # (only in GCC >= 6.0) warn if indentation implies blocks where blocks do not exist
    add_compile_options(-Wduplicated-cond) # (only in GCC >= 6.0) warn if if / else chain has duplicated conditions
    add_compile_options(-Wduplicated-branches) # (only in GCC >= 7.0) warn if if / else branches have duplicated code
    add_compile_options(-Wlogical-op) # (only in GCC) warn about logical operations being used where bitwise were probably wanted
    add_compile_options(-Wnull-dereference) # (only in GCC >= 6.0) warn if a null dereference is detected
    add_compile_options(-Wuseless-cast) # (only in GCC >= 4.8) warn if you perform a cast to the same type
    add_compile_options(-Wdouble-promotion) # (GCC >= 4.6, Clang >= 3.8) warn if float is implicitly promoted to double
    add_compile_options(-Wformat=2) # warn on security issues around functions that format output (i.e., printf)
    add_compile_options(-Wlifetime) # (only special branch of Clang currently) shows object lifetime issues
    add_compile_options(-Wimplicit-fallthrough) # Warns when case statements fall-through. (Included with -Wextra in GCC, not in clang)

    # Consider using -Weverything and disabling the few warnings you need to on Clang
    # -Weffc++ warning mode can be too noisy, but if it works for your project, use it also.

elseif(MSVC)

    add_compile_definitions(_WIN32_WINNT=0x0601)  # windows feature magic happening here (just here to avoid compiler warning from boost??)

    add_compile_options(/permissive-) # Enforces standards conformance.
    add_compile_options(/W4) # All reasonable warnings
    add_compile_options(/w14640) # Enable warning on thread unsafe static member initialization
    # use these and consider the following (see descriptions below)
    add_compile_options(/w14242) # 'identfier': conversion from 'type1' to 'type1', possible loss of data
    add_compile_options(/w14254) # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
    add_compile_options(/w14263) # 'function': member function does not override any base class virtual member function
    add_compile_options(/w14265) # 'classname': class has virtual functions, but destructor is not virtual instances of this class may not be destructed correctly
    add_compile_options(/w14287) # 'operator': unsigned/negative constant mismatch
    add_compile_options(/we4289) # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside the for-loop scope
    add_compile_options(/w14296) # 'operator': expression is always 'boolean_value'
    add_compile_options(/w14311) # 'variable': pointer truncation from 'type1' to 'type2'
    add_compile_options(/w14545) # expression before comma evaluates to a function which is missing an argument list
    add_compile_options(/w14546) # function call before comma missing argument list
    add_compile_options(/w14547) # 'operator': operator before comma has no effect; expected operator with side-effect
    add_compile_options(/w14549) # 'operator': operator before comma has no effect; did you intend 'operator'?
    add_compile_options(/w14555) # expression has no effect; expected expression with side-effect
    add_compile_options(/w14619) # pragma warning: there is no warning number 'number'
    add_compile_options(/w14826) # Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior.
    add_compile_options(/w14905) # wide string literal cast to 'LPSTR'
    add_compile_options(/w14906) # string literal cast to 'LPWSTR'
    add_compile_options(/w14928) # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
endif()

configure_file(src/test.h.in project_version.h)
include_directories(${PROJECT_BINARY_DIR})

add_executable(gcode2hpgl src/gcode2hpgl_cli.cpp)
target_link_libraries(gcode2hpgl gtest::gtest)
target_link_libraries(gcode2hpgl boost::boost)

add_executable(hpgl2lpkf91s src/hpgl2lpkf91s_cli.cpp)
target_link_libraries(hpgl2lpkf91s boost::boost)

add_executable(lpkf91s src/lpkf91s_gui.cpp)
set_target_properties(lpkf91s PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
target_link_libraries(lpkf91s wxWidgets::wxWidgets)
target_link_libraries(lpkf91s boost::boost)
# target_sources(lpkf91s PRIVATE ./src/resource.rc)

add_executable(sendHPGL src/sendhpgl_gui.cpp)
set_target_properties(sendHPGL PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
target_link_libraries(sendHPGL wxWidgets::wxWidgets)
target_link_libraries(sendHPGL boost::boost)
# target_sources(sendHPGL PRIVATE ./src/resource.rc)



# Prevent building in the source directory
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n")
endif()




# Testing
include(CTest)
include(GoogleTest)
gtest_discover_tests(gcode2hpgl)




add_compile_options(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
set(CMAKE_CXX_CLANG_TIDY
  clang-tidy-11;
  -header-filter=${CMAKE_CURRENT_SOURCE_DIR};
)
