FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ARG USER=ubuntu

# Install packages and clean up after
RUN apt-get update && apt-get install -y --no-install-recommends \
    bash-completion \
    build-essential \
    cmake \
    clang \
    clang-format \
    clang-tidy \
    clangd \
    curl \
    doxygen \
    gdb \
    git \
    graphviz \
    lcov \
    lldb \
    llvm \
    nano \
    openssh-client \
    pipx \
    software-properties-common \
    sudo \
    unzip \
    wget \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Configure user environment and permissions
RUN passwd -d ${USER} \
    && mkdir -p /home/${USER}/.cache \
    && chown -R ${USER}:${USER} /home/${USER}/.cache \
    && echo "export PROMPT_COMMAND='history -a'" >> /home/${USER}/.bashrc \
    && echo "export HISTFILE=/home/${USER}/.cache/.bash_history" >> /home/${USER}/.bashrc

USER ${USER}

# Install cmake-format
RUN pipx install cmake-format --include-deps \
    && pipx ensurepath

WORKDIR /workspace
