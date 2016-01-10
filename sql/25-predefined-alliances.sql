-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 25-predefined-alliances.sql
--
-- Create the table for predefined alliances and insert
-- its contents.
--
-- Copyright(C) 2004-2008, DeepClone Development
-- --------------------------------------------------------


-- Connect to the database in ADMIN mode
\c legacyworlds legacyworlds_admin


--
-- Create the definition table
--

CREATE TABLE main.default_alliance (
	tag		VARCHAR(5)	NOT NULL		PRIMARY KEY,
	name		VARCHAR(64)	NOT NULL,
	html_color	CHAR(6)		NOT NULL		UNIQUE
);

GRANT SELECT ON main.default_alliance TO legacyworlds;


--
-- Insert default alliances
--
COPY main.default_alliance FROM STDIN;
-R-	Red team	ff0000
-G-	Green team	00ff00
-B-	Blue team	0000ff
-C-	Teal team	007f7f
-P-	Purple team	7f007f
-Y-	Yellow team	afaf00
-O-	Orange team	ffaf3f
-A-	Aquamarine team	003f7f
\.
