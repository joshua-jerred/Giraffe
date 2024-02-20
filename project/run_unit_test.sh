set -e

docker compose run --rm build_host_unit_test

echo ""
/usr/games/cowsay "Target built, running unit test on host"
echo ""

ssh balloon "mkdir -p ~/unit_test"
scp build/gcc_docker_pi_zero/bin/tests/unit_test_on_pi_host balloon:~/unit_test/host
ssh balloon "cd ~/unit_test && ./host"