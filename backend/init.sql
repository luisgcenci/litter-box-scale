-- Runs automatically the first time TimescaleDB starts with an empty volume.
-- For an existing database: docker exec -i timescaledb psql -U postgres < init.sql

CREATE EXTENSION IF NOT EXISTS timescaledb;

CREATE TABLE IF NOT EXISTS readings (
    time    TIMESTAMPTZ      NOT NULL,
    message DOUBLE PRECISION NOT NULL  -- weight in grams
);

SELECT create_hypertable('readings', by_range('time'), if_not_exists => TRUE);
