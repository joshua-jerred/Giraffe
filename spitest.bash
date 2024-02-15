set -e

docker compose run --rm build_spi_test

ssh balloon "mkdir -p ~/unit_test"
scp build/gcc_docker_pi_zero/tests/spitest balloon:~/unit_test/spitest
ssh balloon "cd ~/unit_test && ./spitest"
