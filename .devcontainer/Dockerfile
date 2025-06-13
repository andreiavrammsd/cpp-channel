FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    bash-completion \
    build-essential \
    cmake \
    clang \
    clangd \
    clang-format \
    clang-tidy \
    curl \
    doxygen \
    gdb \
    git \
    graphviz \
    lcov \
    lldb \
    llvm \
    nano \
    software-properties-common \
    sudo \
    unzip \
    wget \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

ARG USERNAME=ubuntu
RUN passwd -d ${USERNAME} \
    && mkdir -p /home/${USERNAME}/.cache && chown -R ${USERNAME}:${USERNAME} /home/${USERNAME}/.cache \
    && SNIPPET="export PROMPT_COMMAND='history -a' && export HISTFILE=/home/${USERNAME}/.cache/.bash_history" \
    && echo "$SNIPPET" >> "/home/${USERNAME}/.bash_aliases"

USER $USERNAME

WORKDIR /workspace
