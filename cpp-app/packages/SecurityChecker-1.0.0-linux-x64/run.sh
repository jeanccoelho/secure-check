#!/bin/bash
cd "$(dirname "$0")"
echo "🛡️  SecurityChecker"
echo "=================="
echo ""
if [[ $EUID -ne 0 ]]; then
    echo "⚠️  Este programa precisa ser executado como root."
    echo "   Execute: sudo ./run.sh"
    exit 1
fi
./SecurityChecker