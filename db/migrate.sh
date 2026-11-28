#!/usr/bin/env bash
# migrate.sh – starts the migration of the database schema and data
# Start: ./db/migrate.sh

set -e
cd "$(dirname "$0")/.."
source .env

# Start Database Container and wait for it to be ready
docker compose up -d --wait

# Run SQL commands in the database container
psql() {
    docker compose exec -T db psql -U "$POSTGRES_USER" -d "$POSTGRES_DB" -v ON_ERROR_STOP=1 "$@"
}

# Crate Table wich contains the version of the data are alreyady in the database
psql -c "CREATE TABLE IF NOT EXISTS schema_migrations (
    version text PRIMARY KEY,
    applied_at timestamp NOT NULL DEFAULT now()
    );"