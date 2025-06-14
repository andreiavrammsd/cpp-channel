DIRS = include tests examples

all:

bench:
	- sudo cpupower frequency-set --governor performance
	
	mkdir -p build/bench && cd build/bench \
	&& cmake ../.. -DCMAKE_BUILD_TYPE=Release -DCPP_CHANNEL_BUILD_TESTS=ON \
	&& cmake --build . --config Release --target channel_benchmark -j \
	&& ./tests/channel_benchmark
	
	- sudo cpupower frequency-set --governor powersave

coverage:
	mkdir -p build/coverage && cd build/coverage \
	&& cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DCPP_CHANNEL_BUILD_TESTS=ON -DCPP_CHANNEL_COVERAGE=ON \
	&& cmake --build . --config Debug --target channel_tests -j \
	&& ctest -C Debug --verbose -L channel_tests --output-on-failure -j \
	&& lcov  --capture --directory . --output-file coverage.info  --ignore-errors mismatch \
	&& lcov --remove coverage.info "*/usr/*" -o coverage.info \
	&& lcov --remove coverage.info "*/gtest/*" -o coverage.info \
	&& genhtml coverage.info --output-directory coverage-report \
	&& cd coverage-report \
	&& python3 -m http.server 8000

doc:
	doxygen
	cd docs && python3 -m http.server 8000

format:
	clang-format -i $(shell find $(DIRS) -name *.*pp)
	cmake-format -i $(shell find $(DIRS) -name CMakeLists.txt)
