#!/bin/bash
# Fetch git submodules for a full clone. Exported trees have no .git and skip this.
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

if [ ! -e "${ROOT}/.git" ]; then
    exit 0
fi
if ! command -v git >/dev/null 2>&1; then
    echo "git is required to fetch thirdparty submodules" >&2
    exit 1
fi

git -C "${ROOT}" submodule update --init --recursive
