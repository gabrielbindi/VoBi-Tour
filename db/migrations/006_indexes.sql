-- 006_indexes.sql
-- GiST on all geometry columns
CREATE INDEX idx_location_geom     ON location USING gist (geom);
CREATE INDEX idx_zone_area         ON zone     USING gist (area);
CREATE INDEX idx_zone_entry_point  ON zone     USING gist (entry_point);
CREATE INDEX idx_route_geom        ON route    USING gist (geom);

-- Foreign key indexes
-- 002 Stammdaten
CREATE INDEX idx_cargo_client_id          ON cargo (client_id);
CREATE INDEX idx_cargo_location_id        ON cargo (location_id);

-- 003 Tour
CREATE INDEX idx_tour_crew_crew_id        ON tour_crew (crew_id);
CREATE INDEX idx_tour_crew_co_driver_id   ON tour_crew (co_driver_id);
CREATE INDEX idx_tour_vehicle_tour_id     ON tour_vehicle (tour_id);
CREATE INDEX idx_tour_vehicle_vehicle_id  ON tour_vehicle (vehicle_id);
CREATE INDEX idx_break_tour_id            ON break (tour_id);
CREATE INDEX idx_break_crew_id            ON break (crew_id);
CREATE INDEX idx_zone_tour_id             ON zone (tour_id);

-- 004 Stop
CREATE INDEX idx_stop_tour_id             ON stop (tour_id);
CREATE INDEX idx_stop_location_id         ON stop (location_id);
CREATE INDEX idx_stop_zone_id             ON stop (zone_id);
CREATE INDEX idx_stop_crew_stop_id        ON stop_crew (stop_id);
CREATE INDEX idx_stop_crew_crew_id        ON stop_crew (crew_id);
CREATE INDEX idx_stop_crew_orig_assigned  ON stop_crew (originally_assigned_to);
CREATE INDEX idx_problem_stop_id          ON problem (stop_id);
CREATE INDEX idx_problem_reported_by      ON problem (reported_by);
CREATE INDEX idx_photo_stop_id            ON photo (stop_id);
CREATE INDEX idx_photo_problem_id         ON photo (problem_id);
CREATE INDEX idx_photo_crew_id            ON photo (crew_id);
CREATE INDEX idx_delivery_note_stop_id    ON delivery_note (stop_id);
CREATE INDEX idx_change_log_account_id    ON change_log (account_id);

-- 005 Routing
CREATE INDEX idx_route_tour_id            ON route (tour_id);
CREATE INDEX idx_route_warning_route_id   ON route_warning (route_id);