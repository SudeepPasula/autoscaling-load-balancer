# Autoscaling Load Balancer

CSCE 412 Project 4: a C++ simulation of a load balancer that hands web requests to a pool of web servers, adds or removes servers to keep the queue between 50 and 80 requests per server, and blocks requests from configured IP ranges.

## Build and run

```
make
./loadbalancer [config file] [-v | -q]
```

The program asks for the number of web servers and the number of clock cycles to run. `-v` logs events as they happen (scaling, blocked requests, status), and `-q` only prints the starting configuration and the final summary. Output goes to the terminal in color and to the log file named in the config.

## Config files

| File | Scenario |
|---|---|
| `config.txt` | Normal traffic, single load balancer |
| `config_stress.txt` | Traffic spike with 30% of requests from blocked IP ranges |
| `config_light.txt` | Light traffic, so the load balancer scales down |
| `config_switch.txt` | Bonus: a switch sends processing and streaming jobs to separate load balancers |

## Other make targets

- `make docs` generates the Doxygen HTML documentation in `docs/html`
- `make zip` packages the source, Makefile, and config files for submission
- `make clean` removes the executable and object files

`log_20servers_100000cycles.txt` is the quiet mode log from a 20 server, 100,000 cycle run.
