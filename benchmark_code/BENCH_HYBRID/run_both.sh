#!/bin/bash

# Get the directory where the script is located
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"

echo "=========================================="
echo "1. SET 'NORMAL' MODE"
echo "=========================================="

# Update run.sh (uncomment normal logs, comment local x logs)
sed -i 's/.*OUT_LOGS="out_logs\/".*/            OUT_LOGS="out_logs\/"/' run.sh
echo "[CHECK] run.sh - out_logs/ line:"
grep 'OUT_LOGS="out_logs/"' run.sh

sed -i 's/.*OUT_LOGS="out_logs_CPU_LOCAL_X_OPT\/".*/            # OUT_LOGS="out_logs_CPU_LOCAL_X_OPT\/"/' run.sh
echo "[CHECK] run.sh - out_logs_CPU_LOCAL_X_OPT/ line:"
grep 'OUT_LOGS="out_logs_CPU_LOCAL_X_OPT/"' run.sh

# Update hybrid_dispatcher.cpp (comment the define)
sed -i 's/.*#define DIAG_CPU_LOCAL_X_OPT.*/\/\/ #define DIAG_CPU_LOCAL_X_OPT/' src/spmv_kernels/hybrid_dispatcher.cpp
echo "[CHECK] hybrid_dispatcher.cpp - DIAG_CPU_LOCAL_X_OPT line:"
grep '#define DIAG_CPU_LOCAL_X_OPT' src/spmv_kernels/hybrid_dispatcher.cpp
echo ""

echo ">>> Building and running normal mode..."
cd src/
make clean
time ./make.sh
cd ../
time ./run.sh

echo ""
echo "=========================================="
echo "2. SET 'LOCAL X' MODE"
echo "=========================================="

# Update run.sh (comment normal logs, uncomment local x logs)
sed -i 's/.*OUT_LOGS="out_logs\/".*/            # OUT_LOGS="out_logs\/"/' run.sh
echo "[CHECK] run.sh - out_logs/ line:"
grep 'OUT_LOGS="out_logs/"' run.sh

sed -i 's/.*OUT_LOGS="out_logs_CPU_LOCAL_X_OPT\/".*/            OUT_LOGS="out_logs_CPU_LOCAL_X_OPT\/"/' run.sh
echo "[CHECK] run.sh - out_logs_CPU_LOCAL_X_OPT/ line:"
grep 'OUT_LOGS="out_logs_CPU_LOCAL_X_OPT/"' run.sh

# Update hybrid_dispatcher.cpp (uncomment the define)
sed -i 's/.*#define DIAG_CPU_LOCAL_X_OPT.*/#define DIAG_CPU_LOCAL_X_OPT/' src/spmv_kernels/hybrid_dispatcher.cpp
echo "[CHECK] hybrid_dispatcher.cpp - DIAG_CPU_LOCAL_X_OPT line:"
grep '#define DIAG_CPU_LOCAL_X_OPT' src/spmv_kernels/hybrid_dispatcher.cpp
echo ""

echo ">>> Building and running local x mode..."
cd src/
make clean
time ./make.sh
cd ../
time ./run.sh
