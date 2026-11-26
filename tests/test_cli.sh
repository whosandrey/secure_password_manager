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

run_logged() {
    "$vault_app" 2>&1 >/dev/null
}

create_log=$(printf '1\nfake-master\nfake-master\ndemo-service\nalice-test\nfake-pass-a\nn\n' | run_logged)
add_log=$(printf '4\nfake-master\nmail-service\nbob-test\nfake-pass-b\n' | run_logged)
unlock_log=$(printf '2\nfake-master\n' | run_logged)
failed_log=$(printf '2\nwrong-master\n' | run_logged) || :

printf '%s\n' "$create_log" | grep -Fxq 'event=vault_create result=success'
printf '%s\n' "$add_log" | grep -Fxq 'event=vault_add_entry result=success'
if [ "$(stat -c '%a' passwords.vault)" != 600 ]; then
    echo "FAIL: vault permissions after adding an entry are not 0600" >&2
    exit 1
fi
printf '%s\n' "$unlock_log" | grep -Fxq 'event=vault_unlock result=success'
printf '%s\n' "$failed_log" | grep -Fxq 'event=vault_unlock result=failure'

if printf '%s\n' "$create_log" "$add_log" "$unlock_log" "$failed_log" |
    grep -Eq 'fake-master|wrong-master|fake-pass-[ab]|demo-service|mail-service|alice-test|bob-test'; then
    echo "FAIL: test data leaked to stderr" >&2
    exit 1
fi

echo "PASS: expected log events present, no test data in stderr"