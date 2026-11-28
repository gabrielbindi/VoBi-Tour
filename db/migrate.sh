#!/usr/bin/env bash
# migrate.sh – starts the migration of the database schema and data
# Start: ./db/migrate.sh

set -e
cd "$(dirname "$0")/.."
source .env

# Start Database Container and wait for it to be ready
docker compose up -d --wait
