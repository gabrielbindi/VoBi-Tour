## Database Setup (Docker)

We use Docker so that everyone has the same PostgreSQL, PostGIS and pgRouting versions without installing anything locally. The repository stays small because the database itself is not committed.

### Start the database

```bash
cp .env.example .env          # then set a password in .env
docker compose up -d
docker compose ps             # status should be "healthy"
docker compose exec db psql -U vobitour -d vobitour
```

**What the commands do**

1. `cp .env.example .env`  
   Copies the example environment file.  
   → Edit `.env` and set a real value for `POSTGRES_PASSWORD`.

2. `docker compose up -d`  
   Starts the PostgreSQL container (with PostGIS + pgRouting) in the background.  
   Image, volume, port and credentials come from `docker-compose.yml` and `.env`.

3. `docker compose ps`  
   Shows the status of the containers.  
   The `db` container should show `healthy` before you continue.

4. `docker compose exec db psql -U vobitour -d vobitour`  
   Opens an interactive `psql` session inside the running container  
   (user: `vobitour`, database: `vobitour`).

### Other useful commands

- `docker compose down` – stop the database (data is kept)
- `docker compose down -v` – stop the database **and delete all data** (empty database again)

> If `docker compose up` says port 5432 is already in use, a local PostgreSQL is running.  
> Set `POSTGRES_PORT=5433` in `.env` and try again.

---

## Using psql

Once you are in the `psql` prompt (`vobitour=#`):

| Command                          | Description                              |
|----------------------------------|------------------------------------------|
| `\l`                             | List all databases                       |
| `\dt`                            | List all tables                          |
| `\d tablename`                   | Show structure of a table                |
| `\dx`                            | List installed extensions                |
| `\q`                             | Quit psql                                |
| `SELECT version();`              | PostgreSQL version                       |
| `SELECT PostGIS_Version();`      | PostGIS version                          |
| `SELECT pgr_version();`          | pgRouting version                        |

Extensions are created by the migrations. You normally do **not** need to run these by hand:

```sql
CREATE EXTENSION postgis;
CREATE EXTENSION pgrouting;
```

**Useful `psql` options**

| Option          | Description                                                                 |
|-----------------|-----------------------------------------------------------------------------|
| `-c "..."`      | Execute the given SQL command and exit (non-interactive)                    |
| `-t`            | Tuples only – output only the data, no column headers or footer             |
| `-A`            | Unaligned output – no formatting (no `|` separators or extra spaces)        |
| `-f file.sql`   | Execute the SQL commands from the given file                                |
| `-1`            | Run everything in a single transaction (`BEGIN` … `COMMIT`)                 |

---

## PostGIS geometry types used

| Type | Meaning | Used in |
|------|---------|---------|
| `geometry(Point, 4326)` | A single point (e.g. stop location, depot, zone entry) | `location.geom`, `zone.entry_point` |
| `geometry(LineString, 4326)` | A line made of connected points (the calculated route) | `route.geom` |
| `geometry(Polygon, 4326)` | A closed area (zone boundary) | `zone.area` |

**SRID 4326** = WGS 84 (standard GPS coordinates: longitude/latitude).

### Documentation

- PostGIS geometry types:  
  https://postgis.net/docs/using_postgis_dbmanagement.html#RefObject

- `geometry` type reference:  
  https://postgis.net/docs/geometry.html

- Spatial reference systems (SRIDs):  
  https://postgis.net/docs/using_postgis_dbmanagement.html#spatial_ref_sys

---

## Running migrations

From the repository root:

```bash
./db/migrate.sh
```

Applied migrations are recorded in the `schema_migrations` table.  
Running the script twice does nothing.

### Migration files

| File | Tables / Content |
|------|------------------|
| `001_extensions.sql` | `postgis`, `pgrouting` |
| `002_master_data.sql` | `crew_member`, `account`, `vehicle`, `client`, `location`, `cargo` |
| `003_tour.sql` | `tour`, `tour_crew`, `tour_vehicle`, `break`, `zone` |
| `004_stop.sql` | `stop`, `stop_crew`, `problem`, `photo`, `delivery_note`, `change_log` |
| `005_routing.sql` | `route`, `route_warning`, `street_attribute`, `street_override` |
| `006_indexes.sql` | GiST indexes, foreign-key indexes, OSM ID index |

### Rules

- Never edit a migration that has already been applied.
- Every change must be a new numbered file (`007_….sql`, `008_….sql`, …).
- Do not put empty files into `db/migrations/`.
- Constraint indexes (e.g. “exactly one start stop per tour”) belong next to their table.  
  Performance indexes (GiST, foreign keys, OSM lookups) belong in `006_indexes.sql`.

### Creating a new migration

1. Take the next free number (e.g. `007_add_capacity.sql`).
2. Write the SQL.
3. Run `./db/migrate.sh`.
4. Regenerate `schema.sql` (see below) and commit both files.

---

## schema.sql

`db/schema.sql` is a read-only dump of the current schema.  
It is **never** applied – it exists only so you can look at the full schema without a running database.

Generate it after a successful migration run:

```bash
docker compose exec db pg_dump -U vobitour -d vobitour --schema-only --no-owner --no-privileges > db/schema.sql
```

---

## Indexes (`006_indexes.sql`)

This migration adds performance indexes.  
They make queries faster but do **not** enforce any rules  
(constraint indexes such as “exactly one start stop per tour” are created together with their tables in `004`/`005`).

### GiST indexes on all geometry columns

- `location.geom`
- `zone.area`
- `zone.entry_point`
- `route.geom`

A normal B-tree index can only sort and compare values.  
Spatial queries such as “which stops are inside this zone?” (`ST_Within`) or “what is nearby?” are only fast when a GiST index exists.

### Foreign-key indexes

PostgreSQL does **not** automatically create an index for a foreign key (`REFERENCES`).  
Without an index, a query like “all stops of tour 5” has to scan the whole `stop` table.  
With many tours and up to 400 stops per tour this becomes slow.  
Therefore an index is created on every foreign-key column.

**Official documentation**

- [PostgreSQL: Indexes on Foreign Keys](https://www.postgresql.org/docs/current/ddl-constraints.html#DDL-CONSTRAINTS-FK)  
  > “PostgreSQL does not automatically create an index on the referencing columns of a foreign key constraint. […] It is usually a good idea to index the referencing columns […]”

- [PostgreSQL: CREATE INDEX](https://www.postgresql.org/docs/current/sql-createindex.html)

### OSM IDs

```sql
CREATE INDEX idx_location_osm_section
    ON location (way_osm_id, from_node_osm_id, to_node_osm_id);
```

Used to look up addresses by their street segment after an OSM import.

### What is intentionally not indexed again

PostgreSQL already creates an index for `PRIMARY KEY` and `UNIQUE` constraints.  
These columns are therefore skipped:

- `account.crew_id` (UNIQUE)
- `tour_crew (tour_id, crew_id)` (UNIQUE)
- `street_attribute.osm_way_id` (part of PRIMARY KEY)
- `street_override.osm_way_id` (part of UNIQUE)

### Naming

All indexes follow the pattern:

```text
idx_<table>_<column>
```
