########################################################################################
##	MIT License
########################################################################################
##	Copyright (c) 2012-2020 University of Bremen, Germany. 
##  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
##  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
##
##	Permission is hereby granted, free of charge, to any person obtaining a copy
##	of this software and associated documentation files (the "Software"), to deal
##	in the Software without restriction, including without limitation the rights
##	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
##	copies of the Software, and to permit persons to whom the Software is
##	furnished to do so, subject to the following conditions:
##
##	The above copyright notice and this permission notice shall be included in all
##	copies or substantial portions of the Software.
##
##	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
##	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
##	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
##	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
##	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
##	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
##	SOFTWARE.
##########################################################################################

add_executable(ALU4 ALU4.cpp)
target_link_libraries(ALU4 ${PROJECT_NAME} ${ALL_EXTERNAL_LIBS})
add_executable(ALU12 ALU12.cpp)
target_link_libraries(ALU12 ${PROJECT_NAME} ${ALL_EXTERNAL_LIBS})
add_executable(ALU16 ALU16.cpp)
target_link_libraries(ALU16 ${PROJECT_NAME} ${ALL_EXTERNAL_LIBS})
add_executable(ALU24 ALU24.cpp)
target_link_libraries(ALU24 ${PROJECT_NAME} ${ALL_EXTERNAL_LIBS})
add_executable(ALU32 ALU32.cpp)
target_link_libraries(ALU32 ${PROJECT_NAME} ${ALL_EXTERNAL_LIBS})
