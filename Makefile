PROJECT_DIRS = include tests examples
BUILD_DIR = build
BENCH_DIR = build/bench
COV_DIR = build/coverage
DOCS_DIR = docs

all:

bench:
	# If needed: sudo cpupower frequency-set --governor <performance|powersave>
	mkdir -p $(BENCH_DIR) && cd $(BENCH_DIR) \
	&& cmake ../.. -DCMAKE_BUILD_TYPE=Release -DCPP_CHANNEL_BUILD_BENCHMARKS=ON \
	&& cmake --build . --config Release --target channel_benchmark -j \
	&& ./benchmarks/channel_benchmark \
		--benchmark_repetitions=10 \
		--benchmark_report_aggregates_only=true

coverage:
	rm -rf $(COV_DIR) && mkdir $(COV_DIR) && cd $(COV_DIR) \
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
	cd $(DOCS_DIR) && python3 -m http.server 8000

format:
	clang-format -i $(shell find $(PROJECT_DIRS) -name *.*pp)
	cmake-format -i $(shell find $(PROJECT_DIRS) -name CMakeLists.txt)

clean:
	rm -rf $(BUILD_DIR) $(DOCS_DIR)
