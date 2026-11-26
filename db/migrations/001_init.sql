-- 001 init.sql
-- creates Tables

-- Users

CREATE TABLE users (
    id            INTEGER PRIMARY KEY,
    username      TEXT    NOT NULL UNIQUE,
    display_name  TEXT    NOT NULL,
    role          TEXT    NOT NULL CHECK (role IN ('driver', 'dispatcher', 'admin')),
    created_at    TEXT    NOT NULL DEFAULT (datetime('now'))
);

