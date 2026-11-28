FROM ghcr.io/catthehacker/ubuntu:act-24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-yaml \
    tidy \
    nginx \
    gettext-base \
    openssl \
    npm \
    && pip3 install --break-system-packages detect-secrets \
    && npm install -g markdownlint-cli2 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*