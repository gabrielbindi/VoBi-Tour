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

# All SQL files in the db/migrations directory
for file in db/migrations/*.sql; do
    name=$(basename "$file")
    version=$(psql -tA -c "SELECT 1 FROM schema_migrations WHERE version = '$name';")
    if [ "$version" = "1" ]; then
        echo "Skipping migration $name (already applied)"
    else
        echo "Applying migration $name"
        psql -1 -f "/db/migrations/$name" \
            -c "INSERT INTO schema_migrations (version) VALUES ('$name');"
    fi
done

echo "Migration completed successfully."