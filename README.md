# Distributed Query Engine

A C++ query engine that runs analytical queries over a 10,000,000+ row PostgreSQL database using sequential, multi-threaded, and distributed execution, with the distributed version running across multiple AWS EC2 nodes.

## Getting Started

### Prerequisites

* CMake 3.21+
* A C++20 capable compiler (MSVC, Clang, or GCC)
* Ninja
* Git
* PostgreSQL (local install or AWS RDS)

### Clone

This project uses `vcpkg` as a git submodule, so clone recursively:

```
git clone --recursive https://github.com/gjain-27/DistributedQueryEngine.git
cd DistributedQueryEngine
```

If you've already cloned without `--recursive`:

```
git submodule update --init --recursive
```

### Install dependencies (vcpkg)

Bootstrap vcpkg inside `extern/`:

```
# Windows
extern\vcpkg\bootstrap-vcpkg.bat

# Linux
./extern/vcpkg/bootstrap-vcpkg.sh
```

### Build (CMake)

Configure the project using a preset:

```
# Windows
cmake --preset x64-release

# Linux
cmake --preset linux-release
```

CMake will automatically install dependencies (libpqxx) from `vcpkg.json`.

Build:

```
# Windows
cmake --build out/build/x64-release

# Linux
cmake --build out/build/linux-release
```

### Database Setup

1. Create a database:

```
psql -U postgres -c "CREATE DATABASE dqe;"
```

2. Apply the schema:

```
psql -U postgres -d dqe -f sql/schema.sql
```

3. Fill in `.env` in the project root with your connection details:

```
DB_HOST=localhost
DB_PORT=5432
DB_NAME=dqe
DB_USER=postgres
DB_PASSWORD=your_password
```

### Generate Data

Run the data generator to populate the database. This clears any existing data first.

```
# Windows
out/build/x64-release/generator/DataGenerator.exe

# Linux
./out/build/linux-release/generator/DataGenerator
```

After generating, add the covering index used by the queries:

```
CREATE INDEX idx_transactions_store_covering ON transactions (store_id) INCLUDE (occurred_at, currency_code, total_amount);
VACUUM ANALYZE transactions;
```

### Run

Run sequential and multi-threaded execution:

```
# Windows
out/build/x64-release/DQE/DQE.exe

# Linux
./out/build/linux-release/DQE/DQE
```

Run as a distributed worker by passing the worker's index and the total number of workers:

```
./out/build/linux-release/DQE/DQE <workerIndex> <workerCount>
```

For example, with 3 workers, run `DQE 0 3`, `DQE 1 3`, and `DQE 2 3` on three separate machines at the same time. Each worker needs its own copy of the project built and a `.env` pointing at the shared database. Performance will vary depending on the database specs.

## Sample Output

```
Distributed query engine (worker 0/3, 2 threads): 5 queries run in 2778 ms
ARG month 1 | revenue: 51261967108 GBP | transactions: 127674
ARG month 2 | revenue: 45543970060 GBP | transactions: 114786
...
```

## Features

* Relational database - a normalised PostgreSQL schema with customers, products, stores, and transactions tables linked by foreign keys, with money stored as integer minor units (pence/cents) to avoid floating point errors.
* Synthetic data generator - generates 10,000,000+ transactions with category-based price ranges, multiple currencies (GBP, USD, EUR, JPY), and timestamps spread uniformly over two years. Uses PostgreSQL's COPY protocol (`stream_to` / `stream_from`) for fast bulk reads and writes.
* Country revenue report - calculates total revenue and transaction count per country per month, converting every currency to GBP before combining results.
* Sequential engine - runs each country's query one after another as a baseline.
* Multi-threaded engine - runs queries concurrently using a custom thread pool, with a dedicated database connection per task. Thread count defaults to the machine's hardware concurrency.
* Distributed engine - splits the countries across multiple machines, with each worker running its share using its own thread pool.
* Load balancing - countries are assigned to workers using a greedy algorithm based on each country's store count, so every worker receives a similar amount of work. Every worker computes the same assignment independently, so no coordination is needed.
* Query optimisation - a covering index lets PostgreSQL answer queries using index-only scans, without reading the full transactions table.
* Cross-platform builds - CMake presets for Windows (MSVC) and Linux (GCC), with dependencies managed through vcpkg.

## Deployment (AWS)

* Database - PostgreSQL hosted on AWS RDS.
* Workers - 3 AWS EC2 instances running Ubuntu, cloned from a custom AMI of a fully configured worker.
* Networking - EC2 and RDS share a VPC, so workers connect to the database over AWS's private network. Security groups restrict database access to the worker nodes.

## Planned Features

* Custom queries - allow users to define their own queries, such as revenue by product category or top customers by spend, instead of a single built-in report.
* Updated libpqxx APIs - replace deprecated `stream_from::query`, `stream_from::table`, and `exec0` calls with their newer equivalents.
* Coordinator - a single command that starts all workers at once and merges their results.
