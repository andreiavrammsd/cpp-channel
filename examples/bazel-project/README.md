# Bazel project

Example of using C++ Channel in a project with Bazel.

## Requirements

* C++11 compiler
* Bazel

## Build and run

```shell script
bazel run //:bazel-project

docker run --rm -ti -v $PWD:/app -w /app --name bazel-project gcr.io/bazel-public/bazel:8.0.1 run //:bazel-project
```
