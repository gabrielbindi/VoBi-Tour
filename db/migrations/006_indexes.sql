-- 006_indexes.sql
-- GiST on all geometry columns
CREATE INDEX idx_location_geom     ON location USING gist (geom);
CREATE INDEX idx_zone_area         ON zone     USING gist (area);
CREATE INDEX idx_zone_entry_point  ON zone     USING gist (entry_point);
CREATE INDEX idx_route_geom        ON route    USING gist (geom);
