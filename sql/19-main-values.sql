-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 19-main-values.sql
--
-- Insert data into some of the main tables
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



-- Connect to the database in ADMIN mode
\c legacyworlds legacyworlds_admin


--
-- Peacekeepers AI
--

INSERT INTO main.account (name, email, password, status, vac_credits, last_login, last_logout, admin)
	VALUES ('AI>Peacekeepers', 'not-a-valid-email@not-a-valid-domain.com',
		'[-jv&VCR3B-b}F75qS["lpBDk8C[v~DU78Oc}=6WROXt)b+&U7[ZbNRb[d0', 'STD', 240,
		UNIX_TIMESTAMP(NOW()), UNIX_TIMESTAMP(NOW()) + 1, 't');


--
-- Words banned from the manual's index
--
COPY main.man_index_ban FROM STDIN;
en	-
en	a
en	am
en	an
en	as
en	at
en	and
en	are
en	be
en	being
en	by
en	for
en	had
en	has
en	have
en	he
en	if
en	in
en	into
en	is
en	it
en	its
en	it's
en	of
en	on
en	or
en	out
en	she
en	that
en	the
en	then
en	these
en	this
en	those
en	thus
en	to
en	was
en	were
en	with
en	what
en	when
en	where
en	which
en	who
en	you
en	your
\.


COPY main.f_smiley FROM STDIN;
:-?\\)	smile
[:;]-?p	razz
:-?D	lol
[:;]-&gt;	biggrin
;-?\\)	wink
;-?D	mrgreen
:-?\\(	sad
:evil:	evil
:smile:	smile
:happy:	smile
:wink:	wink
:sad:	sad
:unhappy:	sad
:'\\(	cry
:cry:	cry
:crying:	cry
:grin:	biggrin
:lol:	lol
:tongue:	razz
:rofl:	mrgreen
[:;]-?\\|	neutral
:neutral:	neutral
\.


COPY main.f_code FROM STDIN;
\\[b\\](.*?)\\[\\/b\\]	<b>$1</b>
\\[u\\](.*?)\\[\\/u\\]	<u>$1</u>
\\[i\\](.*?)\\[\\/i\\]	<i>$1</i>
\\[sep(arator)?\\]	<hr/>
\\[item\\](.*?)\\[\\/item\\]	<ul><li>$1</li></ul>
\\[quote\\](.*?)\\[\\/quote\\]	<blockquote class="quote">$1</blockquote>
\\[quote=([^\\]]+)\\](.*?)\\[\\/quote\\]	<blockquote class="quote"><b>$1</b> said:<br/>$2</blockquote>
\\[link=(http[^\\]]+)\\](.+?)\\[\\/link\\]	<a href="$1" target="_blank">$2</a>
\\[code\\](.*?)\\[\\/code\\]	<pre>$1</pre>
<\\/li><\\/ul>\\s*(<br\\/>\\s*)*<ul><li>	</li><li>
\\[manual\\](.*?)\\[\\/manual\\]	<a href="manual" target="_blank">$1</a>
\\[manual=(\\w+)(#\\w+)?\\](.*?)\\[\\/manual\\]	<a href="manual?p=$1$2" target="_blank">$3</a>
\\[topic=(\\d+)\\](.*?)\\[\\/topic\\]	<a href="forums?cmd=T%23G%23$1" target="_blank">$2</a>
\.


-- Connect to the database in USER mode
\c legacyworlds legacyworlds


--
-- Default values for user preferences
--
COPY main.user_preferences (id, version, value) FROM STDIN;
colour	main	red
font_size	main	2
forums_nitems	main	20
forums_ntopics	main	20
forums_reversed	main	1
forums_threaded	main	1
forum_code	main	1
smileys	main	1
tooltips	main	2
\.
