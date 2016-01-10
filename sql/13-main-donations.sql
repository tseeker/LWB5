-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 13-main-donations.sql
--
-- Initialises the part of the database that contains
-- data related to PayPal donations
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



-- Connect to the database
\c legacyworlds legacyworlds_admin



-- 
-- Create the ticket table
--
CREATE TABLE main.pp_ticket (
	md5_id	VARCHAR(64)	NOT NULL	PRIMARY KEY,
	account	BIGINT		NOT NULL	REFERENCES main.account (id),
	created	INT		NOT NULL	DEFAULT UNIX_TIMESTAMP(NOW())
);

CREATE INDEX pp_tick_acnt ON main.pp_ticket(account);
GRANT SELECT,INSERT,DELETE ON TABLE main.pp_ticket TO legacyworlds;


--
-- Create the donations history table
--
CREATE TABLE main.pp_history (
	account	BIGINT		NOT NULL	REFERENCES main.account (id),
	donated	INT		NOT NULL	DEFAULT UNIX_TIMESTAMP(NOW()),
	amount	FLOAT		NOT NULL,
	PRIMARY KEY (account, donated)
);

GRANT SELECT,INSERT ON TABLE main.pp_history TO legacyworlds;


--
-- Create the Paypal IPN log table
--
CREATE TABLE main.pp_ipn (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	received	INT		NOT NULL	DEFAULT UNIX_TIMESTAMP(NOW()),
	receiver_email	VARCHAR(127),
	item_number	VARCHAR(127),
	payment_status	VARCHAR(20),
	pending_reason	VARCHAR(10),
	payment_date	VARCHAR(20),
	mc_gross	VARCHAR(20),
	mc_fee		VARCHAR(20),
	tax		VARCHAR(20),
	mc_currency	VARCHAR(3),
	txn_id		VARCHAR(20),
	txn_type	VARCHAR(10),
	payer_email	VARCHAR(127),
	payer_status	VARCHAR(10),
	payment_type	VARCHAR(10),
	verify_sign	VARCHAR(10),
	referrer_id	VARCHAR(10)
);

GRANT SELECT,INSERT ON TABLE main.pp_ipn TO legacyworlds;
GRANT SELECT,UPDATE ON main.pp_ipn_id_seq TO legacyworlds;
