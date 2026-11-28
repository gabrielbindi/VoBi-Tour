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

---

## Running migrations

```bash
./db/migrate.sh
```

**Rules**

- Never edit a migration that has already been applied.
- Every change must be a new numbered file (`007_…sql`, `008_…sql`, …).
- Applied migrations are recorded in the `schema_migrations` table.
- Running the migrations twice must not change anything.
