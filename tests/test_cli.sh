#!/bin/sh
set -eu

vault_app=$1
test_dir=$(mktemp -d)
trap 'rm -f -- "$test_dir/passwords.vault"; rmdir -- "$test_dir"' EXIT
cd "$test_dir"

if {
    printf '1\nfake-master\nfake-master\n'
    printf '%064d\n' 0
    printf 'alice\nfake-password\nn\n'
} | "$vault_app"; then
    echo "FAIL: overlong service was accepted" >&2
    exit 1
fi

if [ -e passwords.vault ]; then
    echo "FAIL: vault was created with invalid input" >&2
    exit 1
fi

echo "PASS: overlong service rejected, no vault created"