-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/99-player-fk.sql
--
-- Beta 5 games:
-- Foreign keys on the player table
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------

ALTER TABLE player ADD FOREIGN KEY (alliance) REFERENCES alliance (id) ON DELETE SET NULL;
ALTER TABLE player ADD FOREIGN KEY (a_grade) REFERENCES alliance_grade (id) ON DELETE SET NULL;
ALTER TABLE player ADD FOREIGN KEY (a_vote) REFERENCES alliance_candidate (id) ON DELETE SET NULL;
ALTER TABLE player ADD FOREIGN KEY (first_planet) REFERENCES planet (id);
