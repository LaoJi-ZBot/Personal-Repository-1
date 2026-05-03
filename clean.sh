#!/bin/bash
# ArenaBreakout 清理脚本
# 删除 build/ 和 third_party/ 下的编译产物

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

GREEN='\033[0;32m'
NC='\033[0m'

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_info "Cleaning build directory..."
if [ -d "build" ]; then
    rm -rf build
    log_info "build/ directory removed"
else
    log_info "build/ directory does not exist"
fi

log_info "Cleaning third-party compiled files..."
if [ -d "third_party" ]; then
    # 保留下载的源码，只删除编译产物
    find third_party -name "*.o" -delete 2>/dev/null || true
    find third_party -name "*.a" -delete 2>/dev/null || true
    find third_party -name "*.so" -delete 2>/dev/null || true
    find third_party -name "CMakeFiles" -type d -exec rm -rf {} + 2>/dev/null || true
    find third_party -name "CMakeCache.txt" -delete 2>/dev/null || true
    find third_party -name "cmake_install.cmake" -delete 2>/dev/null || true
    find third_party -name "Makefile" -delete 2>/dev/null || true
    log_info "Third-party compiled files cleaned"
else
    log_info "third_party/ directory does not exist"
fi

log_info "=========================================="
log_info "Clean complete!"
log_info "=========================================="
